package : preinstall

package-dev : preinstall-dev

package-docs-dev : preinstall-docs-dev

package : $(PACKAGES_DIR)/$(PACKAGE_BASENAME).tgz

$(PACKAGES_DIR)/$(PACKAGE_BASENAME).tgz :
	@echo "PACKAGE-TGZ"
	@( cd $(LOCAL_INSTALL_DIR) && tar cvf - . | gzip >$(PACKAGES_DIR)/$(PACKAGE_BASENAME).tgz )

package-dev : $(PACKAGES_DIR)/$(PACKAGEDEV_BASENAME).tgz

$(PACKAGES_DIR)/$(PACKAGEDEV_BASENAME).tgz :
	@echo "PACKAGE-DEV-TGZ"
	@( cd $(LOCAL_INSTALLDEV_DIR) && tar cvf - . | gzip >$(PACKAGES_DIR)/$(PACKAGEDEV_BASENAME).tgz )

package-docs-dev : $(PACKAGES_DIR)/$(PACKAGEDOCSDEV_BASENAME).tgz

$(PACKAGES_DIR)/$(PACKAGEDOCSDEV_BASENAME).tgz :
	@echo "PACKAGE-DOCS-DEV-TGZ"
	@( cd $(LOCAL_INSTALLDOCSDEV_DIR) && tar cvf - . | gzip >$(PACKAGES_DIR)/$(PACKAGEDOCSDEV_BASENAME).tgz )

