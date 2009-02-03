LIB_GUI_DEV_DIR+=$(call target_suffix_platform_dirs,gui-dev/qt4) $(foreach i,$(SUBLIBS),$(call target_suffix_platform_dirs,gui-dev/qt4$(i)))


