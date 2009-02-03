##############################################################################################
#
# The multi magic.makefile Copyright 2004-2007 by Jeff Koftinoff <jeffk@jdkoftinoff.com>
# version 6.
#
# Simplifies the building of a c/c++ library, it's tests, tools, examples, and documentation.
#
# See http://opensource.jdkoftinoff.com/jdks/trac/wiki/MagicMakefileV5
# for more information, including license information (GPL). Note that this tool being GPL does
# NOT mean that it can only be used to build GPL projects.
#

MAGIC_UTIL_DIR=$(AUTOBUILD_DIR)/multi-magic

TMP_TARGET=tmp-target

DEBUG?=1
RELEASE?=0

ifdef ($(PROJECT_VARIANT))
CONFIGURE_FLAGS+=--project-variant=$(PROJECT_VARIANT)
endif
ifeq ($(DEBUG),1)
CONFIGURE_FLAGS+=--debug=1
endif
ifeq ($(RELEASE),1)
CONFIGURE_FLAGS+=--release=1
endif

comma:= ,
empty:=
space:= $(empty) $(empty)
NEWPATH:=$(subst $(space),:,$(foreach p,$(PROJECTS),$(PWD)/$(TMP_TARGET)/$(p)))

define make_in
	@for i in $(2); do ( echo "PROJECT $$i: make $(1)"; export PATH="$(NEWPATH)$$PATH"; cd "$(TMP_TARGET)/$$i" && make $(1) ) || exit 1; done
endef

define make_in_all
	@for i in $(PROJECTS); do ( echo "PROJECT $$i: make $(1)"; export PATH="$(NEWPATH):$$PATH"; cd "$(TMP_TARGET)/$$i" && make $(1) ) || exit 1; done
endef

.PHONY : everything all dirs configure build ship install \
	install-dev preinstall preinstall-dev package clean \
	docs-dev check test package-docs-dev package-dev docs-dev

everything : all

all : dirs build

dirs :
	@for i in $(PROJECTS); do mkdir -p "$(TMP_TARGET)/$$i"; done

clean : 
	@rm -r -f $(TMP_TARGET)

configure : dirs
	@for i in $(PROJECTS); do \
    ( \
        cd "$(TMP_TARGET)/$$i" && \
        "$(PROJECT_TOP_DIR)/$$i/configure" $(CONFIGURE_FLAGS) \
    ) || exit 1; \
	done 


build : configure
	$(call make_in_all,)

ship : preinstall
	$(call make_in_all,ship)

install : preinstall
	$(call make_in_all,install)

preinstall : build
	$(call make_in_all,preinstall)

install-dev : install
	$(call make_in_all,install-dev)

preinstall-dev : install
	$(call make_in_all,preinstall-dev)

package : preinstall
	$(call make_in_all,package)

package-dev : preinstall
	$(call make_in_all,package-dev)

package-docs-dev : configure
	$(call make_in_all,package-docs-dev)

docs : configure
	$(call make_in_all,docs)

docs-dev : configure
	$(call make_in_all,docs-dev)

check : build
	$(call make_in_all,check)

test : build
	$(call make_in_all,test)

ifeq ($(TARGET_PLATFORM_LINUX),1)

checkinstall : preinstall
	$(call make_in,checkinstall,$(FINAL_PROJECT))

checkinstall-dev : preinstall-dev
	$(call make_in,checkinstall-dev,$(FINAL_PROJECT))

endif




