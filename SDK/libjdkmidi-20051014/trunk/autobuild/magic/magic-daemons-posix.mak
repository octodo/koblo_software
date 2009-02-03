define posix_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/posix/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/posix/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/posix/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/posix/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

POSIX_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/posix/$(1)/*.cpp))
POSIX_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/posix/$(1)/*.cc))
POSIX_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/posix/$(1)/*.c))
#POSIX_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/posix/$(1)/*.rc))

POSIX_DAEMONS_O_$(1)= $$(strip $$(POSIX_DAEMONS_CPP_$(1):.cpp=.o) $$(POSIX_DAEMONS_CC_$(1):.cc=.o) $$(POSIX_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT POSIX DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

define posix_sublib_daemon

vpath %.cpp $(PROJECT_TOP_DIR)/daemons/posix/$(1)/
vpath %.cc $(PROJECT_TOP_DIR)/daemons/$(2)/posix/$(1)/
vpath %.c $(PROJECT_TOP_DIR)/daemons/$(2)/posix/$(1)/
vpath %.rc $(PROJECT_TOP_DIR)/daemons/$(2)/posix/$(1)/

LIB_DAEMONS_EXE_FILES += $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

POSIX_DAEMONS_CPP_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/posix/$(1)/*.cpp))
POSIX_DAEMONS_CC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/posix/$(1)/*.cc))
POSIX_DAEMONS_C_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/posix/$(1)/*.c))
#POSIX_DAEMONS_RC_$(1)= $$(notdir $$(wildcard $$(PROJECT_TOP_DIR)/daemons/$(2)/posix/$(1)/*.rc))

POSIX_DAEMONS_O_$(1)= $$(strip $$(POSIX_DAEMONS_CPP_$(1):.cpp=.o) $$(POSIX_DAEMONS_CC_$(1):.cc=.o) $$(POSIX_DAEMONS_C_$(1):.c=.o)

daemons : $$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE)

$$(OUTPUT_DAEMONS_DIR)/$(1)$$(EXE) : $$(addprefix $$(OUTPUT_OBJ_DIR)/, $$(OUTPUT_DAEMONS_O_$(1)))
	@echo "LINKING MULTI-OBJECT POSIX DAEMON: $$(notdir $$@) from $$(notdir $$^)"
	@$$(LINK.cpp) $$(LINK_FLAGS) $$(LDFLAGS) $$(LINK_FLAGS_GUI) -o $$@ $$^ -L$$(OUTPUT_LIB_DIR) $$(PROJECT_LDLIB) $$(LDLIBS) 

endef

$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/posix),$(eval $(call posix_daemon,$(prog)))) 

$(foreach sublib,$(SUBLIBS),$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/daemons/$(sublib)/posix),$(eval $(call posix_sublib_daemon,$(prog),$(sublib)))))
