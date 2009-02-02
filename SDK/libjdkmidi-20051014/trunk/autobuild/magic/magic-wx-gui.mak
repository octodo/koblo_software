LIB_GUI_DIR+=$(call target_suffix_platform_dirs,gui/wx) $(foreach i,$(SUBLIBS),$(call target_suffix_platform_dirs,gui/wx$(i)))

