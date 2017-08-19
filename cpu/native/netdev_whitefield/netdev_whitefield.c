/*
 * Copyright (C) 2015 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>,
 *                    Martine Lenders <mlenders@inf.fu-berlin.de>
 *                    Kaspar Schleiser <kaspar@schleiser.de>
 *                    Ell-i open source co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/*
 * @ingroup netdev
 * @{
 * @brief   Low-level 802154 driver for whitefield interfaces
 * @author  Rahul Jadhav <nyrahul@gmail.com>
 * @}
 */
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* needs to be included before native's declarations of ntohl etc. */
#include "byteorder.h"

#include "native_internal.h"

#include "async_read.h"
#include "net/eui64.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "netdev_whitefield.h"
#include "net/netopt.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/* netdev interface */
static int _init(netdev_t *netdev);
static int _send(netdev_t *netdev, const struct iovec *vector, unsigned n);
static int _recv(netdev_t *netdev, void *buf, size_t n, void *info);

static inline void _isr(netdev_t *netdev)
{
	netdev_whitefield_t *wfdev=(netdev_whitefield_t*)netdev;
	netdev_t *dev = &wfdev->dev802154.netdev;
    if (dev->event_callback) {
        dev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
#if DEVELHELP
    else {
        puts("netdev_whitefield: _isr(): no event_callback set.");
    }
#endif
}

static int _get_addr(netdev_whitefield_t *wfdev, uint8_t *value)
{
	uint16_t nworderid=htons(wfdev->nodeid);
	memcpy(value, &nworderid, sizeof(nworderid));
	return 0;
}

static int _get(netdev_t *dev, netopt_t opt, void *value, size_t max_len)
{
	netdev_whitefield_t *wfdev=(netdev_whitefield_t*)dev;
    int res = 0;

	switch(opt) {
		case NETOPT_ADDRESS:
			assert(max_len >= IEEE802154_SHORT_ADDRESS_LEN);
			_get_addr(wfdev, value);
			return IEEE802154_SHORT_ADDRESS_LEN;

		case NETOPT_ADDRESS_LONG:
			assert((max_len >= IEEE802154_LONG_ADDRESS_LEN))
			memset(value, 0, IEEE802154_LONG_ADDRESS_LEN);
			_get_addr(wfdev, (uint8_t*)value+6);
			return IEEE802154_LONG_ADDRESS_LEN;

		default:
			res = netdev_ieee802154_get(&wfdev->dev802154, opt, value, max_len);
			break;
	}
    return res;
}

static int _set(netdev_t *dev, netopt_t opt, void *val, size_t val_len)
{
	netdev_whitefield_t *wfdev=(netdev_whitefield_t*)dev;
    int res = 0;

	switch(opt) {
		case NETOPT_ADDRESS:
			assert(val_len == IEEE802154_SHORT_ADDRESS_LEN);
			DEBUG("DIDNT EXPECT! Assigning new short address\n");
			memcpy(wfdev->dev802154.short_addr, val, val_len);
			memcpy(&wfdev->nodeid, val, val_len);
			return IEEE802154_SHORT_ADDRESS_LEN;

		case NETOPT_ADDRESS_LONG:
			DEBUG("DIDNT EXPECT LONG ADDRESS ASSIGNMENT!\n");
			return IEEE802154_LONG_ADDRESS_LEN;

		default:
			res = netdev_ieee802154_set(&wfdev->dev802154, opt, val, val_len);
			break;
	}

    return res;
}

static netdev_driver_t netdev_driver_whitefield = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

/* driver implementation */
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    netdev_whitefield_t *wfdev = (netdev_whitefield_t*)netdev;
    (void)info;
	(void)wfdev;
	(void)buf;
	(void)len;
    DEBUG("whitefield recv called len=%d\n", len);

    return -1;
}

static int _send(netdev_t *netdev, const struct iovec *vector, unsigned n)
{
    netdev_whitefield_t *wfdev = (netdev_whitefield_t*)netdev;
    netdev_t *dev = &wfdev->dev802154.netdev;
    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV_EVENT_TX_COMPLETE);
    }
    size_t bytes = 0;
    for (unsigned i = 0; i < n; i++) {
        bytes += vector->iov_len;
        vector++;
    }
#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += bytes;
#endif
    DEBUG("whitefield send called bytes=%d\n", bytes);
    return bytes;
}

void netdev_whitefield_setup(netdev_whitefield_t *wfdev, const netdev_whitefield_params_t *params) {
    wfdev->dev802154.netdev.driver = &netdev_driver_whitefield;
	wfdev->nodeid = (uint16_t) params->nodeid;
	DEBUG("whitefield setup called nodeid:%d\n", wfdev->nodeid);
}

static int _init(netdev_t *netdev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    netdev_whitefield_t *wfdev = (netdev_whitefield_t*)netdev;
	netdev_t *dev = &wfdev->dev802154.netdev;

    /* check device parametrs */
    if (wfdev == NULL) {
        return -ENODEV;
    }

#ifdef MODULE_NETSTATS_L2
    memset(&dev->stats, 0, sizeof(netstats_t));
#endif
#ifdef MODULE_GNRC_SIXLOWPAN
    wfdev->dev802154.proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    wfdev->dev802154.proto = GNRC_NETTYPE_UNDEF;
#endif
    wfdev->dev802154.seq = 0;
    wfdev->dev802154.flags = 0;
    DEBUG("whitefield initialized.\n");
    return 0;
}
