#!/bin/bash
Target="OMPS"
pwd
cd "`dirname $0`"
#cd /Development/OMPS/Products/KeyToSound/OMPS/resources
pwd
if (echo Y | ./ResCompOSX); then
	echo "Success"
	if [ -r $Target.k2s ]; then
		echo "rm $Target.k2s"
		rm $Target.k2s
	fi
	mv res.k2s $Target.k2s

	if [ -r "/Applications/KeyToSound/OMPS/OMPS Resources/$Target.k2s" ]; then
		echo "rm /Applications/KeyToSound/OMPS/OMPS Resources/$Target.k2s"
		rm "/Applications/KeyToSound/OMPS/OMPS Resources/$Target.k2s"
	fi
	cp $Target.k2s "/Applications/KeyToSound/OMPS/OMPS Resources/"

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
cd ~