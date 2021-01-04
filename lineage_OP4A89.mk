#
# Copyright (C) 2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from guacamole device
$(call inherit-product, device/oppo/OP4A89/device.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := lineage_OP4A89
PRODUCT_DEVICE := OP4A89
PRODUCT_BRAND := OPPO
PRODUCT_MODEL := OPPO Reno Ace
PRODUCT_MANUFACTURER := OPPO

PRODUCT_AAPT_CONFIG := xxhdpi
PRODUCT_AAPT_PREF_CONFIG := xxhdpi
PRODUCT_CHARACTERISTICS := nosdcard

# Boot animation
TARGET_SCREEN_HEIGHT := 2400
TARGET_SCREEN_WIDTH := 1080

# Build info
BUILD_FINGERPRINT := "OPPO/PCLM10/OP4A89:10/QKQ1.191021.002/1587105942:user/release-keys"
PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_DEVICE=OP4A89 \
    PRODUCT_NAME=PCLM10 \
    PRIVATE_BUILD_DESC="OP4A89-user 10 QKQ1.191021.002 1587105942 release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-oppo
