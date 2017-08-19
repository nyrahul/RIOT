/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Glue for Whitefield-Framework
 *
 * @author      Rahul Jadhav <nyrahul@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "msg.h"
#include "net/gnrc.h"
#include "net/gnrc/nettype.h"
#include "net/ieee802154.h"
#ifdef MODULE_NETSTATS_L2
#include "net/netstats.h"
#endif

#include "whitefield.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

#define WF_NETAPI_MSG_QUEUE_SIZE   (8U)
#define WF_PRIO                    (THREAD_PRIORITY_MAIN - 1)

kernel_pid_t g_wf_pid;
static char g_stack[(THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)];
#ifdef MODULE_NETSTATS_L2
static netstats_t g_l2_stats;
#endif

#if 0
static void _handle_raw_sixlowpan(ble_mac_inbuf_t *inbuf)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, inbuf->payload,
            inbuf->len,
            GNRC_NETTYPE_SIXLOWPAN);

    if(!pkt) {
        LOG("_handle_raw_sixlowpan(): no space left in packet buffer.\n");
        return;
    }

    /* create netif header */
    gnrc_pktsnip_t *netif_hdr;
    netif_hdr = gnrc_pktbuf_add(NULL, NULL,
            sizeof(gnrc_netif_hdr_t) + (2 * sizeof(eui64_t)),
            GNRC_NETTYPE_NETIF);

    if (netif_hdr == NULL) {
        LOG("_handle_raw_sixlowpan(): no space left in packet buffer.\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = g_wf_pid;

    LOG("_handle_raw_sixlowpan(): received packet from %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x "
            "of length %d\n",
            inbuf->src[0], inbuf->src[1], inbuf->src[2], inbuf->src[3], inbuf->src[4],
            inbuf->src[5], inbuf->src[6], inbuf->src[7], inbuf->len);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(inbuf->payload, inbuf->len, OD_WIDTH_DEFAULT);
#endif

    LL_APPEND(pkt, netif_hdr);

    /* throw away packet if no one is interested */
    if (!gnrc_netapi_dispatch_receive(pkt->type, GNRC_NETREG_DEMUX_CTX_ALL, pkt)) {
        LOG("_handle_raw_sixlowpan: unable to forward packet of type %i\n", pkt->type);
        gnrc_pktbuf_release(pkt);
    }
}
#endif

static int _send(gnrc_pktsnip_t *pkt)
{
	uint8_t _sendbuf[WF_MAX_PKT_SZ];

    if (pkt == NULL) {
        ERR("_send: pkt was NULL\n");
        return -EINVAL;
    }

    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload = pkt->next;
    uint8_t *dst;

    uint8_t *buf = _sendbuf;
    unsigned len = 0;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        ERR("_send: first header is not generic netif header\n");
        return -EBADMSG;
    }

    netif_hdr = pkt->data;

    /* prepare destination address */
    if (netif_hdr->flags &
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        dst = NULL;
    }
    else {
        dst = gnrc_netif_hdr_get_dst_addr(netif_hdr);
    }
	(void)dst;

    /* prepare packet for sending */
    while (payload) {
		assert(len+payload->size<sizeof(_sendbuf));
        memcpy(buf, payload->data, payload->size);
        len += payload->size;
        buf +=  payload->size;
        payload = payload->next;
    }

    gnrc_pktbuf_release(pkt);
#ifdef	MODULE_NETSTATS_L2
	g_l2_stats.tx_bytes += len;
	if(dst) {
		g_l2_stats.tx_unicast_count++;
	} else {
		g_l2_stats.tx_mcast_count++;
	}
#endif
	LOG("Sending pkt of sz:%d\n", len);

    return 0;
}

