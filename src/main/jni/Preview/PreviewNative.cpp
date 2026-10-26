#include <jni.h>
#include <GLES3/gl3.h>
#include <chrono>
#include <algorithm>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "PreviewState.h"
#include "UI/ApplicationUI.h"

namespace {
using Clock = std::chrono::steady_clock;
Clock::time_point lastFrame;
bool initializedState = false;
int previousWidth = 0, previousHeight = 0;

void Destroy() {
    if (!ImGui::GetCurrentContext()) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    StudioUI::BodyFont = StudioUI::TitleFont = nullptr;
    StudioUI::ResetGestures();
}
void Create() {
    Destroy();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.BackendPlatformName = "studio_android_surface";
    io.ConfigInputTrickleEventQueue = true; // Preserve taps shorter than a render frame.
    StudioUI::Initialize(io);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    if (!initializedState) { InitializePreviewState(); initializedState = true; }
    lastFrame = Clock::now();
    previousWidth = previousHeight = 0;
}
ImGuiKey Key(int key) {
    switch (key) {
        case 19: return ImGuiKey_UpArrow;
        case 20: return ImGuiKey_DownArrow;
        case 21: return ImGuiKey_LeftArrow;
        case 22: return ImGuiKey_RightArrow;
        case 61: return ImGuiKey_Tab;
        case 62: return ImGuiKey_Space;
        case 66: return ImGuiKey_Enter;
        case 67: return ImGuiKey_Backspace;
        case 111: return ImGuiKey_Escape;
        case 112: return ImGuiKey_Delete;
        case 122: return ImGuiKey_Home;
        case 123: return ImGuiKey_End;
        default: return ImGuiKey_None;
    }
}
}

extern "C" JNIEXPORT void JNICALL
Java_com_android_support_ImGuiView_nativeCreate(JNIEnv*, jclass) { Create(); }

extern "C" JNIEXPORT void JNICALL
Java_com_android_support_ImGuiView_nativeDestroy(JNIEnv*, jclass) { Destroy(); }

extern "C" JNIEXPORT jboolean JNICALL
Java_com_android_support_ImGuiView_nativeFrame(JNIEnv*, jclass, jint width, jint height) {
    if (width <= 0 || height <= 0) return JNI_FALSE;
    if (!ImGui::GetCurrentContext()) Create();
    auto& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);
    io.DisplayFramebufferScale = ImVec2(1, 1);
    const auto now = Clock::now();
    io.DeltaTime = std::max(.001f, std::min(.1f, std::chrono::duration<float>(now-lastFrame).count()));
    lastFrame = now;
    if (width != previousWidth || height != previousHeight) {
        GImGui->InputEventsQueue.clear();
        io.ClearInputKeys();
        ImGui::ClearActiveID();
        StudioUI::ResetGestures();
        previousWidth = width; previousHeight = height;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    StudioUI::Draw(); // The very same menu, fonts, custom drawing and widgets as Main.cpp.
    ImGui::Render();
    glViewport(0, 0, width, height);
    glClearColor(.065f, .08f, .09f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return io.WantTextInput ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_android_support_ImGuiView_nativeTouch(JNIEnv*, jclass, jint action, jfloat x, jfloat y) {
    if (!ImGui::GetCurrentContext()) return;
    auto& io = ImGui::GetIO();
    if (action == 3) {
        // Cancellation must discard an unfinished tap, not activate its button.
        // This project pins Dear ImGui 1.89.6, whose ClearInputKeys also resets mouse state.
        GImGui->InputEventsQueue.clear();
        io.ClearInputKeys();
        ImGui::ClearActiveID();
        StudioUI::ResetGestures();
        return;
    }
    io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
    io.AddMousePosEvent(x, y);
    if (action == 0) {
        io.AddMouseButtonEvent(0, true);
    } else if (action == 1) {
        io.AddMouseButtonEvent(0, false);
    }
}
extern "C" JNIEXPORT void JNICALL
Java_com_android_support_ImGuiView_nativeScroll(JNIEnv*, jclass, jfloat x, jfloat y) {
    if (ImGui::GetCurrentContext()) ImGui::GetIO().AddMouseWheelEvent(x, y);
}
extern "C" JNIEXPORT void JNICALL
Java_com_android_support_ImGuiView_nativeBottomInset(JNIEnv*, jclass, jint pixels) {
    StudioUI::BottomInset=static_cast<float>(std::max(0,static_cast<int>(pixels)));
}
extern "C" JNIEXPORT void JNICALL
Java_com_android_support_ImGuiView_nativeKey(JNIEnv*, jclass, jint key, jboolean down) {
    if (!ImGui::GetCurrentContext()) return;
    const auto mapped = Key(key);
    if (mapped != ImGuiKey_None) ImGui::GetIO().AddKeyEvent(mapped, down == JNI_TRUE);
}
extern "C" JNIEXPORT void JNICALL
Java_com_android_support_ImGuiView_nativeText(JNIEnv* env, jclass, jstring text) {
    if (!text || !ImGui::GetCurrentContext()) return;
    // JNI UTF-16 avoids modified-UTF8 corruption of supplementary characters.
    const jchar* chars = env->GetStringChars(text, nullptr);
    if (!chars) return;
    const jsize length = env->GetStringLength(text);
    for (jsize i = 0; i < length; ++i) ImGui::GetIO().AddInputCharacterUTF16(chars[i]);
    env->ReleaseStringChars(text, chars);
}
