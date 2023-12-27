include $(TOPDIR)/rules.mk

PKG_NAME:=fw-manager
PKG_VERSION:=0.0.1
PKG_RELEASE:=1
PKG_BUILD_DEPENDS:=fw-manager/host

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/fw-manager
  SECTION:=net
  CATEGORY:=Network
  TITLE:=firmware manager
  DEPENDS:= +libopenssl
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
	echo "set(OPENSSL_CRYPTO_LIBRARY \" $(STAGING_DIR)/usr/lib\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
	echo "set(OPENSSL_INCLUDE_DIR \" $(STAGING_DIR)/usr/include\")" >> $(PKG_BUILD_DIR)/toolchain.cmake
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
	../.. && \
	$(MAKE)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/fw-manager $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/fw-manager
	$(CP) $(PKG_BUILD_DIR)/cmake/build/libfwmgr.a $(1)/usr/lib
endef

define Build/Install
endef

define Package/fw-manager/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/cmake/build/fw-manager $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/cmake/build/libfwmgr.a $(1)/usr/lib
endef

define Host/Prepare
	mkdir -p $(HOST_BUILD_DIR)
	$(CP) ./src/app/* $(HOST_BUILD_DIR)/
endef

define Host/Configure
	cd $(HOST_BUILD_DIR) && \
	mkdir -p cmake/build && \
	cd cmake/build && \
	CFLAGS="$(HOST_CFLAGS)" \
	CXXFLAGS="$(HOST_CFLAGS)" \
	cmake $(CMAKE_HOST_OPTIONS) ../..
endef

define Host/Compile
	cd $(HOST_BUILD_DIR)/cmake/build && \
	$(MAKE) $(HOST_JOBS) $(HOST_MAKE_FLAGS)
endef

define Host/Install
	cp $(HOST_BUILD_DIR)/cmake/build/fw-manager $(STAGING_DIR_HOST)/bin
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,fw-manager))
