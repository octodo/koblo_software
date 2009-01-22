define linux_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/linux/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/linux/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/linux/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/linux/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

LINUX_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/linux/$(1)/*.cpp))
LINUX_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/linux/$(1)/*.cc))
LINUX_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/linux/$(1)/*.c))
#LINUX_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/linux/$(1)/*.rc))

LINUX_DAEMONS_O_$(1)= $$(strip $$(LINUX_DAEMONS_CPP_$(1):.cpp=.o) $$(LINUX_DAEMONS_CC_$(1):.cc=.o) $$(LINUX_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT LINUX DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

define linux_sublib_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/linux/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/$(2)/linux/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/$(2)/linux/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/$(2)/linux/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

LINUX_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/linux/$(1)/*.cpp))
LINUX_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/linux/$(1)/*.cc))
LINUX_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/linux/$(1)/*.c))
#LINUX_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/linux/$(1)/*.rc))

LINUX_DAEMONS_O_$(1)= $$(strip $$(LINUX_DAEMONS_CPP_$(1):.cpp=.o) $$(LINUX_DAEMONS_CC_$(1):.cc=.o) $$(LINUX_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT LINUX DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/linux),$(eval $(call linux_daemon,$(prog)))) 

$(foreach sublib,$(SUBLIBS),$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/$(sublib)/linux),$(eval $(call linux_sublib_daemon,$(prog),$(sublib)))))
