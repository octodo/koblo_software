#! /bin/sh

project="$1"
version="$2"
license="$3"
relative_dir="$(dirname "$0")"
project_top_dir="$(cd ${relative_dir}/.. && pwd)"
subdirs="include src docs docs-dev tests tools tools-dev gui gui-dev examples examples-dev share/common share-dev/commoon etc daemons"

cd "$project_top_dir"

usage() {
    (
        echo "usage:"
        echo "  $0 PROJECT_NAME PROJECT_VERSION LICENSE "
        echo ""
        echo "LICENSE possibilities:"
        for i in autobuild/generic/licenses/*.txt; do 
            echo $(basename "${i%.txt}")
        done
    ) 1>&2
}

if [ -z "$project" ]; then
    echo "No project name specified on command line." 1>&2
    usage
    exit 1
fi

if [ -z "$version" ]; then
    echo "No project version specified on command line." 1>&2
    usage
    exit 1
fi

if [ -z "$license" ]; then
    echo "Error: No license specified on command line" 1>&2
    usage
    exit 1
fi

mkdir -p ${subdirs}

ln -s autobuild/generic/package . 
ln -s autobuild/configure 
ln -s ../autobuild/docs-dev/Doxyfile docs-dev/

cp "autobuild/generic/licenses/$license.txt" LICENSE.txt
touch README.txt

(
    echo "PROJECT=$project"
    echo "PROJECT_NAME=$project"
    echo "PROJECT_MAINTAINER=$USER"
    echo "PROJECT_COPYRIGHT=$(date +%G)"
    echo "PROJECT_EMAIL=$USER@$HOSTNAME"
    echo "PROJECT_LICENSE=$license"
    echo "PROJECT_LICENSE_FILE=\$(PROJECT_TOP_DIR)/LICENSE.txt"
    echo "PROJECT_README_FILE=\$(PROJECT_TOP_DIR)/README.txt"
    echo "PROJECT_COMPANY=project_company"
    echo "PROJECT_WEBSITE=project_website"
    echo "PROJECT_DESCRIPTION=project_description"
    echo "PROJECT_VERSION=$version"
    echo "PROJECT_VARIANT?=generic"
    echo "TOP_LIB_DIRS+=."
    echo "SUBLIBS="
    echo "INCLUDES+="
    echo "DEFINES+="
    echo "COMPILE_FLAGS+=-Wall"
    echo "LINK_FLAGS+="
    echo "PKGCONFIG_PACKAGES+="
    echo "CONFIG_TOOLS+="
) >project.mak

(
    echo "PROJECT=\"$project\""
    echo "PROJECT_NAME=\"$project\"" 
    echo "PROJECT_VERSION=$version"
) >project.sh





