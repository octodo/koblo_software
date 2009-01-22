# win32_gui_dev_program is a bit of a hack but it shows the future plan of allowing multi object programs being built.
# a program is defined my a dirname in gui-dev/win32 - this dirname is the application name. Any sources in the dir are
# compiled and linked with the library

define win32_gui_dev_program
vpath %.cpp $(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/

LIB_GUI_DEV_EXE_FILES += $$(OUTPUT_GUI_DIR)/$(1)$$(EXE)

WIN32_GUI_DEV_DIR_$(1)=$$(PROJECT_TOP_DIR)/gui-dev/win32/$(1)

#LIB_GUI_DEV_DIR += $$(WIN32_GUI_DEV_DIR_$(1))

WIN32_GUI_DEV_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/*.cpp))
WIN32_GUI_DEV_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/*.cc))
WIN32_GUI_DEV_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/*.c))
WIN32_GUI_DEV_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/win32/$(1)/*.rc))

WIN32_GUI_DEV_O_$(1)= $$(strip $$(WIN32_GUI_DEV_CPP_$(1):.cpp=.o) $$(WIN32_GUI_DEV_CC_$(1):.cc=.o) $$(WIN32_GUI_DEV_C_$(1):.c=.o) $$(WIN32_GUI_DEV_RC_$(1):.rc=.o))

gui-dev :	$$(OUTPUT_GUI_DEV_DIR)/$(1)$$(EXE) 

$$(OUTPUT_GUI_DEV_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(WIN32_GUI_DEV_O_$(1)))
	@echo "LINKING MULTI-OBJECT GUI-DEV: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) $$(LDLIBS_GUI)


endef


define win32_gui_dev_sublib_program

vpath %.cpp $(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/

LIB_GUI_DEV_EXE_FILES += $$(OUTPUT_GUI_DEV_DIR)/$(1)$$(EXE)

WIN32_GUI_DEV_DIR_$(1)=$$(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)

#LIB_GUI_DEV_DIR += $$(WIN32_GUI_DEV_DIR_$(1))

WIN32_GUI_DEV_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/*.cpp))
WIN32_GUI_DEV_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/*.cc))
WIN32_GUI_DEV_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/*.c))
WIN32_GUI_DEV_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/gui-dev/$(2)/win32/$(1)/*.rc))

WIN32_GUI_DEV_O_$(1)= $$(strip $$(WIN32_GUI_DEV_CPP_$(1):.cpp=.o) $$(WIN32_GUI_DEV_CC_$(1):.cc=.o) $$(WIN32_GUI_DEV_C_$(1):.c=.o) $$(WIN32_GUI_DEV_RC_$(1):.rc=.o))

gui-dev :	$$(OUTPUT_GUI_DEV_DIR)/$(1)$$(EXE) 

$$(OUTPUT_GUI_DEV_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(WIN32_GUI_DEV_O_$(1)))
	@echo "LINKING MULTI-OBJECT GUI-DEV: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) $$(LDLIBS_GUI)


endef

ifeq ($(TARGET_PLATFORM_MINGW32),1)
$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/gui-dev/win32),$(eval $(call win32_gui_dev_program,$(prog)))) 

$(foreach sublib,$(SUBLIBS),$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/gui-dev/$(sublib)/win32),$(eval $(call win32_gui_dev_sublib_program,$(prog),$(sublib)))))
endif

