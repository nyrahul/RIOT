PKG_NAME = whitefield
PKG_VERSION = 123456
#PKG_FILE = nrf5_iot_sdk_$(PKG_VERSION).zip
#PKG_URL = https://developer.nordicsemi.com/nRF5_IoT_SDK/nRF5_IoT_SDK_v0.9.x/$(PKG_FILE)
#PKG_MD5 = 291c875df69c51e22491e6112128bd9c
PKG_LICENSE = gpl2
PKG_DIR=$(CURDIR)
PKG_BUILDDIR=$(PKGDIRBASE)/$(PKG_NAME)
PKG_SRCDIR=$(PKG_BUILDDIR)/src

MODULE_MAKEFILE := $(PKG_DIR)/Makefile.module

.PHONY: all clean distclean

all:

clean::
	rm -rf $(PKG_BUILDDIR)/

distclean::
	rm -rf $(PKG_BUILDDIR)/
