ifeq ($(TARGET_PLATFORM_POSIX),1)
include $(MAGICMAKE_DIR)/magic/magic-daemons-posix.mak
endif
ifeq ($(TARGET_PLATFORM_LINUX),1)
include $(MAGICMAKE_DIR)/magic/magic-daemons-linux.mak
endif
ifeq ($(TARGET_PLATFORM_WIN32),1)
include $(MAGICMAKE_DIR)/magic/magic-daemons-win32.mak
endif
ifeq ($(TARGET_PLATFORM_MACOSX),1)
include $(MAGICMAKE_DIR)/magic/magic-daemons-macosx.mak
endif