static int _handle_set(gnrc_netapi_opt_t *_opt)
{
    int res = -ENOTSUP;
    uint8_t *value = _opt->data;

    switch (_opt->opt) {
        case NETOPT_SRC_LEN:
            assert(_opt->data_len == sizeof(uint16_t));
            res = sizeof(uint16_t);
            switch ((*(uint16_t *)value)) {
                case IEEE802154_SHORT_ADDRESS_LEN:
                    LOG("SHORT ADDRESS LEN set\n");
                    break;
                case IEEE802154_LONG_ADDRESS_LEN:
                    LOG("LONG ADDRESS LEN set\n");
                    break;
                default:
                    res = -EAFNOSUPPORT;
                    break;
            }
            break;
        default:
            break;
    }
    return res;
}

static int _handle_get(gnrc_netapi_opt_t *_opt)
{
    int res = -ENOTSUP;
    uint8_t *value = _opt->data;

    switch (_opt->opt) {
        case NETOPT_IPV6_IID:
        case NETOPT_ADDRESS_LONG:
			wf_get_longaddr(value, _opt->data_len);
            res = IEEE802154_LONG_ADDRESS_LEN;
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(_opt->data_len == sizeof(uint16_t));
            *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
            res = sizeof(uint16_t);
            break;
		case NETOPT_MAX_PACKET_SIZE:
			assert(_opt->data_len >= 2);
			*((uint16_t*)value) = WF_MAX_PKT_SZ;
			res = sizeof(uint16_t);
			break;
#ifdef MODULE_NETSTATS_L2
        case NETOPT_STATS:
            assert(_opt->data_len == sizeof(uintptr_t));
            *((netstats_t **)value) = &g_l2_stats;
            res = sizeof(uintptr_t);
            break;
#endif
#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            assert(_opt->data_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
            res = sizeof(gnrc_nettype_t);
            break;
#endif
        default:
            break;
    }
    return res;
}

/**
 * @brief   Startup code and event loop of the gnrc_whitefield_6lowpan layer
 *
 * @return          never returns
 */
static void *_gnrc_whitefield_6lowpan_thread(void *args)
{
	(void)args;
#ifdef MODULE_NETSTATS_L2
    memset(&g_l2_stats, 0, sizeof(g_l2_stats));
#endif

    LOG("gnrc_whitefield_6lowpan: starting thread\n");

    g_wf_pid = thread_getpid();

    gnrc_netapi_opt_t *opt;
    int res;
    msg_t msg, reply, msg_queue[WF_NETAPI_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, WF_NETAPI_MSG_QUEUE_SIZE);

    /* register the device to the network stack*/
    gnrc_netif_add(thread_getpid());

    /* start the event loop */
    while (1) {
//        LOG("gnrc_whitefield_6lowpan: waiting for incoming messages\n");
        msg_receive(&msg);
        /* dispatch NETDEV and NETAPI messages */
        switch (msg.type) {
#if 0
            case BLE_EVENT_RX_DONE:
                {
                    LOG("ble rx:\n");
                    _handle_raw_sixlowpan(msg.content.ptr);
                    ble_mac_busy_rx = 0;
                    break;
                }
#endif
            case GNRC_NETAPI_MSG_TYPE_SND:
                _send(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* read incoming options */
                opt = msg.content.ptr;
                /* set option for device driver */
                res = _handle_set(opt);
                LOG("SET received. opt=%s res:%i\n", netopt2str(opt->opt), res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* read incoming options */
                opt = msg.content.ptr;
                res = _handle_get(opt);
                //LOG("GET: opt=%s res:%i\n", netopt2str(opt->opt), res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            default:
                LOG("gnrc_whitefield_6lowpan: Unknown command %" PRIu16 "\n", msg.type);
                break;
        }
    }
    /* never reached */
    return NULL;
}

void gnrc_whitefield_6lowpan_init(void)
{
	if(wf_get_nodeid() == 0xffff) {
		ERR("Nodeid not passed white execution. -w <nodeid>\n");
		abort();
	}
    kernel_pid_t res = thread_create(g_stack, sizeof(g_stack), WF_PRIO,
                        THREAD_CREATE_STACKTEST,
                        _gnrc_whitefield_6lowpan_thread, NULL,
                        "whitefield");
    assert(res > 0);
	LOG("Whitefield 6Lowpan Thread created. Nodeid=0x%x\n", wf_get_nodeid());
    (void)res;
}
