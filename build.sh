#!/bin/bash 
#this script is for RELEASE only

params="$1"


case $params in

	build)
	make -f Makefile.linux_release
	;;

	build-assets)
	make -f Makefile.linux_release create_zip
	;;

	*)
	make -f Makefile.linux_release
	;;
esac
