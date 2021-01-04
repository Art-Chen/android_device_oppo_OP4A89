#! /system/bin/sh
#***********************************************************
#** Copyright (C), 2008-2016, OPPO Mobile Comm Corp., Ltd.
#** VENDOR_EDIT
#** 
#** Version: 1.0
#** Date : 2019/03/01
#** Author: Tao.Hui@PSW.CN.WiFi.Basic.Custom.1884719 2019/03/08
#** add for bdf fw rus
#**
#** ---------------------Revision History: ---------------------
#**  <author>    <data>       <version >       <desc>
#**  Tao.Hui       2019/03/01     1.0     build this module
#****************************************************************/


config="$1"

sauDir="/data/oppo/common/sau_res/res/SAU-AUTO_LOAD_FW-10/wifi"
bdfDateDir="/data/misc/wifi/sau"
bdfPersistDir="/mnt/vendor/persist"

# cp sau file from temp data dir to persist
function bdfAutoUpdate() {
    # Notice that /storage/persist is link of /mnt/vendor/persist

    echo bdfAutoUpdate cp from Temp dataDir to persist beging.

    cp -f ${bdfDateDir}/* ${bdfPersistDir}

    echo bdfAutoUpdate end cp start sh

    source /vendor/bin/init.oppo.wifi.sh
}

# cp sau file to temp data dir
function sauFileTransfer() {

   persistPath=${bdfPersistDir}/bin_version
   dataPath=${sauDir}/bin_version

   echo persistPath= ${persistPath} dataPath=${dataPath}
   if [ -s ${persistPath} ]; then
       persist_version=`cat ${persistPath}`
   else
       persist_version=1
   fi

   if [ -s ${dataPath} ]; then
       system_version=`cat ${dataPath}`
   else
       system_version=1
   fi

   echo system_version = $system_version  persist_version=$persist_version
   
   #check the version of sau and persist,if ok we will do the update
   if [ $system_version -gt $persist_version ]; then
       #every times update clear the old temp file
       rm -rf ${bdfDateDir}/*
       echo bdfAutoUpdate cp from SauDir to Temp dataDir beging.
       cp -f ${sauDir}/* ${bdfDateDir}
       echo bdfAutoUpdate a flag of cp finished.
       touch ${bdfDateDir}/finish
       chown system:system ${bdfDateDir}/*
   fi
}

function wifiFwAssert() {
    echo Notice that trigger a fw recovery.
    iwpriv wlan0 crash_inject 1 0
}

case "$config" in
        "bdfAutoUpdate")
        bdfAutoUpdate
    ;;
        "sauFileTransfer")
        sauFileTransfer
    ;;
        "wifiFwAssert")
        wifiFwAssert
    ;;
esac
