define macosx_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/macosx/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/macosx/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/macosx/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/macosx/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

MACOSX_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/macosx/$(1)/*.cpp))
MACOSX_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/macosx/$(1)/*.cc))
MACOSX_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/macosx/$(1)/*.c))
#MACOSX_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/macosx/$(1)/*.rc))

MACOSX_DAEMONS_O_$(1)= $$(strip $$(MACOSX_DAEMONS_CPP_$(1):.cpp=.o) $$(MACOSX_DAEMONS_CC_$(1):.cc=.o) $$(MACOSX_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT MACOSX DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

define macosx_sublib_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

MACOSX_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/*.cpp))
MACOSX_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/*.cc))
MACOSX_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/*.c))
#MACOSX_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/macosx/$(1)/*.rc))

MACOSX_DAEMONS_O_$(1)= $$(strip $$(MACOSX_DAEMONS_CPP_$(1):.cpp=.o) $$(MACOSX_DAEMONS_CC_$(1):.cc=.o) $$(MACOSX_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT MACOSX DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/macosx),$(eval $(call macosx_daemon,$(prog)))) 

$(foreach sublib,$(SUBLIBS),$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/$(sublib)/macosx),$(eval $(call macosx_sublib_daemon,$(prog),$(sublib)))))
