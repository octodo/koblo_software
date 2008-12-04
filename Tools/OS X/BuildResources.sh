#!/bin/bash
bError=0
if [ "$CompanyName" == "" ]; then
	echo "ERROR: Variable CompanyName isn't defined."
	bError=1
fi
if [ "$ProductName" == "" ]; then
	echo "ERROR: Variable ProductName isn't defined."
	bError=1
fi

if [ $bError != 0 ]; then
	exit 1
fi

dirname "$0"
sAppFile="`dirname "$0"`/ResCompOSX"
if [ ! -f "$sAppFile" ]; then
	echo "ERROR: $sAppFile missing"
	exit 1
fi

if [ ! -x "$sAppFile" ]; then
	echo "chmod +x \"$sAppFile\""
	chmod +x "$sAppFile"
	if [ $? != 0 ]; then
		echo "ERROR: Unable to make $sAppFile executable"
		exit 1
	fi
fi

if (echo Y | "$sAppFile"); then
	echo "Success"
	SourceFile="$ProductName.k2s"
	if [ -r "$SourceFile" ]; then
		echo "rm $SourceFile"
		rm "$SourceFile"
	fi
	mv res.k2s "$SourceFile"

	if [ "$iResourceDest" == "1" ]; then
		TargetDir="/Library/Application Support/$CompanyName/$ProductName/"
	else
		TargetDir="/Applications/$CompanyName/$ProductName/$ProductName Resources/"
	fi
	if [ ! -d "$TargetDir" ]; then
		mkdir -p "$TargetDir"
	fi
	
	TargetFile="$TargetDir$ProductName.k2s"
	if [ -r "$TargetFile" ]; then
		echo "rm $TargetFile"
		rm "$TargetFile"
	fi
	echo "cp \"$SourceFile\" \"$TargetDir\""
	cp "$SourceFile" "$TargetDir"

	if [ -r ../include/Resources.h ]; then
		echo "rm ../include/Resources.h"
		rm ../include/Resources.h
	fi
	mv Resources.h ../include/
	echo "Press enter to exit"
	cat
	exit 0
else
	echo "Error code $?"
	echo "Press enter to exit"
	cat
	exit 1
fi