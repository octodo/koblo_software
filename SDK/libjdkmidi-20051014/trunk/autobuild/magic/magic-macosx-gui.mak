

$(OUTPUT_GUI_DIR)/%.app : $(OUTPUT_OBJ_DIR)/%.o
	@echo "LINKING MAC OS X gui: $(notdir $<)"
	@$(call make_default_macosx_info_plist_file,$(OUTPUT_OBJ_DIR),$*-Info.plist,$*)
	@$(LINK.cpp) $(LINK_FLAGS) $(LDFLAGS) $(LINK_FLAGS_GUI) -o $(OUTPUT_OBJ_DIR)/$*$(EXE) $< -L$(OUTPUT_LIB_DIR) $(PROJECT_LDLIB) $(LDLIBS) $(LDLIBS_PACKAGES) $(LDLIBS_GUI)
	@$(call make_macosx_app_bundle,$(OUTPUT_GUI_DIR),$*,$(OUTPUT_OBJ_DIR)/$*$(EXE),$(OUTPUT_OBJ_DIR)/$*-Info.plist,$(PROJECT_TOP_DIR)/gui/macosx/$*/resources/)

define macosx_xcode_gui_program

LIB_GUI_EXE_FILES+=$$(OUTPUT_GUI_DIR)/$(1).app

$$(OUTPUT_GUI_DIR)/$(1).app : lib $$(PROJECT_TOP_DIR)/gui/macosx-xcode/$(1)/build/Release/$(1).app
	@echo COPYING MACOSX-XCODE gui app: $(1).app
	@$(RM) -r -f $$(OUTPUT_GUI_DIR)/$(1).app
	@$$(RSYNC) -aE $$(PROJECT_TOP_DIR)/gui/macosx-xcode/$(1)/build/Release/$(1).app $$(OUTPUT_GUI_DIR)/

$$(PROJECT_TOP_DIR)/gui/macosx-xcode/$(1)/build/Release/$(1).app :
	@echo BUILDING MACOSX-XCODE gui app: $(1).app
	@( cd $$(PROJECT_TOP_DIR)/gui/macosx-xcode/$(1) && $$(XCODEBUILD) $$(XCODEBUILD_OPTS) )

CLEAN_DIRS+=$$(PROJECT_TOP_DIR)/gui/macosx-xcode/$(1)/build $$(OUTPUT_GUI_DIR)/$(1).app

endef

$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/gui/macosx-xcode),$(eval $(call macosx_xcode_gui_program,$(prog))))

define macosx_xcode_gui_sublib_program

LIB_GUI_EXE_FILES+=$$(OUTPUT_GUI_DIR)/$(1).app

$$(OUTPUT_GUI_DIR)/$(1).app : lib $$(PROJECT_TOP_DIR)/gui/$(2)/macosx-xcode/$(1)/build/Release/$(1).app
	@echo COPYING MACOSX-XCODE gui app: $(1).app
	@$(RM) -r -f $$(OUTPUT_GUI_DIR)/$(1).app
	@$$(RSYNC) -aE $$(PROJECT_TOP_DIR)/gui/$(2)/macosx-xcode/$(1)/build/Release/$(1).app $$(OUTPUT_GUI_DIR)/

$$(PROJECT_TOP_DIR)/gui/$(2)/macosx-xcode/$(1)/build/Release/$(1).app :
	@echo BUILDING MACOSX-XCODE gui app: $(1).app
	@( cd $$(PROJECT_TOP_DIR)/gui/$(2)/macosx-xcode/$(1) && $$(XCODEBUILD) $$(XCODEBUILD_OPTS) )

CLEAN_DIRS+=$$(PROJECT_TOP_DIR)/gui/$(2)/macosx-xcode/$(1)/build $$(OUTPUT_GUI_DIR)/$(1).app

endef


$(foreach sublib,$(SUBLIBS),$(foreach prog,$(call bare_subdirs_in_path,$(PROJECT_TOP_DIR)/gui/$(sublib)/macosx-code),$(eval $(call macosx_xcode_gui_sublib_program,$(prog),$(sublib)))))

