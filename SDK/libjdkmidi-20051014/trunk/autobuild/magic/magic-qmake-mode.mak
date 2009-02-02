
ifeq ($(TARGET_PLATFORM_MACOSX),1)
QMAKESPEC?=macx-g++
endif

ifeq ($(RELEASE),1)
QMAKE_MAKE_OPTIONS+=-config release
endif

ifeq ($(DEBUG),1)
QMAKE_MAKE_OPTIONS+=-config debug
endif

ifeq ($(TARGET_PLATFORM_MACOSX_UNIVERSAL),1)
QMAKESPEC?=macx-g++
QMAKE_CONFIG+=x86 ppc
endif

ifeq ($(TARGET_PLATFORM_LINUX),1)
QMAKESPEC?=linux-g++
endif

ifeq ($(TARGET_PLATFORM_MINGW32)-$(CROSS_COMPILING),1-1)
QMAKESPEC?=$(PROJECT_TOP_DIR)/autobuild/mkspecs/$(COMPILER_PREFIX)-g++
endif

ifeq ($(TARGET_PLATFORM_MINGW32),1)
QMAKESPEC?=win32-g++
endif

QMAKESPEC?=default

ALL = all-via-qmake

QMAKE_BUILD_DIR?=$(OUTPUT_DIR)/qt4

QMAKE_LIB_CONFIG+=static

LIB_QT4_H_FILES=$(call get_file_list_full,$(LIB_INCLUDE_DIR) $(call subdirs_in_path,$(LIB_INCLUDE_DIR)),h)
LIB_QT4_TOOLS_CPP_FILES=$(call get_file_list_full,$(LIB_TOOLS_DIR),cpp)
LIB_QT4_TOOLS_DEV_CPP_FILES=$(call get_file_list_full,$(LIB_TOOLS_DEV_DIR),cpp)
LIB_QT4_EXAMPLES_CPP_FILES=$(call get_file_list_full,$(LIB_EXAMPLES_DIR),cpp)
LIB_QT4_TESTS_CPP_FILES=$(call get_file_list_full,$(LIB_TESTS_DIR),cpp)
LIB_QT4_APPS_CPP_FILES=$(call get_file_list_full,$(addsuffix /qt4,$(LIB_GUI_DIR)),cpp)
LIB_QT4_APPS_DEV_CPP_FILES=$(call get_file_list_full,$(addsuffix /qt4,$(LIB_GUI_DEV_DIR)),cpp)
LIB_QT4_APPS=$(notdir $(LIB_QT4_APPS_CPP_FILES:.cpp=))
LIB_QT4_APPS_DEV=$(notdir $(LIB_QT4_APPS_DEV_CPP_FILES:.cpp=))
QMAKE_MAC_SDK?=$(TARGET_MACOSX_SDK)


