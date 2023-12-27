include $(TOPDIR)/rules.mk

PKG_NAME:=nap-client
PKG_VERSION:=0.0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

PKG_KCONFIG:=RALINK_MT7620 RALINK_MT7621 RALINK_MT7628 MACH_MT7623 GE1_SGMII_FORCE_2500 SUPPORT_OPENWRT MACH_LEOPARD SOC_MT7621 MT753X_GSW
PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=LICENSE
PKG_INSTALL:=1
#CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/nap-client
  SECTION:=net
  CATEGORY:=Network
  TITLE:=NAP Client application
  DEPENDS:= +libnl-tiny +libpthread +librt +libatomic +libstdcpp +libcurl +libnvram
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/app/* $(PKG_BUILD_DIR)/
endef

define Build/Configure/Deprecated
	echo "set(CMAKE_SYSTEM_NAME Linux)" > $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_SYSTEM_PROCESSOR mips)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_C_FLAGS \"$(CMAKE_C_FLAGS) -s -Wno-error\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_C_FLAGS_DEBUG \"${CMAKE_C_FLAGS_DEBUG} -s -Wno-error\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_C_FLAGS_RELEASE \"${CMAKE_C_FLAGS_RELEASE} -s -Wno-error\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -s -Wno-error\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_CXX_FLAGS_DEBUG \"${CMAKE_CXX_FLAGS_DEBUG} -s -Wno-error\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_CXX_FLAGS_RELEASE \"${CMAKE_CXX_FLAGS_RELEASE} -s -Wno-error\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_CXX_STANDARD 11)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_CXX_STANDARD_REQUIRED ON)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "add_definitions(-D_GLIBCXX_USE_C99=1)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_SYSROOT $(TOOLCHAIN_DIR))" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_C_COMPILER \"$(CMAKE_C_COMPILER)\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_CXX_COMPILER \"$(CMAKE_CXX_COMPILER)\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_LINKER $(TARGET_CROSS)ld)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_NM $(TARGET_CROSS)nm)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_STRIP $(TARGET_CROSS)strip)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_OBJCOPY $(TARGET_CROSS)objcopy)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_OBJDUMP $(TARGET_CROSS)-objdump)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_RANLIB $(TARGET_CROSS)ranlib)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CURL_LIBRARY \" $(STAGING_DIR)/usr/lib\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(CURL_INCLUDE_DIR \" $(STAGING_DIR)/usr/include\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(LINUX_DRIVER_INCLUDE_DIR \" $(LINUX_DIR)/drivers/net/ethernet/raeth\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(LINUX_USER_INCLUDE_DIR \" $(LINUX_DIR)/user_headers/include\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(LINUX_NL_TINY_INCLUDE_DIR \" $(STAGING_DIR)/usr/include/libnl-tiny\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
endef

define Build/Configure
	$(call Build/Configure/Deprecated)
endef

define Build/Compile
	cd $(PKG_BUILD_DIR) && \
	mkdir -p cmake/build && \
	cd cmake/build && \
	cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain.cmake \
	-DCMAKE_BUILD_TYPE=Release \
	-DProtobuf_DIR=$(PKG_BUILD_DIR)/../grpc-1.34/target_install/lib/cmake/protobuf \
	-DgRPC_DIR=$(PKG_BUILD_DIR)/../grpc-1.34/target_install/lib/cmake/grpc \
	../.. && \
	$(MAKE)
endef

define Build/Install
	#$(INSTALL_DIR) $(1)/usr/bin
	#$(CP) $(PKG_BUILD_DIR)/cmake/build/nap-client $(1)/usr/bin
	#$(CP) $(PKG_BUILD_DIR)/cmake/build/nap-server $(1)/usr/bin
endef

define Package/nap-client/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/cmake/build/nap-client $(1)/usr/bin
	#$(CP) $(PKG_BUILD_DIR)/cmake/build/nap-server $(1)/usr/bin
endef

$(eval $(call BuildPackage,nap-client))
