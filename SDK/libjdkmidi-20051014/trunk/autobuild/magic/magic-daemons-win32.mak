define win32_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/win32/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/win32/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/win32/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/win32/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

WIN32_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/win32/$(1)/*.cpp))
WIN32_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/win32/$(1)/*.cc))
WIN32_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/win32/$(1)/*.c))
#WIN32_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/win32/$(1)/*.rc))

WIN32_DAEMONS_O_$(1)= $$(strip $$(WIN32_DAEMONS_CPP_$(1):.cpp=.o) $$(WIN32_DAEMONS_CC_$(1):.cc=.o) $$(WIN32_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT WIN32 DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

define win32_sublib_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/win32/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/$(2)/win32/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/$(2)/win32/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/$(2)/win32/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

WIN32_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/win32/$(1)/*.cpp))
WIN32_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/win32/$(1)/*.cc))
WIN32_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/win32/$(1)/*.c))
#WIN32_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/win32/$(1)/*.rc))

WIN32_DAEMONS_O_$(1)= $$(strip $$(WIN32_DAEMONS_CPP_$(1):.cpp=.o) $$(WIN32_DAEMONS_CC_$(1):.cc=.o) $$(WIN32_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT WIN32 DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/win32),$(eval $(call win32_daemon,$(prog)))) 

$(foreach sublib,$(SUBLIBS),$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/$(sublib)/win32),$(eval $(call win32_sublib_daemon,$(prog),$(sublib)))))