define make_qt4_app_pro
	@echo QMAKE-PRO APP: $(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(1)
	@$(QMAKE) -project -o $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_OPTIONS) $(2)
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_APP_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo CONFIG -= $(QMAKE_MINUS_APP_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QT += $(QMAKE_APP_QT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@if [ -f $(PROJECT_TOP_DIR)/gui/$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.prefix ]; \
		then \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(PROJECT_TOP_DIR)/gui/$(QMAKE_BUILD_DIR)/$(1).pro.prefix $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
	fi
	@echo INCLUDEPATH += $(INCLUDES) ../$(PROJECT)>>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo LIBS += -L../$(PROJECT) -l$(PROJECT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts

endef

define make_qt4_app_dev_pro
	@echo QMAKE-PRO APP-DEV: $(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(1)
	@$(QMAKE) -project -o $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_OPTIONS) $(2)
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_APP_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo CONFIG -= $(QMAKE_MINUS_APP_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QT += $(QMAKE_APP_QT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@if [ -f $(PROJECT_TOP_DIR)/gui-dev/$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.prefix ]; \
		then \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(PROJECT_TOP_DIR)/gui-dev/$(QMAKE_BUILD_DIR)/$(1).pro.prefix $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
	fi
	@echo INCLUDEPATH += $(INCLUDES) ../$(PROJECT)>>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo LIBS += -L../$(PROJECT) -l$(PROJECT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts

endef


define make_qt4_tool_pro
	@echo QMAKE-PRO TOOL: $(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(1)
	@$(QMAKE) -project -o $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_OPTIONS) $(2)
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_TOOL_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo CONFIG -= $(QMAKE_MINUS_TOOL_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QT += $(QMAKE_TOOL_QT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@if [ -f $(PROJECT_TOP_DIR)/tools/$(1)/$(1).pro.prefix ]; \
		then \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(PROJECT_TOP_DIR)/tools/$(QMAKE_BUILD_DIR)/$(1).pro.prefix $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
	fi
	@echo INCLUDEPATH += $(INCLUDES) ../$(PROJECT)>>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo LIBS += -L../$(PROJECT) -l$(PROJECT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts

endef

define make_qt4_tool_dev_pro
	@echo QMAKE-PRO TOOL-DEV: $(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(1)
	@$(QMAKE) -project -o $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_OPTIONS) $(2)
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_TOOL_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo CONFIG -= $(QMAKE_MINUS_TOOL_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QT += $(QMAKE_TOOL_QT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@if [ -f $(PROJECT_TOP_DIR)/tools-dev/$(1)/$(1).pro.prefix ]; \
		then \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(PROJECT_TOP_DIR)/tools-dev/$(QMAKE_BUILD_DIR)/$(1).pro.prefix $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
	fi
	@echo INCLUDEPATH += $(INCLUDES) ../$(PROJECT)>>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo LIBS += -L../$(PROJECT) -l$(PROJECT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts

endef


define make_qt4_example_pro
	@echo QMAKE-PRO EXAMPLE: $(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(1)
	@$(QMAKE) -project -o $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_OPTIONS) $(2)
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_EXAMPLE_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo CONFIG -= $(QMAKE_MINUS_EXAMPLE_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QT += $(QMAKE_EXAMPLE_QT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@if [ -f $(PROJECT_TOP_DIR)/examples/$(1)/$(1).pro.prefix ]; \
		then \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(PROJECT_TOP_DIR)/examples/$(QMAKE_BUILD_DIR)/$(1).pro.prefix $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
	fi
	@echo INCLUDEPATH += $(INCLUDES) ../$(PROJECT)>>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo LIBS += -L../$(PROJECT) -l$(PROJECT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts

endef

define make_qt4_test_pro
	@echo QMAKE-PRO EXAMPLE: $(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(1)
	@$(QMAKE) -project -o $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_OPTIONS) $(2)
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_TEST_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo CONFIG -= $(QMAKE_MINUS_TEST_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QT += $(QMAKE_TEST_QT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@if [ -f $(PROJECT_TOP_DIR)/tests/$(1)/$(1).pro.prefix ]; \
		then \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(PROJECT_TOP_DIR)/tests/$(QMAKE_BUILD_DIR)/$(1).pro.prefix $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
	fi
	@echo INCLUDEPATH += $(INCLUDES) ../$(PROJECT)>>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo LIBS += -L../$(PROJECT) -l$(PROJECT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts

endef


define make_qt4_daemons_pro
	@echo QMAKE-PRO DAEMON: $(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(1)
	@$(QMAKE) -project -o $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_OPTIONS) $(2)
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_DAEMON_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo CONFIG -= $(QMAKE_MINUS_DAEMON_CONFIG) >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo QT += $(QMAKE_TEST_QT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts
	@if [ -f $(PROJECT_TOP_DIR)/daemons/$(1)/$(1).pro.prefix ]; \
		then \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(PROJECT_TOP_DIR)/daemons/$(QMAKE_BUILD_DIR)/$(1).pro.prefix $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp >$(QMAKE_BUILD_DIR)/$(1)/$(1).pro; \
	fi
	@echo INCLUDEPATH += $(INCLUDES) ../$(PROJECT)>>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@echo LIBS += -L../$(PROJECT) -l$(PROJECT) >>$(QMAKE_BUILD_DIR)/$(1)/$(1).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.tmp $(QMAKE_BUILD_DIR)/$(1)/$(1).pro.opts

endef


.PHONY : qmake-pro qmake-lib-pro qmake-app-pro qmake-app-dev-pro qmake-tools-pro qmake-tools-dev-pro qmake-examples-pro qmake-tests-pro qmake-daemons-pro

qmake-pro :  $(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro qmake-apps-pro qmake-apps-dev-pro qmake-tools-pro qmake-tools-dev-pro qmake-examples-pro qmake-tests-pro qmake-daemons-pro

QT4_APPS = $(notdir $(LIB_QT4_APPS_CPP_FILES:.cpp=))
QT4_APPS_DEV = $(notdir $(LIB_QT4_APPS_DEV_CPP_FILES:.cpp=))
QT4_TOOLS = $(notdir $(LIB_TOOLS_CPP_FILES:.cpp=))
QT4_TOOLS_DEV = $(notdir $(LIB_TOOLS_DEV_CPP_FILES:.cpp=))
QT4_EXAMPLES = $(notdir $(LIB_EXAMPLES_CPP_FILES:.cpp=))
QT4_TESTS = $(notdir $(LIB_TESTS_CPP_FILES:.cpp=))
QT4_DAEMONS = $(notdir $(LIB_DAEMONS_CPP_FILES:.cpp=))

qmake-apps-pro : 
	$(foreach src,$(LIB_QT4_APPS_CPP_FILES),$(call make_qt4_app_pro,$(notdir $(src:.cpp=)),$(src)))

qmake-apps-dev-pro : 
	$(foreach src,$(LIB_QT4_APPS_DEV_CPP_FILES),$(call make_qt4_app_dev_pro,$(notdir $(src:.cpp=)),$(src)))

qmake-examples-pro : 
	$(foreach src,$(LIB_QT4_EXAMPLES_CPP_FILES),$(call make_qt4_example_pro,$(notdir $(src:.cpp=)),$(src)))

qmake-tools-pro : 
	$(foreach src,$(LIB_QT4_TOOLS_CPP_FILES),$(call make_qt4_tool_pro,$(notdir $(src:.cpp=)),$(src)))

qmake-tools-dev-pro : 
	$(foreach src,$(LIB_QT4_TOOLS_DEV_CPP_FILES),$(call make_qt4_tool_dev_pro,$(notdir $(src:.cpp=)),$(src)))

qmake-tests-pro : 
	$(foreach src,$(LIB_QT4_TESTS_CPP_FILES),$(call make_qt4_test_pro,$(notdir $(src:.cpp=)),$(src)))

qmake-daemons-pro : 
	$(foreach src,$(LIB_QT4_DAEMONS_CPP_FILES),$(call make_qt4_daemons_pro,$(notdir $(src:.cpp=)),$(src)))

$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro :
	@echo MAKING QMAKE LIB PROJECT: $(QMAKE_BUILD_DIR)/$(PROJECT).pro
	@mkdir -p $(QMAKE_BUILD_DIR)/$(PROJECT)
	@$(QMAKE) -project -norecursive -o $(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro.tmp -t lib $(QMAKE_OPTIONS) $(LIB_SRC_DIR)
	@if [ -f $(PROJECT_TOP_DIR)/$(PROJECT).pro.prefix ]; \
		then \
			cat $(PROJECT_TOP_DIR)/$(PROJECT).pro.prefix $(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro.tmp >$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro; \
		else \
			cat $(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro.tmp >$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro; \
	fi
	@echo QMAKE_MAC_SDK = $(QMAKE_MAC_SDK) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@echo INCLUDEPATH += $(INCLUDES) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@echo DEFINES += $(DEFINES) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@echo QMAKE_MACOSX_DEPLOYMENT_TARGET=$(MACOSX_DEPLOYMENT_TARGET) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@echo CONFIG += $(QMAKE_CONFIG) $(QMAKE_LIB_CONFIG) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@echo CONFIG -= $(QMAKE_MINUS_LIB_CONFIG) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@echo PROJECT_TOP_DIR = $(PROJECT_TOP_DIR) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@echo HEADERS += $(LIB_QT4_H_FILES) >>$(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro
	@rm -f $(QMAKE_BUILD_DIR)/$(PROJECT)/$(PROJECT).pro.tmp 



.PHONY : all-via-qmake

all-via-qmake : qmake-pro qmake-apps-pro qmake-apps-dev-pro qmake-tools-pro qmake-tools-dev-pro qmake-examples-pro qmake-tests-pro qmake-daemons-pro
	@(cd $(QMAKE_BUILD_DIR)/$(PROJECT) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS) && $(MAKE)  ) && echo DONE LIB
	@$(foreach tool,$(QT4_TOOLS),(cd $(QMAKE_BUILD_DIR)/$(tool) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS) && $(MAKE) ) && ) echo DONE TOOLS
	@$(foreach tool,$(QT4_TOOLS_DEV),(cd $(QMAKE_BUILD_DIR)/$(tool) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS) && $(MAKE) ) && ) echo DONE TOOLS-DEV
	@$(foreach app,$(QT4_APPS),(cd $(QMAKE_BUILD_DIR)/$(app) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS) && $(MAKE) ) && ) echo DONE APPS
	@$(foreach app-dev,$(QT4_APPS_DEV),(cd $(QMAKE_BUILD_DIR)/$(app) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS) && $(MAKE) ) && ) echo DONE APPS-DEV
	@$(foreach example,$(QT4_EXAMPLES),(cd $(QMAKE_BUILD_DIR)/$(example) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS && $(MAKE) )) && ) echo DONE EXAMPLES
	@$(foreach test,$(QT4_TESTS),(cd $(QMAKE_BUILD_DIR)/$(test) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS) && $(MAKE) ) && ) echo DONE TESTS
	@$(foreach daemon,$(QT4_DAEMONS),(cd $(QMAKE_BUILD_DIR)/$(daemon) && $(QMAKE) -makefile -spec $(QMAKESPEC) $(QMAKE_MAKE_OPTIONS) && $(MAKE) ) && ) echo DONE DAEMONS


