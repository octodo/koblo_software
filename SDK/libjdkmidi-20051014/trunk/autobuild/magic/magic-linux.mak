.PHONY : checkinstall

checkinstall :
	checkinstall -y --type debian --fstrans --pkgname $(PROJECT)$(PROJECT_VARIANT_LABEL) --pkgversion $(PROJECT_VERSION) --pkglicense $(PROJECT_LICENSE) --maintainer $(PROJECT_MAINTAINER) --strip --stripso --addso make install

checkinstall-dev :
	checkinstall -y --type debian --fstrans --pkgname $(PROJECT)$(PROJECT_VARIANT_LABEL)_dev --pkgversion $(PROJECT_VERSION) --pkglicense $(PROJECT_LICENSE) --maintainer $(PROJECT_MAINTAINER) --strip --stripso --addso make install-dev


