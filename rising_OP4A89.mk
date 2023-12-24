#
# Copyright (C) 2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

$(call inherit-product, device/oppo/OP4A89/device.mk)

# Inherit some common stuff
$(call inherit-product, vendor/rising/config/rising.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := rising_OP4A89
PRODUCT_DEVICE := OP4A89
PRODUCT_BRAND := OPPO
PRODUCT_MODEL := PCLM10
PRODUCT_MANUFACTURER := OPPO

PRODUCT_AAPT_CONFIG := xxhdpi
PRODUCT_AAPT_PREF_CONFIG := xxhdpi
PRODUCT_CHARACTERISTICS := nosdcard

# Device maintainer
DEVICE_MAINTAINER := art_chen

# Boot animation
TARGET_BOOT_ANIMATION_RES := 1080

# Build info
#BUILD_FINGERPRINT := "OPPO/PCLM10/OP4A89:10/QKQ1.191021.002/1587105942:user/release-keys"
BUILD_FINGERPRINT := google/redfin/redfin:12/SP1A.210812.015/7679548:user/release-keys
PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_DEVICE=OP4A89 \
    PRODUCT_NAME=PCLM10 \
    PRIVATE_BUILD_DESC="OP4A89-user 10 QKQ1.191021.002 1587105942 release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-oppo

# RisingOS Flags
# chipset flag enclose var with "" if more than one
# friendly tip: builders can use init_xxx.cpp as workaround for spacing
# e.g. property_override("ro.rising.chipset", "Snapdragon 870 5G");
RISING_CHIPSET := "Snapdragon 855+"

# chipset flag enclose var with "" if more than one
# friendly tip: builders can use init_xxx.cpp as workaround for spacing
# e.g. property_override("ro.rising.maintainer", "maintainer");
RISING_MAINTAINER := "Art_Chen"

# chipset flag enclose var with "" if more than one
# this will reflect on build/display version, a firmware package/zip name 
# e.g. risingOS-7.0-COMMUNITY-device-AOSP.zip - AOSP is the default package type, WITH_GMS will override the package type to PIXEL
#RISING_PACKAGE_TYPE := "VANILLA_AOSP"

# disable/enable blur support, default is false
TARGET_ENABLE_BLUR := true

# Enable kernel inline building
INLINE_KERNEL_BUILDING := true

RISING_BATTERY := 4000mAh
RISING_DISPLAY := 2400×1080

# Customized GMS Flags 
# WITH_GMS flag is required

# Default GMS flags sets (Override the values on lineage_xxx.mk)
WITH_GMS := true

# Wether to use google (true) or AOSP (false) telephony package bundle. (defaults: false for gms core, true for pixel builds)
TARGET_USE_GOOGLE_TELEPHONY := true

# Compiler will only build GMS playstore services, its dependencies, and Gboard app.
# package type will change from PIXEL/GMS -> CORE
TARGET_CORE_GMS := false

# extra flag under TARGET_CORE_GMS
TARGET_CORE_GMS_EXTRAS := true

# wether to build sound model (exclusive to raviole only)
TARGET_PREBUILT_SOUND_MODEL := true

# Set pixel launcher as default launcher to be set by QuickSwitch
TARGET_PREBUILT_PIXEL_LAUNCHER := false

# Ship pixel features (adaptivecharging, dreamliner etc)
TARGET_ENABLE_PIXEL_FEATURES := true
