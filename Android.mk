LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),OP4A89)
include $(call all-subdir-makefiles,$(LOCAL_PATH))
endif
