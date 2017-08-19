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

#include "whitefield.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

#define WF_NETAPI_MSG_QUEUE_SIZE   (8U)
#define WF_PRIO                    (THREAD_PRIORITY_MAIN - 1)

kernel_pid_t gnrc_whitefield_6lowpan_pid;
static char _stack[(THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)];

//static uint8_t _sendbuf[BLE_SIXLOWPAN_MTU];

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

    ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = gnrc_whitefield_6lowpan_pid;

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
	(void)pkt;
#if 0
    if (pkt == NULL) {
        LOG("_send_ble: pkt was NULL\n");
        return -EINVAL;
    }

    gnrc_netif_hdr_t *netif_hdr;
    gnrc_pktsnip_t *payload = pkt->next;
    uint8_t *dst;

    if (ble_mac_busy_tx) {
        LOG("_send(): ble_mac_busy_tx\n");
        return -EBUSY;
    }

    uint8_t *buf = _sendbuf;
    unsigned len = 0;

    if (pkt->type != GNRC_NETTYPE_NETIF) {
        LOG("_send_ble: first header is not generic netif header\n");
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

    /* prepare packet for sending */
    while (payload) {
        memcpy(buf, payload->data, payload->size);
        len += payload->size;
        buf +=  payload->size;
        payload = payload->next;
    }

    gnrc_pktbuf_release(pkt);

    if (ble_mac_send(dst, _sendbuf, len) == -1) {
        return -EBADMSG;
    }
#endif
    return 0;
}
static int _handle_get(gnrc_netapi_opt_t *_opt)
{
    int res = -ENOTSUP;
    uint8_t *value = _opt->data;

    switch (_opt->opt) {
        case NETOPT_ACK_REQ:
        case NETOPT_CHANNEL:
        case NETOPT_NID:
        case NETOPT_ADDRESS:
            /* -ENOTSUP */
            break;
        case NETOPT_ADDRESS_LONG:
              assert(_opt->data_len >= IEEE802154_LONG_ADDRESS_LEN);
 //           memcpy(value, _own_mac_addr, BLE_SIXLOWPAN_L2_ADDR_LEN);
  //          value[0] = IPV6_IID_FLIP_VALUE;
              res = IEEE802154_LONG_ADDRESS_LEN;
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(_opt->data_len == sizeof(uint16_t));
            *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
            res = sizeof(uint16_t);
            break;
#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            assert(_opt->data_len == sizeof(gnrc_nettype_t));
            *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
            res = sizeof(gnrc_nettype_t);
            break;
#endif
/*        case NETOPT_DEVICE_TYPE:
            assert(_opt->data_len == sizeof(uint16_t));
            *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
            res = sizeof(uint16_t);
            break;*/
        case NETOPT_IPV6_IID:
//            memcpy(value, _own_mac_addr, BLE_SIXLOWPAN_L2_ADDR_LEN);
//            value[0] = IPV6_IID_FLIP_VALUE;
//            res = BLE_SIXLOWPAN_L2_ADDR_LEN;
            break;
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

    LOG("gnrc_whitefield_6lowpan: starting thread\n");

    gnrc_whitefield_6lowpan_pid = thread_getpid();

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
                LOG("gnrc_whitefield_6lowpan: GNRC_NETAPI_MSG_TYPE_SND received\n");
                _send(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                /* read incoming options */
                opt = msg.content.ptr;
                LOG("gnrc_whitefield_6lowpan: GNRC_NETAPI_MSG_TYPE_SET received. opt=%s\n",
                      netopt2str(opt->opt));
                /* set option for device driver */
                res = ENOTSUP;
                LOG("gnrc_whitefield_6lowpan: response of netdev->set: %i\n", res);
                /* send reply to calling thread */
                reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)res;
                msg_reply(&msg, &reply);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                /* read incoming options */
                opt = msg.content.ptr;
                LOG("gnrc_whitefield_6lowpan: GNRC_NETAPI_MSG_TYPE_GET received. opt=%s\n",
                      netopt2str(opt->opt));
                res = _handle_get(opt);
                LOG("gnrc_whitefield_6lowpan: response of netdev->get: %i\n", res);
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
    kernel_pid_t res = thread_create(_stack, sizeof(_stack), WF_PRIO,
                        THREAD_CREATE_STACKTEST,
                        _gnrc_whitefield_6lowpan_thread, NULL,
                        "ble");
    assert(res > 0);
	LOG("Whitefield 6Lowpan Thread created. Nodeid=0x%x\n", wf_get_nodeid());
    (void)res;
}
