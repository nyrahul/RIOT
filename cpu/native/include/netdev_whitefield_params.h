/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     netdev
 * @brief
 * @{
 *
 * @file
 * @brief       Default configuration for the netdev_whitefield driver
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NETDEV_WHITEFIELD_PARAMS_H
#define NETDEV_WHITEFIELD_PARAMS_H

#include "netdev_whitefield.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of allocated parameters at @ref netdev_whitefield_params
 *
 * @note    This was decided to only be configurable on compile-time to be
 *          more similar to actual boards
 */
#ifndef NETDEV_WHITEFIELD_MAX
#define NETDEV_WHITEFIELD_MAX              (1)
#endif

/**
 * @brief   Configuration parameters for @ref netdev_whitefield_t
 *
 * @note    This variable is set on native start-up based on arguments provided
 */
extern netdev_whitefield_params_t netdev_whitefield_params[NETDEV_WHITEFIELD_MAX];

#ifdef __cplusplus
}
#endif

#endif /* NETDEV_WHITEFIELD_PARAMS_H */
/** @} */
