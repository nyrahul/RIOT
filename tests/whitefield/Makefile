APPLICATION = riot-whitefield

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP -Wall

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEPKG += whitefield

USEMODULE += whitefield
USEMODULE += xtimer
USEMODULE += random
#USEMODULE += shell
#USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += ipv6_addr
USEMODULE += gnrc_udp
USEMODULE += gnrc_ipv6_router_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_sixlowpan
USEMODULE += gnrc_sixlowpan_iphc
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_netdev
USEMODULE += gnrc_ipv6_netif
USEMODULE += netstats_l2

#required for C++ compiling
#CXXEXFLAGS += -fno-rtti
#USEMODULE += cpp11-compat

#Define PANID and CHANNEL used by default
#CFLAGS += -DOPENTHREAD_PANID=0xbeef -DOPENTHREAD_CHANNEL=11

include $(RIOTBASE)/Makefile.include
