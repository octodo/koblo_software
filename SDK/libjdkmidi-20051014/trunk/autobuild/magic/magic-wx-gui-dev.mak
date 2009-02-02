LIB_GUI_DEV_DIR+=$(call target_suffix_platform_dirs,gui-dev/wx) $(foreach i,$(SUBLIBS),$(call target_suffix_platform_dirs,gui-dev/wx$(i)))


