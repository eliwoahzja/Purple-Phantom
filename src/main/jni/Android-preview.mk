# Optional local UI preview. It is not the default Android target.
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := studio_preview
LOCAL_CPPFLAGS := -std=c++17 -DSTUDIO_STANDALONE_PREVIEW -DIMGUI_IMPL_OPENGL_ES3
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/ImGui
LOCAL_SRC_FILES := Preview/PreviewNative.cpp \
    ImGui/imgui.cpp ImGui/imgui_draw.cpp ImGui/imgui_tables.cpp \
    ImGui/imgui_widgets.cpp ImGui/imgui_impl_opengl3.cpp
LOCAL_LDLIBS := -lGLESv3 -llog
include $(BUILD_SHARED_LIBRARY)
