#
# Copyright (C) 2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

$(call inherit-product, device/oppo/OP4A89/OP4A89.mk)

# Inherit some common ArrowOS stuff.
$(call inherit-product, vendor/arrow/config/common.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := arrow_OP4A89
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
BUILD_FINGERPRINT := "OPPO/PCLM10/OP4A89:10/QKQ1.191021.002/1587105942:user/release-keys"
PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_DEVICE=OP4A89 \
    PRODUCT_NAME=PCLM10 \
    PRIVATE_BUILD_DESC="OP4A89-user 10 QKQ1.191021.002 1587105942 release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-oppo
