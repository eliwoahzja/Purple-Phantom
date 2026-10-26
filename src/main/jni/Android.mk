# White Crowns native build entry point.
# Production is the default. The preview target is opt-in.
LOCAL_PATH := $(call my-dir)

ifeq ($(WC_BUILD_PREVIEW),1)
    include $(LOCAL_PATH)/Android-preview.mk
else
    include $(LOCAL_PATH)/Android-game.mk
endif
