# Production native library. This target contains Main.cpp and the project's
# real runtime integration. It is selected by Android.mk unless
# WC_BUILD_PREVIEW=1 is explicitly supplied.
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := curl_static
LOCAL_SRC_FILES := curl/curl-android-$(TARGET_ARCH_ABI)/lib/libcurl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/curl/curl-android-$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ssl_static
LOCAL_SRC_FILES := curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libssl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/curl/openssl-android-$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := crypto_static
LOCAL_SRC_FILES := curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libcrypto.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/curl/openssl-android-$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := dobby_static
LOCAL_SRC_FILES := IL2CppSDKGenerator/Dobby/libraries/$(TARGET_ARCH_ABI)/libdobby.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/IL2CppSDKGenerator/Dobby
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := foxcheats_static
LOCAL_SRC_FILES := foxcheats/libs/$(TARGET_ARCH_ABI)/libfoxcheats.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/foxcheats $(LOCAL_PATH)/foxcheats/includes
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := LevTutut202222
LOCAL_CFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CPPFLAGS := -std=c++17 -DIMGUI_IMPL_OPENGL_ES3 -Wno-error=format-security -fvisibility=hidden \
    -ffunction-sections -fdata-sections -w -Wno-error=c++11-narrowing \
    -fms-extensions -fno-rtti -fpermissive
LOCAL_CPP_FEATURES := exceptions
LOCAL_LDFLAGS := -Wl,--gc-sections,--strip-all
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
    $(LOCAL_PATH)/ImGui \
    $(LOCAL_PATH)/Includes \
    $(LOCAL_PATH)/curl/curl-android-$(TARGET_ARCH_ABI)/include \
    $(LOCAL_PATH)/curl/openssl-android-$(TARGET_ARCH_ABI)/include \
    $(LOCAL_PATH)/libzip \
    $(LOCAL_PATH)/IL2CppSDKGenerator/Dobby \
    $(LOCAL_PATH)/foxcheats \
    $(LOCAL_PATH)/foxcheats/includes

# Keep the production source set explicit enough to avoid pulling the optional
# Preview JNI bridge into this library.
FILE_LIST := $(wildcard $(LOCAL_PATH)/ImGui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/IL2CppSDKGenerator/IL2Cpp/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/IL2CppSDKGenerator/KittyMemory/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/libzip/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c*)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%) \
    Substrate/hde64.c \
    Substrate/SubstrateDebug.cpp \
    Substrate/SubstrateHook.cpp \
    Substrate/SubstratePosixMemory.cpp \
    Substrate/And64InlineHook.cpp \
    Noctivagous/md5.cpp

LOCAL_STATIC_LIBRARIES := curl_static ssl_static crypto_static dobby_static foxcheats_static
LOCAL_LDLIBS := -llog -landroid -ldl -lz -lEGL -lGLESv1_CM -lGLESv2 -lGLESv3

include $(BUILD_SHARED_LIBRARY)
