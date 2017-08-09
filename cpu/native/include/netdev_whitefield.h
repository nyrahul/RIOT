/*
 * Copyright (C) 2015 Rahul Jadhav <nyrahul@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     netdev
 * @brief       Low-level ieee802154 driver for native whitefield interfaces
 * @{
 *
 * @file
 * @brief       Definitions for @ref netdev ieee802154 driver for host system's
 *              Whitefield interfaces
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef NETDEV_WHITEFIELD_H
#define NETDEV_WHITEFIELD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

/**
 * @brief whitefield interface state
 */
typedef struct netdev_whitefield {
    netdev_ieee802154_t dev802154;                    /**< netdev internal member */
	uint16_t nodeid;					/**< Node ID for whitefield simulation */
} netdev_whitefield_t;

/**
 * @brief whitefield interface initialization parameters
 */
typedef struct {
	int nodeid;	//Whitefield Node ID
} netdev_whitefield_params_t;

/**
 * @brief Setup netdev_whitefield_t structure.
 *
 * @param dev       the preallocated netdev_whitefield device handle to setup
 * @param params    initialization parameters
 */
void netdev_whitefield_setup(netdev_whitefield_t *dev, const netdev_whitefield_params_t *params);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* NETDEV_WHITEFIELD_H */
