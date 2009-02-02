LIB_GUI_DIR+=$(call target_suffix_platform_dirs,gui/qt4) $(foreach i,$(SUBLIBS),$(call target_suffix_platform_dirs,gui/qt4$(i)))

