# default project settings. These are usually originally set in project.mak

# PROJECT is the single word describing this project.
PROJECT?=project

# PROJECT_NAME is a single line describing this project
PROJECT_NAME?=Project

# PROJECT_VERSION defaults to the date, YYYYMMDD
PROJECT_VERSION_SHELL:=$(shell date +%Y%m%d)
PROJECT_VERSION?=$(PROJECT_VERSION_SHELL)

# this project may have different variants. Default variant is called 'generic'.  
# If the variant is not 'generic' then we will look in subdirectory names suffixed with PROJECT_VARIANT_DIR_SUFFIX 
# for additional specialized code and the resulting package will be labelled with the PROJECT_VARIANT_LABEL.

PROJECT_VARIANT?=generic

ifeq ($(PROJECT_VARIANT),generic)
PROJECT_VARIANT_LABEL?=
PROJECT_VARIANT_DIR_SUFFIX?=
else
PROJECT_VARIANT_LABEL?=_$(PROJECT_VARIANT)
PROJECT_VARIANT_DIR_SUFFIX?=-$(PROJECT_VARIANT)
endif

PROJECT_MAINTAINER?=project_maintainer

PROJECT_COPYRIGHT?=project_copyright

PROJECT_COMPANY?=project_companyname

PROJECT_WEBSITE?=project_website

PROJECT_DESCRIPTION?=project_description

# We are not cross compiling by default
CROSS_COMPILING?=0

# PKG_CONFIG_PACKAGES contains lists of packages handled by pkg-config to be used
PKGCONFIG_PACKAGES?=

# Default to no packaging or install tool
PACKAGER?=none

# CONFIG_TOOLS are like PKGCONFIG_PACKAGES but not handled by 'pkg-config', they are called directly
CONFIG_TOOLS?=

# TOP_LIB_DIRS is a list of relative directories from the PROJECT_TOP_DIR that each contain
# an "include", "src", "tools", "tests" and other directories. Typically for a single project
# TOP_LIB_DIRS is set to '.' in project.mak

# LIB_DIRS is then calculated with the fully qualified directory name for each directory in TOP_LIB_DIRS
LIB_DIRS+=$(call add_top_dir_prefix,$(TOP_LIB_DIRS))

# by default do not build shared libraries
ENABLE_SHLIB?=0

# by default build static libraries
ENABLE_STATICLIB?=1


