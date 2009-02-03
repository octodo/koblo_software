
ifeq ($(TARGET_PLATFORM_LINUX),1)
include $(MAGICMAKE_DIR)/magic/magic-shlib-linux.mak
endif

ifeq ($(TARGET_PLATFORM_WIN32),1)
include $(MAGICMAKE_DIR)/magic/magic-shlib-win32.mak
endif

ifeq ($(TARGET_PLATFORM_MACOSX),1)
include $(MAGICMAKE_DIR)/magic/magic-shlib-macosx.mak
endif
