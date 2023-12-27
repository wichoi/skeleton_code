include $(TOPDIR)/rules.mk

PKG_NAME:=nap-watchdog
PKG_VERSION:=0.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/nap-watchdog
  SECTION:=net
  CATEGORY:=Network
  TITLE:=NAP SW Watchdog Daemon
  DEPENDS:= +libpthread +librt +libatomic +libstdcpp
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/nap-watchdog/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nap-watchdog $(1)/usr/bin
endef

$(eval $(call BuildPackage,nap-watchdog))

