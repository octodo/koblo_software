#!/bin/bash
Target="DoesntMatter"

iErrors=0

pwd
cd "`dirname "$0"`/resources/"

echo ""
echo "Locate Tools/ folder..." 
sToolsDir=
sTest="`dirname "$0"`"
sTest="`dirname "$sTest"`"
while [ "$sToolsDir" == "" -a "$sTest" != "/" ]; do
	if [ "$sTest" == "." -o "$sTest" == ".." ]; then
		# Break out of relative path
		sTest="`pwd`"
	fi
	sTest="`dirname "$sTest"`"
	echo "$sTest"
	if [ -d "$sTest/Tools" ]; then
		sToolsDir="$sTest/Tools"
		echo "$sToolsDir"
	fi
done

if [ "$sToolsDir" == "" ]; then
	echo "ERROR: Unable to locate Tools/ folder"
	let iErrors+=1
fi

sResComp="$sToolsDir/OS X/ResCompOSX"
sResInc="$sToolsDir/OS X/ResourceToInclude"

if [ "$iErrors" == "0" ]; then
	if [ ! -x "$sResComp" ]; then
		echo "Making $sResComp executable"
		chmod +x "$sResComp"
		if [ $? != 0 ]; then let iErrors+=1
		fi
	fi
fi

if [ "$iErrors" == "0" ]; then
	if [ ! -x "$sResInc" ]; then
		echo "Making $sResInc executable"
		chmod +x "$sResInc"
		if [ $? != 0 ]; then let iErrors+=1
		fi
	fi
fi

if [ "$iErrors" != "0" ]; then
	exit 1
fi

#pwd
echo ""
echo Y | "$sResComp"
iRcCompiler=$?
echo ""
if [ "$iRcCompiler" = "0" ]; then
	echo "Resources compiler reported success - now handle its output:"
	if [ -r $Target.k2s ]; then
		#echo "rm $Target.k2s"
		rm $Target.k2s
	fi
	
	echo "Build include file of .k2s:"
	"$sResInc" ResourcesInc.cpp res.k2s
	if [ $? != 0 ]; then let iErrors+=1
	fi
	cmp -s ResourcesInc.cpp ../Source/ResourcesInc.cpp
	if [ $? == 0 ]; then
		echo "ResourcesInc.cpp unchanged: Skipped replace."
		rm ResourcesInc.cpp
	else
		mv -v ResourcesInc.cpp ../Source/ResourcesInc.cpp
		if [ $? != 0 ]; then let iErrors+=1
		fi
	fi
	
	#mv -v res.k2s $Target.k2s
	#if [ $? != 0 ]; then let iErrors+=1
	#fi

	#if [ ! -d "$TargetDir0" ]; then
	#	#chown -v "$USER:$USER" "$TargetDir0_FirstPart"
	#	chmod -v +w "$TargetDir0_FirstPart"
	#	mkdir -pv "$TargetDir0"
	#fi
	#if [ -r "$TargetDir0/$Target.k2s" ]; then
	#	echo "rm $TargetDir0/$Target.k2s"
	#	rm "$TargetDir0/$Target.k2s"
	#fi
	#cp -v $Target.k2s "$TargetDir0/"
	#if [ $? != 0 ]; then let iErrors+=1
	#fi

	cmp --quiet Resources.h ../include/Resources.h
	if [ $? == 0 ]; then
		echo "Resources.h unchanged: Skipped replace."
		rm Resources.h
	else
		if [ -r ../include/Resources.h ]; then
			echo "rm ../include/Resources.h"
			rm ../include/Resources.h
		fi

		mv -v Resources.h ../include/
		if [ $? != 0 ]; then let iErrors+=1
		fi
	fi
else
	echo "Error code $iRcCompiler"
	let iErrors+=1
fi

echo ""
if [ "$iErrors" != "0" ]; then
	echo "ERROR: Encountered $iErrors errors."
else
	echo "Success!"
fi

cd ~