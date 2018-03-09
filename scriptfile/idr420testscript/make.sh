#!/bin/bash

#You can do "source make.sh" in amlogic root dir.
echo $0
if [ "$0" == "-bash" ] || [ "$0" == "bash" ] || [ "$0" == "sh" ];
then
	export_env
	return 0
fi


#You can run this script in any path.
SCRIPT_FILE=$0
JOBS=$(cat /proc/cpuinfo | grep "processor" | wc -l)
export TARGET_PRODUCT=rk3368

echo "********************************************************************"
echo "===================================================================="
echo "                  RK3368 SDK for iDR420 by Routon                   "
echo "===================================================================="
echo "********************************************************************"
#echo "svn up"
#svn up

#sed -i '17c ro.build.hwmodel=IDR420' device/rockchip/rk3368/system.prop
touch build/tools/buildinfo.sh
svn_a=`svn info|awk -F: '/Revision/{print $2}'`
if [ "$svn_a" == "" ];then
	svn_a=`svn info|awk -F: '/版本:/{print $2}'`
fi
svn_b=`echo "${svn_a// /}"|awk 'NR==1{print}'`
export SVN_VERSION="V1.1.0.STD.$svn_b"
echo "======================================"
echo -e "    The svn version of SDK is \033[33m$SVN_VERSION\033[0m"
echo "======================================"
echo $SVN_VERSION > version.txt
#sed -i '7c echo "ro.build.display.id=V1.0.7.STD.'$b'"' build/tools/buildinfo.sh
#sed -i '65c echo "ro.product.version=V1.0.7.STD.'$b'"' build/tools/buildinfo.sh

while [ -L $SCRIPT_FILE ]
do
	SCRIPT_FILE=`readlink $SCRIPT_FILE`
done
cd `dirname $SCRIPT_FILE`


function export_env(){
	source build/envsetup.sh
	lunch rk3368_32-userdebug
}

function make_all(){
	export FACTORY_PACKAGE=no
	touch bootable/recovery/recovery.cpp
	export_env
	make_uboot
	make_kernel
	make_android
}

function make_all_factory(){
	export FACTORY_PACKAGE=yes
	touch bootable/recovery/recovery.cpp
	export_env
	make_uboot
	make_kernel
	make_android
}

#uboot
function make_uboot(){
	cd u-boot
	make distclean
	make rk3368_defconfig
	make -j$JOBS ARCHV=aarch64
	cd ..
}

#kernel
function make_kernel(){
	cd kernel
	# make distclean
	make ARCH=arm64 rockchip_defconfig
	#make -j8 ARCH=arm64 rk3368-box-r88_808.img
	make -j$JOBS ARCH=arm64 rk3368-p9_818.img
	cd ..
}

#android
function make_android(){
	export_env
	make -j$JOBS
}

#make image without ota
function gen_img(){
	export_env
	./mkimage.sh
}

#make image with ota
function gen_img_ota(){
	export_env
	./mkimage.sh ota
}

function gen_ota(){
	export_env
	make otapackage -j$JOBS
	fname=$TARGET_PRODUCT-ota-eng.$USER.zip
	mv -v out/target/product/$TARGET_PRODUCT/$fname out/target/product/$TARGET_PRODUCT/update-ota.zip
}

function update_api(){
	export_env
	make update-api
}

function copy_jars(){
	cp  out/target/common/obj/JAVA_LIBRARIES/framework_intermediates/classes.jar /srv/ftp/framework.jar
	cp  out/target/common/obj/JAVA_LIBRARIES/framework2_intermediates/classes.jar /srv/ftp/framework2.jar
}

function reback_files(){
	cp frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/phone/PhoneStatusBar.java_js frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/phone/PhoneStatusBar.java
	cp frameworks/base/policy/src/com/android/internal/policy/impl/PhoneWindowManager.java_js  frameworks/base/policy/src/com/android/internal/policy/impl/PhoneWindowManager.java

	mv routon_patch.sh_bak routon_patch.sh

	if [ -f frameworks/base/core/res/res/drawable-sw720dp-nodpi/default_wallpaper.jpg ];then
		rm frameworks/base/core/res/res/drawable-sw720dp-nodpi/default_wallpaper.jpg
	fi
}

function rebuild_all(){
	rm -rf ./out
	make_all
}


FUNCTION_ARRAY=("help" "make_all" "make_all_factory" "make_uboot" "make_kernel" "make_android" "gen_img" "gen_img_ota" "gen_ota" "update_api" "copy_jars" "reback_files" "rebuild_all")

####DO NOT EDIT CODE UNDER THIS LINE####
FUNCTION_NUM=`expr ${#FUNCTION_ARRAY[@]} - 1`

echo "***********************"
echo -e "    Product:\033[31m$TARGET_PRODUCT\033[0m"
echo -e "    CPU job:\033[31m$JOBS\033[0m"
echo "***********************"
echo "Please Choose Function"
for I in `seq $FUNCTION_NUM`
do
    echo "$I: ${FUNCTION_ARRAY[$I]}"
done


if [ "$1" == "" ];then
    echo "Please Enter Number:"
    read FUNCTION
elif [ "$1" == "d" ]||[ "$1" == "D" ]||[ "$1" == "debug" ];then
	if [ "$2" == "" ];then
	    echo "Please Enter Number:"
	    read FUNCTION
	else
	    FUNCTION=$2
	fi
else
	FUNCTION=$1
fi
echo ""

for I in `seq $FUNCTION_NUM`
do
    if [ "$FUNCTION" == "$I" -o "$FUNCTION" == "${FUNCTION_ARRAY[$I]}" ];
    then
        echo "Choose ${FUNCTION_ARRAY[$I]}"
        ${FUNCTION_ARRAY[$I]}
        exit $?
    fi
done
echo "SCRIPT ERROR: FUNCTION \"$FUNCTION\" NOT EXIST!"

