##############################################################################################
#
# The magic.makefile Copyright 2004-2008 by Jeff Koftinoff <jeffk@jdkoftinoff.com> and J.D. Koftinoff Software Ltd.
# version 6.
#
# Simplifies the building of a c/c++ library, it's tests, tools, examples, and documentation.
#
# See http://opensource.jdkoftinoff.com/jdks/trac/wiki/MagicMakefileV6
# for more information, including license information (GPL). Note that this tool being GPL does
# NOT mean that it can only be used to build GPL projects.
#

MAGICMAKE_DIR=$(PROJECT_TOP_DIR)/autobuild

include $(MAGICMAKE_DIR)/magic/magic-project.mak
include $(MAGICMAKE_DIR)/magic/magic-tools.mak

ifeq ($(QMAKE_MODE),1)
include $(MAGICMAKE_DIR)/magic/magic-qmake-mode.mak
endif

include $(MAGICMAKE_DIR)/magic/magic-utils.mak
include $(MAGICMAKE_DIR)/magic/magic-options.mak
include $(MAGICMAKE_DIR)/magic/magic-platform.mak
include $(MAGICMAKE_DIR)/magic/magic-dirs.mak

include $(MAGICMAKE_DIR)/magic/magic-vpaths.mak
include $(MAGICMAKE_DIR)/magic/magic-rules.mak
include $(MAGICMAKE_DIR)/magic/magic-firsttarget.mak
include $(MAGICMAKE_DIR)/magic/magic-configtool.mak
include $(MAGICMAKE_DIR)/magic/magic-native.mak
include $(MAGICMAKE_DIR)/magic/magic-filelist.mak

include $(MAGICMAKE_DIR)/magic/magic-latex.mak

ifeq ($(WX_MODE),1)
include $(MAGICMAKE_DIR)/magic/magic-wx-mode.mak
endif

ifeq ($(GTK_MODE),1)
include $(MAGICMAKE_DIR)/magic/magic-gtk.mak
endif

ifeq ($(QT4_MODE),1)
include $(MAGICMAKE_DIR)/magic/magic-qt4.mak
endif

ifeq ($(DO_NOT_BUILD_LIB)$(ENABLE_SHLIB),01)
include $(MAGICMAKE_DIR)/magic/magic-shlib.mak
else

.PHONY : shlib

shlib :

endif

include $(MAGICMAKE_DIR)/magic/magic-daemons.mak
include $(MAGICMAKE_DIR)/magic/magic-targets.mak

ifeq ($(TARGET_PLATFORM_MACOSX),1)
include $(MAGICMAKE_DIR)/magic/magic-macosx.mak
endif

ifeq ($(TARGET_PLATFORM_LINUX),1)
include $(MAGICMAKE_DIR)/magic/magic-linux.mak
endif

ifeq ($(TARGET_PLATFORM_POSIX),1)
include $(MAGICMAKE_DIR)/magic/magic-posix.mak
endif

ifeq ($(TARGET_PLATFORM_WIN32),1)
include $(MAGICMAKE_DIR)/magic/magic-win32.mak
endif

include $(MAGICMAKE_DIR)/magic/magic-custom.mak
include $(MAGICMAKE_DIR)/magic/magic-install.mak
include $(MAGICMAKE_DIR)/magic/magic-package.mak
include $(MAGICMAKE_DIR)/magic/magic-ship.mak
include $(MAGICMAKE_DIR)/magic/magic-help.mak

ifeq ($(MAKECMDGOALS),clean)
GOALISCLEAN=1
endif
ifeq ($(MAKECMDGOALS),realclean)
GOALISCLEAN=1
endif
ifeq ($(MAKECMDGOALS),distclean)
GOALISCLEAN=1
endif

ifneq ($(GOALISCLEAN),1)
include $(MAGICMAKE_DIR)/magic/magic-deps.mak
endif
