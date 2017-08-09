/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for whitefield devices
 *
 * @author  Rahul Jadhav <nyrahul@gmail.com>
 */

#ifdef MODULE_NETDEV_WHITEFIELD

#include "log.h"
#include "debug.h"
#include "netdev_whitefield_params.h"
#include "net/gnrc/netdev/ieee802154.h"

#define WHITEFIELD_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define WHITEFIELD_MAC_PRIO                (THREAD_PRIORITY_MAIN - 3)

static netdev_whitefield_t netdev_whitefield[NETDEV_WHITEFIELD_MAX];
static char _netdev_802154_stack[NETDEV_WHITEFIELD_MAX][WHITEFIELD_MAC_STACKSIZE + DEBUG_EXTRA_STACKSIZE];
static gnrc_netdev_t _gnrc_netdev_whitefield[NETDEV_WHITEFIELD_MAX];

void auto_init_netdev_whitefield(void)
{
    for (unsigned i = 0; i < NETDEV_WHITEFIELD_MAX; i++) {
        const netdev_whitefield_params_t *p = &netdev_whitefield_params[i];

        LOG_DEBUG("[auto_init_netif] initializing netdev_whitefield #%u on WHITEFIELD\n", i);

        netdev_whitefield_setup(&netdev_whitefield[i], p);
        gnrc_netdev_ieee802154_init(&_gnrc_netdev_whitefield[i], 
						(netdev_ieee802154_t*)&netdev_whitefield[i]);

        gnrc_netdev_init(_netdev_802154_stack[i], WHITEFIELD_MAC_STACKSIZE,
                         WHITEFIELD_MAC_PRIO, "gnrc_netdev_whitefield",
                         &_gnrc_netdev_whitefield[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NETDEV_WHITEFIELD */
/** @} */
