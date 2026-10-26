#include "Call_ImGui.h"
 #include "Includes/Logger.h"
#include "Includes/Macros.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "Includes/Jni_Stuff.h"

#include "Noctivagous/StrEnc.h"
#include "Noctivagous/json.hpp"
#include "Noctivagous/md5.h"

#include "oxorany/source/oxorany.h"
#include "oxorany/source/oxorany.cpp"
#include "oxorany/source/oxorany_include.h"


#include "IL2CppSDKGenerator/BasicStructs/Call_BasicStructs.h"
#include "IL2CppSDKGenerator/IL2Cpp/Call_IL2Cpp.h"
#include "IL2CppSDKGenerator/KittyMemory/MemoryPatch.h"


#include "configs.h"
#include "saved.h"
#include "Hacks/Hacks.h"
#include "foxcheats/include/ScanEngine.hpp"
#include "curl/curl.h"
#include "hooks.h"

#include "ImGui/imgui_internal.h"
#include "ImGui/stb_image.h"
#include "Call_ImGui.h"

#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <array>
#include <unordered_map>
#include <deque>
#include <EGL/egl.h>
#include <functional>
#include <GLES3/gl3.h>
#include <thread>
#include <chrono>
#include <atomic>



//==================== HIDE RECORD - SINGLE FILE ====================
// All implementations of HideRecord have been intentionally placed in Main.cpp to avoid the need for a HideRec folder.
#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_activity.h>
#include <android/native_window_jni.h>
#include <dlfcn.h>
#include <sys/system_properties.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>



namespace android::anwcreator::detail::types
{

    enum class SurfaceControlFlags : uint32_t
    {
        eHidden          = 0x00000004,
        eSkipScreenshot  = 0x00000040,
        eSecure          = 0x00000080,
        eNonPremultiplied= 0x00000100,
        eOpaque          = 0x00000400,
        eNoColorFill     = 0x00004000,
    };

    enum class DisplayRotation : int32_t
    {
        Rotation0   = 0,
        Rotation90  = 1,
        Rotation180 = 2,
        Rotation270 = 3
    };

    template <typename enum_t, typename = std::enable_if_t<std::is_enum_v<enum_t>>>
    constexpr enum_t operator|(enum_t lhs, enum_t rhs)
    {
        using underlying_t = std::underlying_type_t<enum_t>;
        return static_cast<enum_t>(static_cast<underlying_t>(lhs) | static_cast<underlying_t>(rhs));
    }

    template <typename enum_t, typename = std::enable_if_t<std::is_enum_v<enum_t>>>
    constexpr enum_t operator|=(enum_t &lhs, enum_t rhs)
    {
        return (lhs = lhs | rhs);
    }

} // namespace android::anwcreator::detail::types

namespace android::anwcreator::detail::jni
{

    struct JNIEnvironment
    {
        JNIEnv *env;
        JavaVM *vm;

        JNIEnvironment(JavaVM *javaVM) : env(nullptr), vm(javaVM)
        {
            if (vm)
                vm->AttachCurrentThread(&env, nullptr);
        }

        ~JNIEnvironment() {}

        inline bool IsValid() const
        {
            return env != nullptr;
        }

        inline operator JNIEnv *() const
        {
            return env;
        }

        inline JNIEnv *operator->() const
        {
            return env;
        }

        inline bool CheckException(const char * /*context*/ = nullptr)
        {
            if (!env || !env->ExceptionCheck())
                return false;

            env->ExceptionDescribe();
            env->ExceptionClear();
            return true;
        }
    };

    struct LocalRef
    {
        JNIEnv *env;
        jobject obj;

        LocalRef(JNIEnv *e, jobject o) : env(e), obj(o) {}

        ~LocalRef()
        {
            if (env && obj)
                env->DeleteLocalRef(obj);
        }

        LocalRef(const LocalRef &) = delete;
        LocalRef &operator=(const LocalRef &) = delete;

        LocalRef(LocalRef &&other) noexcept : env(other.env), obj(other.obj)
        {
            other.obj = nullptr;
        }

        inline operator jobject() const { return obj; }
        inline operator jclass() const { return reinterpret_cast<jclass>(obj); }
        inline jobject get() const { return obj; }
        inline jclass getClass() const { return reinterpret_cast<jclass>(obj); }
        inline bool IsValid() const { return obj != nullptr; }
        inline explicit operator bool() const { return obj != nullptr; }
    };

    struct GlobalRef
    {
        JNIEnv *env;
        jobject obj;

        GlobalRef(JNIEnv *e, jobject o) : env(e), obj(nullptr)
        {
            if (e && o)
                obj = e->NewGlobalRef(o);
        }

        ~GlobalRef()
        {
            Release();
        }

        void Release()
        {
            if (env && obj)
            {
                env->DeleteGlobalRef(obj);
                obj = nullptr;
            }
        }

        GlobalRef(const GlobalRef &) = delete;
        GlobalRef &operator=(const GlobalRef &) = delete;

        GlobalRef(GlobalRef &&other) noexcept : env(other.env), obj(other.obj)
        {
            other.obj = nullptr;
        }

        inline operator jobject() const { return obj; }
        inline jobject get() const { return obj; }
        inline bool IsValid() const { return obj != nullptr; }
        inline explicit operator bool() const { return obj != nullptr; }
    };

} // namespace android::anwcreator::detail::jni

namespace android::anwcreator::detail::framework
{

    struct DisplayMetrics
    {
        int32_t widthPixels;
        int32_t heightPixels;
        float   density;
        int32_t densityDpi;
    };

    struct DisplayInfo
    {
        int32_t                    width;
        int32_t                    height;
        types::DisplayRotation     rotation;
        float                      refreshRate;
    };

    class Display
    {
    public:
        static DisplayInfo GetDisplayInfo(JNIEnv *env, ANativeActivity *activity)
        {
            DisplayInfo info{};

            if (!env || !activity || !activity->clazz)
                return info;

            jni::LocalRef activityCls(env, env->GetObjectClass(activity->clazz));
            if (!activityCls)
                return info;

            jmethodID getWindowManager =
                env->GetMethodID(activityCls, "getWindowManager", "()Landroid/view/WindowManager;");
            if (!getWindowManager)
                return info;

            jni::LocalRef windowManager(env,
                                        env->CallObjectMethod(activity->clazz, getWindowManager));
            if (!windowManager || jni::JNIEnvironment(activity->vm).CheckException("Activity.getWindowManager()"))
            {
                return info;
            }

            jni::LocalRef windowManagerCls(env, env->GetObjectClass(windowManager));
            jmethodID getDefaultDisplay =
                env->GetMethodID(windowManagerCls, "getDefaultDisplay", "()Landroid/view/Display;");
            if (!getDefaultDisplay)
                return info;

            jni::LocalRef display(env,
                                  env->CallObjectMethod(windowManager, getDefaultDisplay));
            if (!display || jni::JNIEnvironment(activity->vm).CheckException("WindowManager.getDefaultDisplay()"))
            {
                return info;
            }

            jni::LocalRef displayMetricsCls(env, env->FindClass("android/util/DisplayMetrics"));
            jmethodID displayMetricsCtor =
                env->GetMethodID(displayMetricsCls, "<init>", "()V");
            jni::LocalRef displayMetrics(env,
                                         env->NewObject(displayMetricsCls, displayMetricsCtor));

            jni::LocalRef displayCls(env, env->GetObjectClass(display));
            jmethodID getRealMetrics =
                env->GetMethodID(displayCls, "getRealMetrics", "(Landroid/util/DisplayMetrics;)V");
            env->CallVoidMethod(display, getRealMetrics, displayMetrics.get());

            jfieldID widthPixelsField  = env->GetFieldID(displayMetricsCls, "widthPixels", "I");
            jfieldID heightPixelsField = env->GetFieldID(displayMetricsCls, "heightPixels", "I");

            info.width  = env->GetIntField(displayMetrics, widthPixelsField);
            info.height = env->GetIntField(displayMetrics, heightPixelsField);

            jmethodID getRotation = env->GetMethodID(displayCls, "getRotation", "()I");
            int32_t   rotation    = env->CallIntMethod(display, getRotation);
            info.rotation         = static_cast<types::DisplayRotation>(rotation);

            jmethodID getRefreshRate =
                env->GetMethodID(displayCls, "getRefreshRate", "()F");
            if (getRefreshRate)
            {
                info.refreshRate = env->CallFloatMethod(display, getRefreshRate);
            }

            return info;
        }
    };

    class SurfaceControl
    {
    public:
        struct Builder
        {
            JNIEnv *env;
            jobject builder;
            jclass  builderClass;

            Builder(JNIEnv *e) : env(e), builder(nullptr), builderClass(nullptr)
            {
                builderClass = env->FindClass("android/view/SurfaceControl$Builder");
                if (!builderClass)
                    return;

                jmethodID ctor = env->GetMethodID(builderClass, "<init>", "()V");
                builder        = env->NewObject(builderClass, ctor);
            }

            ~Builder()
            {
                if (env && builder)
                    env->DeleteLocalRef(builder);
                if (env && builderClass)
                    env->DeleteLocalRef(builderClass);
            }

            Builder &SetName(const char *name)
            {
                if (!builder)
                    return *this;

                jmethodID setName = env->GetMethodID(
                    builderClass,
                    "setName",
                    "(Ljava/lang/String;)Landroid/view/SurfaceControl$Builder;");
                if (setName)
                {
                    jni::LocalRef jName(env, env->NewStringUTF(name));
                    builder = env->CallObjectMethod(builder, setName, jName.get());
                }
                return *this;
            }

            Builder &SetParent(jobject parent)
            {
                if (!builder || !parent)
                    return *this;

                jmethodID setParent = env->GetMethodID(
                    builderClass,
                    "setParent",
                    "(Landroid/view/SurfaceControl;)Landroid/view/SurfaceControl$Builder;");
                if (setParent)
                {
                    builder = env->CallObjectMethod(builder, setParent, parent);
                }
                return *this;
            }

            Builder &SetBufferSize(int32_t width, int32_t height)
            {
                if (!builder)
                    return *this;

                jmethodID setBufferSize = env->GetMethodID(
                    builderClass,
                    "setBufferSize",
                    "(II)Landroid/view/SurfaceControl$Builder;");
                if (setBufferSize)
                {
                    builder = env->CallObjectMethod(builder, setBufferSize, width, height);
                }
                return *this;
            }

            Builder &SetFormat(int32_t format)
            {
                if (!builder) return *this;
                jmethodID setFormat = env->GetMethodID(
                    builderClass,
                    "setFormat",
                    "(I)Landroid/view/SurfaceControl$Builder;");
                if (setFormat) builder = env->CallObjectMethod(builder, setFormat, (jint)format);
                return *this;
            }

            Builder &SetOpaque(bool opaque)
            {
                if (!builder) return *this;
                jmethodID setOpaque = env->GetMethodID(
                    builderClass,
                    "setOpaque",
                    "(Z)Landroid/view/SurfaceControl$Builder;");
                if (setOpaque) builder = env->CallObjectMethod(builder, setOpaque, (jboolean)opaque);
                return *this;
            }

            Builder &SetFlags(uint32_t flags, uint32_t mask)
            {
                if (!builder)
                    return *this;

                jmethodID setFlags = env->GetMethodID(
                    builderClass,
                    "setFlags",
                    "(II)Landroid/view/SurfaceControl$Builder;");
                if (setFlags)
                {
                    builder = env->CallObjectMethod(builder,
                                                    setFlags,
                                                    static_cast<jint>(flags),
                                                    static_cast<jint>(mask));
                }
                return *this;
            }

            Builder &SetSkipScreenshot(bool skip)
            {
                if (skip)
                {
                    constexpr uint32_t FLAG_SKIP_SCREENSHOT = 0x40;
                    SetFlags(FLAG_SKIP_SCREENSHOT, FLAG_SKIP_SCREENSHOT);
                }
                return *this;
            }

            jobject Build()
            {
                if (!builder)
                    return nullptr;

                jmethodID buildMethod = env->GetMethodID(
                    builderClass,
                    "build",
                    "()Landroid/view/SurfaceControl;");
                if (!buildMethod)
                    return nullptr;

                return env->CallObjectMethod(builder, buildMethod);
            }

            inline bool IsValid() const { return builder != nullptr; }
        };

        class Transaction
        {
        public:
            JNIEnv *env;
            jobject transaction;
            jclass  transactionClass;

            Transaction(JNIEnv *e) : env(e), transaction(nullptr), transactionClass(nullptr)
            {
                transactionClass = env->FindClass("android/view/SurfaceControl$Transaction");
                if (!transactionClass)
                    return;

                jmethodID ctor = env->GetMethodID(transactionClass, "<init>", "()V");
                transaction     = env->NewObject(transactionClass, ctor);
            }

            ~Transaction()
            {
                if (env && transaction)
                    env->DeleteLocalRef(transaction);
                if (env && transactionClass)
                    env->DeleteLocalRef(transactionClass);
            }

            Transaction &SetAlpha(jobject surfaceControl, float alpha)
            {
                if (!transaction || !surfaceControl)
                    return *this;

                jmethodID setAlpha = env->GetMethodID(
                    transactionClass,
                    "setAlpha",
                    "(Landroid/view/SurfaceControl;F)Landroid/view/SurfaceControl$Transaction;");
                if (setAlpha)
                {
                    transaction = env->CallObjectMethod(transaction,
                                                        setAlpha,
                                                        surfaceControl,
                                                        alpha);
                }
                return *this;
            }

            Transaction &SetLayer(jobject surfaceControl, int32_t z)
            {
                if (!transaction || !surfaceControl)
                    return *this;

                jmethodID setLayer = env->GetMethodID(
                    transactionClass,
                    "setLayer",
                    "(Landroid/view/SurfaceControl;I)Landroid/view/SurfaceControl$Transaction;");
                if (setLayer)
                {
                    transaction = env->CallObjectMethod(transaction,
                                                        setLayer,
                                                        surfaceControl,
                                                        z);
                }
                return *this;
            }

            Transaction &SetPosition(jobject surfaceControl, float x, float y)
            {
                if (!transaction || !surfaceControl) return *this;
                jmethodID m = env->GetMethodID(transactionClass,"setPosition","(Landroid/view/SurfaceControl;FF)Landroid/view/SurfaceControl$Transaction;");
                if (m) transaction = env->CallObjectMethod(transaction,m,surfaceControl,x,y);
                return *this;
            }

            Transaction &SetMatrix(jobject surfaceControl, float dsdx, float dtdx, float dtdy, float dsdy)
            {
                if (!transaction || !surfaceControl) return *this;
                jmethodID m = env->GetMethodID(transactionClass,"setMatrix","(Landroid/view/SurfaceControl;FFFF)Landroid/view/SurfaceControl$Transaction;");
                if (m) transaction = env->CallObjectMethod(transaction,m,surfaceControl,dsdx,dtdx,dtdy,dsdy);
                return *this;
            }

            Transaction &Show(jobject surfaceControl)
            {
                if (!transaction || !surfaceControl)
                    return *this;

                jmethodID show = env->GetMethodID(
                    transactionClass,
                    "show",
                    "(Landroid/view/SurfaceControl;)Landroid/view/SurfaceControl$Transaction;");
                if (show)
                {
                    transaction = env->CallObjectMethod(transaction, show, surfaceControl);
                }
                return *this;
            }

            Transaction &Hide(jobject surfaceControl)
            {
                if (!transaction || !surfaceControl)
                    return *this;

                jmethodID hide = env->GetMethodID(
                    transactionClass,
                    "hide",
                    "(Landroid/view/SurfaceControl;)Landroid/view/SurfaceControl$Transaction;");
                if (hide)
                {
                    transaction = env->CallObjectMethod(transaction, hide, surfaceControl);
                }
                return *this;
            }

            // NEW: re-parent a SurfaceControl to a new parent
            Transaction &SetParent(jobject child, jobject newParent)
            {
                if (!transaction || !child || !newParent)
                    return *this;

                jmethodID setParent = env->GetMethodID(
                    transactionClass,
                    "setParent",
                    "(Landroid/view/SurfaceControl;Landroid/view/SurfaceControl;)Landroid/view/SurfaceControl$Transaction;");
                if (setParent)
                {
                    transaction = env->CallObjectMethod(transaction,
                                                        setParent,
                                                        child,
                                                        newParent);
                }
                return *this;
            }

            Transaction &Remove(jobject surfaceControl)
            {
                if (!transaction || !surfaceControl)
                    return *this;

                jmethodID remove = env->GetMethodID(
                    transactionClass,
                    "remove",
                    "(Landroid/view/SurfaceControl;)Landroid/view/SurfaceControl$Transaction;");
                if (remove)
                {
                    transaction = env->CallObjectMethod(transaction, remove, surfaceControl);
                }
                return *this;
            }

            void Apply()
            {
                if (!transaction)
                    return;

                jmethodID apply = env->GetMethodID(transactionClass, "apply", "()V");
                if (apply)
                {
                    env->CallVoidMethod(transaction, apply);
                }
            }

            inline bool IsValid() const { return transaction != nullptr; }
        };

        static jobject GetParentSurfaceControl(JNIEnv *env, ANativeActivity *activity)
        {
            if (!env || !activity || !activity->clazz)
                return nullptr;

            jni::LocalRef activityCls(env, env->GetObjectClass(activity->clazz));
            jmethodID getWindow =
                env->GetMethodID(activityCls, "getWindow", "()Landroid/view/Window;");
            if (!getWindow)
                return nullptr;

            jni::LocalRef window(env,
                                 env->CallObjectMethod(activity->clazz, getWindow));
            if (!window)
                return nullptr;

            jni::LocalRef windowCls(env, env->GetObjectClass(window));
            jmethodID getDecorView =
                env->GetMethodID(windowCls, "getDecorView", "()Landroid/view/View;");
            if (!getDecorView)
                return nullptr;

            jni::LocalRef decorView(env,
                                    env->CallObjectMethod(window, getDecorView));
            if (!decorView)
                return nullptr;

            jni::LocalRef viewCls(env, env->GetObjectClass(decorView));
            jmethodID getViewRootImpl =
                env->GetMethodID(viewCls,
                                 "getViewRootImpl",
                                 "()Landroid/view/ViewRootImpl;");
            if (!getViewRootImpl)
                return nullptr;

            jni::LocalRef viewRootImpl(env,
                                       env->CallObjectMethod(decorView, getViewRootImpl));
            if (!viewRootImpl)
                return nullptr;

            jni::LocalRef viewRootImplCls(env, env->GetObjectClass(viewRootImpl));
            jmethodID getSurfaceControl =
                env->GetMethodID(viewRootImplCls,
                                 "getSurfaceControl",
                                 "()Landroid/view/SurfaceControl;");
            if (!getSurfaceControl)
                return nullptr;

            return env->CallObjectMethod(viewRootImpl, getSurfaceControl);
        }
    };

    class Surface
    {
    public:
        static jobject CreateFromSurfaceControl(JNIEnv *env, jobject surfaceControl)
        {
            if (!env || !surfaceControl)
                return nullptr;

            jni::LocalRef surfaceCls(env, env->FindClass("android/view/Surface"));
            if (!surfaceCls)
                return nullptr;

            jmethodID ctor =
                env->GetMethodID(surfaceCls, "<init>", "(Landroid/view/SurfaceControl;)V");
            if (!ctor)
                return nullptr;

            return env->NewObject(surfaceCls, ctor, surfaceControl);
        }
    };

} // namespace android::anwcreator::detail::framework

namespace android::anwcreator::detail
{
    struct WindowContext
    {
        std::unique_ptr<jni::GlobalRef> surfaceControl;
        std::unique_ptr<jni::GlobalRef> surface;
        std::unique_ptr<jni::GlobalRef> parentSurfaceControl; // NEW: remember parent root
        ANativeWindow                  *nativeWindow;
        int32_t                         width;
        int32_t                         height;
        bool                            skipScreenshot;

        WindowContext()
            : surfaceControl(nullptr),
              surface(nullptr),
              parentSurfaceControl(nullptr),
              nativeWindow(nullptr),
              width(0),
              height(0),
              skipScreenshot(false)
        {
        }

        ~WindowContext()
        {
            Release();
        }

        void Release()
        {
            if (nativeWindow)
            {
                ANativeWindow_release(nativeWindow);
                nativeWindow = nullptr;
            }
            surface.reset();
            surfaceControl.reset();
            parentSurfaceControl.reset();
        }
    };

} // namespace android::anwcreator::detail

namespace android
{
    class ANwCreator
    {
    public:
        struct DisplayInfo
        {
            int32_t theta;
            int32_t width;
            int32_t height;
            float   refreshRate;

            DisplayInfo()
                : theta(0), width(0), height(0), refreshRate(60.0f)
            {
            }
        };

        struct CreateOptions
        {
            const char *name;
            int32_t     width;
            int32_t     height;
            bool        skipScreenshot;

            CreateOptions()
                : name(""),
                  width(-1),
                  height(-1),
                  skipScreenshot(false)
            {
            }
        };

    public:
        static DisplayInfo GetDisplayInfo(ANativeActivity *activity)
        {
            DisplayInfo result{};

            if (!activity || !activity->vm || !activity->clazz)
                return result;

            anwcreator::detail::jni::JNIEnvironment jniEnv(activity->vm);
            if (!jniEnv.IsValid())
                return result;

            auto displayInfo = anwcreator::detail::framework::Display::GetDisplayInfo(jniEnv,
                                                                                      activity);

            result.width       = displayInfo.width;
            result.height      = displayInfo.height;
            result.theta       = 90 * static_cast<int32_t>(displayInfo.rotation);
            result.refreshRate = displayInfo.refreshRate;

            return result;
        }

        static ANativeWindow *Create(ANativeActivity *activity,
                                     const CreateOptions &options = CreateOptions())
        {
            if (!activity || !activity->vm || !activity->clazz)
                return nullptr;

            anwcreator::detail::jni::JNIEnvironment jniEnv(activity->vm);
            if (!jniEnv.IsValid())
                return nullptr;

            int32_t width  = options.width;
            int32_t height = options.height;

            if (width <= 0 || height <= 0)
            {
                auto displayInfo = anwcreator::detail::framework::Display::GetDisplayInfo(jniEnv,
                                                                                          activity);
                width  = displayInfo.width;
                height = displayInfo.height;
            }

            jobject parentSC =
                anwcreator::detail::framework::SurfaceControl::GetParentSurfaceControl(jniEnv,
                                                                                       activity);
            if (!parentSC && jniEnv.CheckException("GetParentSurfaceControl"))
                return nullptr;

            anwcreator::detail::framework::SurfaceControl::Builder builder(jniEnv);
            if (!builder.IsValid())
                return nullptr;

            builder.SetName(options.name)
                .SetBufferSize(width, height)
                .SetFormat(1)       // PixelFormat.RGBA_8888
                .SetOpaque(false)   // alpha buffer must be composited, not treated opaque
                .SetSkipScreenshot(options.skipScreenshot);

            if (parentSC)
                builder.SetParent(parentSC);

            jobject localSurfaceControl = builder.Build();
            if (!localSurfaceControl || jniEnv.CheckException("Builder.build()"))
            {
                if (parentSC)
                    jniEnv->DeleteLocalRef(parentSC);
                return nullptr;
            }

            auto context                = std::make_unique<anwcreator::detail::WindowContext>();
            context->width              = width;
            context->height             = height;
            context->skipScreenshot     = options.skipScreenshot;
            context->surfaceControl     =
                std::make_unique<anwcreator::detail::jni::GlobalRef>(jniEnv, localSurfaceControl);

            // NEW: store parent root SurfaceControl (if any)
            if (parentSC)
            {
                context->parentSurfaceControl =
                    std::make_unique<anwcreator::detail::jni::GlobalRef>(jniEnv, parentSC);
            }

            {
                anwcreator::detail::framework::SurfaceControl::Transaction transaction(jniEnv);
                if (transaction.IsValid())
                {
                    transaction.SetAlpha(context->surfaceControl->get(), 1.0f)
                        .SetLayer(context->surfaceControl->get(), 0x7FFFFFFE)
                        .SetPosition(context->surfaceControl->get(), 0.0f, 0.0f)
                        .SetMatrix(context->surfaceControl->get(), 1.0f, 0.0f, 0.0f, 1.0f)
                        .Show(context->surfaceControl->get())
                        .Apply();
                }
            }

            jobject localSurface =
                anwcreator::detail::framework::Surface::CreateFromSurfaceControl(
                    jniEnv,
                    context->surfaceControl->get());

            if (!localSurface || jniEnv.CheckException("Create Surface"))
            {
                jniEnv->DeleteLocalRef(localSurfaceControl);
                if (parentSC)
                    jniEnv->DeleteLocalRef(parentSC);
                return nullptr;
            }

            context->surface =
                std::make_unique<anwcreator::detail::jni::GlobalRef>(jniEnv, localSurface);

            context->nativeWindow = ANativeWindow_fromSurface(jniEnv,
                                                              context->surface->get());
            if (!context->nativeWindow)
            {
                jniEnv->DeleteLocalRef(localSurface);
                jniEnv->DeleteLocalRef(localSurfaceControl);
                if (parentSC)
                    jniEnv->DeleteLocalRef(parentSC);
                return nullptr;
            }

            ANativeWindow *result = context->nativeWindow;

            m_windowContexts.emplace(result, std::move(context));

            jniEnv->DeleteLocalRef(localSurface);
            jniEnv->DeleteLocalRef(localSurfaceControl);
            if (parentSC)
                jniEnv->DeleteLocalRef(parentSC);

            return result;
        }

        static void Destroy(ANativeActivity *activity, ANativeWindow *nativeWindow)
        {
            if (!nativeWindow)
                return;

            auto it = m_windowContexts.find(nativeWindow);
            if (it == m_windowContexts.end())
            {
                ANativeWindow_release(nativeWindow);
                return;
            }

            auto &context = it->second;

            if (activity && activity->vm && context->surfaceControl && context->surfaceControl->IsValid())
            {
                anwcreator::detail::jni::JNIEnvironment jniEnv(activity->vm);
                if (jniEnv.IsValid())
                {
                    anwcreator::detail::framework::SurfaceControl::Transaction transaction(jniEnv);
                    if (transaction.IsValid())
                        transaction.Remove(context->surfaceControl->get()).Apply();
                }
            }

            context->Release();
            m_windowContexts.erase(it);
        }

        static bool IsValid(ANativeWindow *nativeWindow)
        {
            return nativeWindow && m_windowContexts.count(nativeWindow) > 0;
        }

        static bool GetWindowSize(ANativeWindow *nativeWindow,
                                  int32_t *outWidth,
                                  int32_t *outHeight)
        {
            auto it = m_windowContexts.find(nativeWindow);
            if (it == m_windowContexts.end())
                return false;

            if (outWidth)
                *outWidth = it->second->width;
            if (outHeight)
                *outHeight = it->second->height;

            return true;
        }

        // 🔥 NEW: ensure overlay surface is attached to the current root and visible
        static void EnsureVisible(ANativeActivity *activity, ANativeWindow *nativeWindow)
        {
            if (!activity || !activity->vm || !nativeWindow)
                return;

            auto it = m_windowContexts.find(nativeWindow);
            if (it == m_windowContexts.end())
                return;

            auto &context = it->second;
            if (!context || !context->surfaceControl || !context->surfaceControl->IsValid())
                return;

            anwcreator::detail::jni::JNIEnvironment jniEnv(activity->vm);
            if (!jniEnv.IsValid())
                return;

            anwcreator::detail::framework::SurfaceControl::Transaction transaction(jniEnv);
            if (!transaction.IsValid())
                return;

            // Get current ViewRootImpl SurfaceControl
            jobject currentParent =
                anwcreator::detail::framework::SurfaceControl::GetParentSurfaceControl(
                    jniEnv,
                    activity);

            bool needReparent = false;

            if (currentParent && context->parentSurfaceControl && context->parentSurfaceControl->IsValid())
            {
                if (!jniEnv->IsSameObject(currentParent, context->parentSurfaceControl->get()))
                {
                    needReparent = true;
                }
            }
            else if (currentParent && !context->parentSurfaceControl)
            {
                needReparent = true;
            }

            if (needReparent && currentParent)
            {
                transaction.SetParent(context->surfaceControl->get(), currentParent);

                context->parentSurfaceControl =
                    std::make_unique<anwcreator::detail::jni::GlobalRef>(jniEnv, currentParent);
            }

            // The SurfaceHideRecord is rendered using Unity coordinates (get_width/get_height). 
            //SurfaceControl itself operates in the physical display's coordinate space. If Unity
             // uses a lower render scale or resolution, an identity matrix would cause the entire 
             // menu/ESP to appear shrunken and shift the touch positions. Scale the layer
             // within the compositor; the ImGui data/coordinates do not need to be modified.
            float scaleX = 1.0f, scaleY = 1.0f;
            auto displayInfo = anwcreator::detail::framework::Display::GetDisplayInfo(jniEnv, activity);
            if (context->width > 0 && context->height > 0 && displayInfo.width > 0 && displayInfo.height > 0)
            {
                int32_t targetW = displayInfo.width;
                int32_t targetH = displayInfo.height;
                // Be aware if the vendor reports metrics with the opposite orientation.
                const bool bufferLandscape = context->width >= context->height;
                const bool targetLandscape = targetW >= targetH;
                if (bufferLandscape != targetLandscape)
                    std::swap(targetW, targetH);
                scaleX = (float)targetW / (float)context->width;
                scaleY = (float)targetH / (float)context->height;
                if (!(scaleX > 0.0f) || !(scaleY > 0.0f) || scaleX > 4.0f || scaleY > 4.0f)
                    scaleX = scaleY = 1.0f;
            }

            transaction
                .SetAlpha(context->surfaceControl->get(), 1.0f)
                .SetLayer(context->surfaceControl->get(), 0x7FFFFFFE)
                .SetPosition(context->surfaceControl->get(), 0.0f, 0.0f)
                .SetMatrix(context->surfaceControl->get(), scaleX, 0.0f, 0.0f, scaleY)
                .Show(context->surfaceControl->get())
                .Apply();

            if (currentParent)
                jniEnv->DeleteLocalRef(currentParent);

        }

    private:
        inline static std::unordered_map<ANativeWindow *,
            std::unique_ptr<anwcreator::detail::WindowContext>> m_windowContexts;
    };

} // namespace android

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_window.h>
#include <android/native_activity.h>

namespace android {
class HideRecAImGui {
public:
    struct Options { ANativeActivity* activity=nullptr; bool skipScreenshot=true; };
    using SwapBuffersFn=EGLBoolean (*)(EGLDisplay,EGLSurface);
    HideRecAImGui():HideRecAImGui(Options{}){}
    explicit HideRecAImGui(const Options& options);
    ~HideRecAImGui();
    bool RenderDrawData(ImDrawData* drawData,EGLDisplay gameDisplay,EGLSurface gameSurface,
                        EGLContext gameContext,SwapBuffersFn swapBuffersFn);
    void Shutdown();
    constexpr operator bool() const{return m_state;}
private:
    bool EnsureEnvironment(EGLDisplay gameDisplay,EGLContext gameContext,int32_t width,int32_t height);
    bool InitEnvironment(EGLDisplay gameDisplay,EGLContext gameContext,int32_t width,int32_t height);
    void UnInitEnvironment();
    EGLConfig FindConfig(EGLDisplay display,EGLContext gameContext);
private:
    bool m_state=false;
    int32_t m_screenWidth=-1,m_screenHeight=-1;
    Options m_options{};
    ANativeWindow* m_nativeWindow=nullptr;
    EGLDisplay m_display=EGL_NO_DISPLAY;
    EGLSurface m_surface=EGL_NO_SURFACE;
    EGLContext m_overlayContext=EGL_NO_CONTEXT; // own shared GLES3 context for RGBA surface
    EGLContext m_gameContext=EGL_NO_CONTEXT; // borrowed; never destroy
};
}


#include <EGL/eglext.h>
#include <cmath>

namespace android {
HideRecAImGui::HideRecAImGui(const Options& options):m_options(options){}
HideRecAImGui::~HideRecAImGui(){UnInitEnvironment();}

EGLConfig HideRecAImGui::FindConfig(EGLDisplay display,EGLContext gameContext){
    (void)gameContext;
    const EGLint attrs[]={
        EGL_SURFACE_TYPE,EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,0x00000040, // EGL_OPENGL_ES3_BIT_KHR
        EGL_RED_SIZE,8,EGL_GREEN_SIZE,8,EGL_BLUE_SIZE,8,EGL_ALPHA_SIZE,8,
        EGL_NONE
    };
    EGLConfig cfg=nullptr;
    EGLint count=0;
    if(eglChooseConfig(display,attrs,&cfg,1,&count)==EGL_TRUE&&count>0&&cfg)return cfg;

    EGLint total=0;
    if(eglGetConfigs(display,nullptr,0,&total)!=EGL_TRUE||total<=0)return nullptr;
    std::vector<EGLConfig> configs((size_t)total);
    if(eglGetConfigs(display,configs.data(),total,&total)!=EGL_TRUE)return nullptr;
    for(int i=0;i<total;i++){
        EGLint st=0,rt=0,r=0,g=0,b=0,a=0;
        eglGetConfigAttrib(display,configs[i],EGL_SURFACE_TYPE,&st);
        eglGetConfigAttrib(display,configs[i],EGL_RENDERABLE_TYPE,&rt);
        eglGetConfigAttrib(display,configs[i],EGL_RED_SIZE,&r);
        eglGetConfigAttrib(display,configs[i],EGL_GREEN_SIZE,&g);
        eglGetConfigAttrib(display,configs[i],EGL_BLUE_SIZE,&b);
        eglGetConfigAttrib(display,configs[i],EGL_ALPHA_SIZE,&a);
        if((st&EGL_WINDOW_BIT)&&(rt&0x00000040)&&r>=8&&g>=8&&b>=8&&a>=8)return configs[i];
    }
    return nullptr;
}

bool HideRecAImGui::InitEnvironment(EGLDisplay gameDisplay,EGLContext gameContext,int32_t width,int32_t height){
    if(!m_options.activity||gameDisplay==EGL_NO_DISPLAY||gameContext==EGL_NO_CONTEXT||width<=0||height<=0)return false;
    ANwCreator::CreateOptions opt;
    opt.name="HideRecSecure";
    opt.width=width;
    opt.height=height;
    opt.skipScreenshot=m_options.skipScreenshot;
    m_nativeWindow=ANwCreator::Create(m_options.activity,opt);
    if(!m_nativeWindow)return false;
    m_display=gameDisplay;
    m_gameContext=gameContext;

    EGLConfig config=FindConfig(m_display,gameContext);
    if(!config){UnInitEnvironment();return false;}
    EGLint format=0;
    if(eglGetConfigAttrib(m_display,config,EGL_NATIVE_VISUAL_ID,&format)!=EGL_TRUE){UnInitEnvironment();return false;}
    ANativeWindow_setBuffersGeometry(m_nativeWindow,width,height,format);

    m_surface=eglCreateWindowSurface(m_display,config,m_nativeWindow,nullptr);
    if(m_surface==EGL_NO_SURFACE){UnInitEnvironment();return false;}

    const EGLint ctxAttrs[]={EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
    m_overlayContext=eglCreateContext(m_display,config,gameContext,ctxAttrs);
    if(m_overlayContext==EGL_NO_CONTEXT){UnInitEnvironment();return false;}

    EGLint realW=0,realH=0;
    if(eglQuerySurface(m_display,m_surface,EGL_WIDTH,&realW)==EGL_TRUE&&realW>0)m_screenWidth=realW;else m_screenWidth=width;
    if(eglQuerySurface(m_display,m_surface,EGL_HEIGHT,&realH)==EGL_TRUE&&realH>0)m_screenHeight=realH;else m_screenHeight=height;
    m_state=true;
    ANwCreator::EnsureVisible(m_options.activity,m_nativeWindow);
    return true;
}

bool HideRecAImGui::EnsureEnvironment(EGLDisplay gameDisplay,EGLContext gameContext,int32_t width,int32_t height){
    if(gameDisplay==EGL_NO_DISPLAY||gameContext==EGL_NO_CONTEXT||width<=0||height<=0)return false;
    if(m_state&&(m_display!=gameDisplay||m_gameContext!=gameContext||m_screenWidth!=width||m_screenHeight!=height))UnInitEnvironment();
    if(!m_state&&!InitEnvironment(gameDisplay,gameContext,width,height))return false;
    return true;
}

bool HideRecAImGui::RenderDrawData(ImDrawData* drawData,EGLDisplay gameDisplay,EGLSurface gameSurface,
                                   EGLContext gameContext,SwapBuffersFn swapBuffersFn){
    if(!drawData||!swapBuffersFn||gameSurface==EGL_NO_SURFACE)return false;
    // ImGui DisplaySize is populated from Unity's get_width()/get_height(), so use a 1:1 ratio.
    int32_t wantedW=(int32_t)std::lround(drawData->DisplaySize.x);
    int32_t wantedH=(int32_t)std::lround(drawData->DisplaySize.y);
    if(wantedW<=0||wantedH<=0)return false;
    if(!EnsureEnvironment(gameDisplay,gameContext,wantedW,wantedH))return false;

    EGLDisplay oldDisplay=eglGetCurrentDisplay();
    EGLSurface oldDraw=eglGetCurrentSurface(EGL_DRAW);
    EGLSurface oldRead=eglGetCurrentSurface(EGL_READ);
    EGLContext oldContext=eglGetCurrentContext();

    // The second context shares resources with the game context but uses an EGLConfig with an alpha channel (RGBA).
    if(m_overlayContext==EGL_NO_CONTEXT||eglMakeCurrent(m_display,m_surface,m_surface,m_overlayContext)!=EGL_TRUE)return false;
    EGLint vw=0,vh=0;
    if(eglQuerySurface(m_display,m_surface,EGL_WIDTH,&vw)!=EGL_TRUE||vw<=0)vw=m_screenWidth;
    if(eglQuerySurface(m_display,m_surface,EGL_HEIGHT,&vh)!=EGL_TRUE||vh<=0)vh=m_screenHeight;
    glViewport(0,0,vw,vh);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.f,0.f,0.f,0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    // ImGui backend akan mengatur blend saat draw; buffer kosong tetap alpha 0.
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
    EGLBoolean ok=swapBuffersFn(m_display,m_surface);

    if(oldDisplay!=EGL_NO_DISPLAY&&oldContext!=EGL_NO_CONTEXT)
        eglMakeCurrent(oldDisplay,oldDraw,oldRead,oldContext);
    else
        eglMakeCurrent(m_display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
    return ok==EGL_TRUE;
}

void HideRecAImGui::Shutdown(){UnInitEnvironment();}
void HideRecAImGui::UnInitEnvironment(){
    m_state=false;
    if(m_display!=EGL_NO_DISPLAY&&m_surface!=EGL_NO_SURFACE){
        if(eglGetCurrentSurface(EGL_DRAW)==m_surface)
            eglMakeCurrent(m_display,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
        eglDestroySurface(m_display,m_surface);
    }
    if(m_display!=EGL_NO_DISPLAY&&m_overlayContext!=EGL_NO_CONTEXT)
        eglDestroyContext(m_display,m_overlayContext);
    m_surface=EGL_NO_SURFACE;
    m_overlayContext=EGL_NO_CONTEXT;
    // Game-owned m_gameContext: DO NOT call eglDestroyContext / eglTerminate.
    if(m_nativeWindow){ANwCreator::Destroy(m_options.activity,m_nativeWindow);m_nativeWindow=nullptr;}
    m_display=EGL_NO_DISPLAY;
    m_gameContext=EGL_NO_CONTEXT;
    m_screenWidth=m_screenHeight=-1;
}
}

#include <jni.h>
#include <android/native_activity.h>
#include <android/log.h>
#include <cstring>



static ANativeActivity g_nativeActivity{};
static bool g_initialized = false;

// JNI_OnLoad removed — JavaVM is set via ZEL_SetVM() from main.cpp
static JavaVM* g_vm = nullptr;
extern "C" void SetANativeActivityVM(JavaVM* vm)
{
    g_vm = vm;
}

static jobject GetCurrentActivity(JNIEnv* env)
{
    jclass activityThreadCls =
        env->FindClass("android/app/ActivityThread");
    if (!activityThreadCls)
        return nullptr;

    jmethodID currentActivityThread =
        env->GetStaticMethodID(
            activityThreadCls,
            "currentActivityThread",
            "()Landroid/app/ActivityThread;");
    if (!currentActivityThread)
        return nullptr;

    jobject activityThread =
        env->CallStaticObjectMethod(
            activityThreadCls,
            currentActivityThread);
    if (!activityThread)
        return nullptr;

    jfieldID mActivitiesField =
        env->GetFieldID(
            activityThreadCls,
            "mActivities",
            "Landroid/util/ArrayMap;");
    if (!mActivitiesField)
        return nullptr;

    jobject mActivities =
        env->GetObjectField(activityThread, mActivitiesField);
    if (!mActivities)
        return nullptr;

    jclass arrayMapCls = env->GetObjectClass(mActivities);

    jmethodID valuesMethod =
        env->GetMethodID(
            arrayMapCls,
            "values",
            "()Ljava/util/Collection;");
    if (!valuesMethod)
        return nullptr;

    jobject values =
        env->CallObjectMethod(mActivities, valuesMethod);
    if (!values)
        return nullptr;

    jclass collectionCls = env->GetObjectClass(values);

    jmethodID iteratorMethod =
        env->GetMethodID(
            collectionCls,
            "iterator",
            "()Ljava/util/Iterator;");
    if (!iteratorMethod)
        return nullptr;

    jobject iterator =
        env->CallObjectMethod(values, iteratorMethod);
    if (!iterator)
        return nullptr;

    jclass iteratorCls = env->GetObjectClass(iterator);

    jmethodID hasNext =
        env->GetMethodID(iteratorCls, "hasNext", "()Z");
    jmethodID next =
        env->GetMethodID(iteratorCls, "next", "()Ljava/lang/Object;");

    while (env->CallBooleanMethod(iterator, hasNext))
    {
        jobject record =
            env->CallObjectMethod(iterator, next);
        if (!record)
            continue;

        jclass recordCls = env->GetObjectClass(record);

        jfieldID activityField =
            env->GetFieldID(
                recordCls,
                "activity",
                "Landroid/app/Activity;");
        if (!activityField)
            continue;

        jobject activity =
            env->GetObjectField(record, activityField);
        if (activity)
            return activity;
    }

    return nullptr;
}

extern "C"
ANativeActivity* GetANativeActivity()
{
    if (g_initialized)
        return &g_nativeActivity;

    if (!g_vm)
        return nullptr;

    JNIEnv* env = nullptr;
    if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK)
        return nullptr;

    jobject activity = GetCurrentActivity(env);
    if (!activity)
        return nullptr;

    memset(&g_nativeActivity, 0, sizeof(g_nativeActivity));
    g_nativeActivity.vm    = g_vm;
    g_nativeActivity.env   = env;
    g_nativeActivity.clazz = env->NewGlobalRef(activity);

    g_initialized = true;

    
    return &g_nativeActivity;
}

#include <jni.h>
#include <android/native_activity.h>
#include <EGL/egl.h>

extern "C" ANativeActivity* GetANativeActivity();
extern "C" void SetANativeActivityVM(JavaVM* vm);

static android::HideRecAImGui* g_zel_imgui=nullptr;
static JavaVM* g_zel_vm=nullptr;

inline void ZEL_SetVM(JavaVM* vm){g_zel_vm=vm;SetANativeActivityVM(vm);}

inline bool ZEL_Render(ImDrawData* drawData,EGLDisplay dpy,EGLSurface surface,EGLContext context,
                       android::HideRecAImGui::SwapBuffersFn swapFn){
    if(!g_zel_imgui){
        ANativeActivity* activity=GetANativeActivity();
        if(!activity)return false;
        android::HideRecAImGui::Options opts;
        opts.activity=activity;
        opts.skipScreenshot=true;
        g_zel_imgui=new android::HideRecAImGui(opts);
    }
    return g_zel_imgui->RenderDrawData(drawData,dpy,surface,context,swapFn);
}

inline void ZEL_Shutdown(){
    if(g_zel_imgui){delete g_zel_imgui;g_zel_imgui=nullptr;}
}
inline bool ZEL_IsReady(){return g_zel_imgui&&(bool)*g_zel_imgui;}

//================== END HIDE RECORD - SINGLE FILE ==================

  
  
  
  
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "ZEUS-ANTIHEAT", __VA_ARGS__)





static std::atomic<bool> antiOverheatRunning(false);
const int TARGET_FPS = 45;
const int CPU_SLEEP_MS = 5;
const float GPU_RENDER_DELAY = 0.85f;

void StartAntiOverheat() {
    if (antiOverheatRunning) return;
    antiOverheatRunning = true;
    std::thread([]() {
        LOGD("Anti-overheat system started.");
        auto frameTime = std::chrono::milliseconds(1000 / TARGET_FPS);
        while (antiOverheatRunning) {
            std::this_thread::sleep_for(frameTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(CPU_SLEEP_MS));
        }
        LOGD("Anti-overheat system stopped.");
    }).detach();
}

void StopAntiOverheat() {
    antiOverheatRunning = false;
}

void MenuInit() {
    StartAntiOverheat();
}

std::map<std::string, bool> checkboxStates;

bool AIMBUTTON;
bool BTBUTTON;
bool active = false;
bool down;
bool Bypass;
bool INFO;
//==============
static ImVec2 lastWindowPos;
static bool g_Initialized = false;
static bool isLogin = false;
static char s[64];
static std::string storedKey = "";
static bool showFloatingIcon = true;
//============
float AVIWA = 119.167f;
float FOVT;
float AimSmooth = 1.0f;
float FOVTB;
//=========
android_app *g_App = 0;

#include <fstream>
using namespace std;

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

struct sRegion
{
    uintptr_t start, end;

};

float menu[4] = {0.78f, 0.12f, 1.0f, 1.0f};

JavaVM *VM;
std::string md5(std::string s);
using json = nlohmann::json;

uintptr_t address = 0;

#define CREATE_COLOR(r, g, b, a) new float[4]{(float)r, (float)g, (float)b, (float)a};

static bool show = false;
static bool isMenuVisible = false;
static bool HIDEESP = false;

static float speed_min = 0.5f;
    static float speed_max = 3.0f;
#include "Hacks/Feature/SkinHaxSystem.h"

#include "UI/ApplicationUI.h"

// Purple Phantom UI skin: visual styling only. This intentionally does not
// change feature callbacks, state keys, rendering hooks, or feature logic.
static void ApplyPurplePhantomTheme()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 14.0f;
    style.ChildRounding = 11.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 9.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 8.0f;
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 18.0f;
    style.WindowPadding = ImVec2(16.0f, 14.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(10.0f, 9.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);

    ImVec4* c = style.Colors;
    c[ImGuiCol_Text]                 = ImVec4(0.94f, 0.91f, 1.00f, 1.00f);
    c[ImGuiCol_TextDisabled]         = ImVec4(0.57f, 0.53f, 0.68f, 1.00f);
    c[ImGuiCol_WindowBg]             = ImVec4(0.045f, 0.025f, 0.085f, 0.97f);
    c[ImGuiCol_ChildBg]              = ImVec4(0.075f, 0.035f, 0.125f, 0.92f);
    c[ImGuiCol_PopupBg]              = ImVec4(0.060f, 0.025f, 0.105f, 0.98f);
    c[ImGuiCol_Border]               = ImVec4(0.42f, 0.18f, 0.78f, 0.62f);
    c[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    c[ImGuiCol_FrameBg]              = ImVec4(0.13f, 0.065f, 0.22f, 0.96f);
    c[ImGuiCol_FrameBgHovered]       = ImVec4(0.29f, 0.10f, 0.45f, 1.00f);
    c[ImGuiCol_FrameBgActive]        = ImVec4(0.38f, 0.12f, 0.58f, 1.00f);
    c[ImGuiCol_TitleBg]              = ImVec4(0.075f, 0.025f, 0.13f, 1.00f);
    c[ImGuiCol_TitleBgActive]        = ImVec4(0.17f, 0.045f, 0.27f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.055f, 0.025f, 0.10f, 0.90f);
    c[ImGuiCol_MenuBarBg]            = ImVec4(0.085f, 0.035f, 0.14f, 1.00f);
    c[ImGuiCol_ScrollbarBg]          = ImVec4(0.035f, 0.02f, 0.065f, 0.70f);
    c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.35f, 0.12f, 0.55f, 0.90f);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.20f, 0.82f, 1.00f);
    c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.72f, 0.30f, 1.00f, 1.00f);
    c[ImGuiCol_CheckMark]            = ImVec4(0.98f, 0.48f, 1.00f, 1.00f);
    c[ImGuiCol_SliderGrab]           = ImVec4(0.66f, 0.25f, 0.95f, 1.00f);
    c[ImGuiCol_SliderGrabActive]     = ImVec4(0.88f, 0.38f, 1.00f, 1.00f);
    c[ImGuiCol_Button]               = ImVec4(0.28f, 0.075f, 0.43f, 0.96f);
    c[ImGuiCol_ButtonHovered]        = ImVec4(0.48f, 0.12f, 0.68f, 1.00f);
    c[ImGuiCol_ButtonActive]         = ImVec4(0.63f, 0.18f, 0.82f, 1.00f);
    c[ImGuiCol_Header]               = ImVec4(0.27f, 0.075f, 0.42f, 0.90f);
    c[ImGuiCol_HeaderHovered]        = ImVec4(0.43f, 0.12f, 0.63f, 1.00f);
    c[ImGuiCol_HeaderActive]         = ImVec4(0.56f, 0.16f, 0.76f, 1.00f);
    c[ImGuiCol_Separator]            = ImVec4(0.37f, 0.15f, 0.57f, 0.72f);
    c[ImGuiCol_SeparatorHovered]     = ImVec4(0.68f, 0.25f, 0.92f, 0.90f);
    c[ImGuiCol_SeparatorActive]      = ImVec4(0.83f, 0.35f, 1.00f, 1.00f);
    c[ImGuiCol_ResizeGrip]           = ImVec4(0.55f, 0.18f, 0.80f, 0.35f);
    c[ImGuiCol_ResizeGripHovered]    = ImVec4(0.70f, 0.25f, 0.95f, 0.75f);
    c[ImGuiCol_ResizeGripActive]     = ImVec4(0.85f, 0.35f, 1.00f, 1.00f);
}

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
  if (!Config.ImGuiMenu.g_Initialized) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplAndroid_Init();
    ImGui_ImplOpenGL3_Init("#version 300 es");

    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = NULL;

    StudioUI::Initialize(io);
    ApplyPurplePhantomTheme();

    memset(&Config, 0, sizeof(sConfig));
    Config.ExtraMenu.StretchAspect = 2.0f;

    Config.sColorsESPPLAYER.LinePLAYER = CREATE_COLOR(255, 0, 0, 255);
    Config.sColorsESPPLAYER.BoxPLAYER = CREATE_COLOR(255, 0, 0, 255);
    Config.sColorsESPPLAYER.NamePLAYER = CREATE_COLOR(255, 0, 0, 255);
    Config.sColorsESPPLAYER.DistancePLAYER = CREATE_COLOR(255, 0, 0, 255);
    Config.sColorsESPPLAYER.HealthPLAYER = CREATE_COLOR(255, 0, 0, 255);

    Config.sColorsESPBOT.LineBOT = CREATE_COLOR(0, 255, 0, 180);
    Config.sColorsESPBOT.BoxBOT = CREATE_COLOR(0, 255, 0, 180);
    Config.sColorsESPBOT.NameBOT = CREATE_COLOR(0, 255, 0, 180);
    Config.sColorsESPBOT.HealthBOT = CREATE_COLOR(0, 255, 0, 180);
    Config.sColorsESPBOT.DistanceBOT = CREATE_COLOR(0, 255, 0, 180);

    Config.sColorsESPOTHERS.PovOTHERS = CREATE_COLOR(225, 255, 255, 255);

    Config.ImGuiMenu.g_Initialized = true;
  }


// 1. Setup IO and Screen Dimensions
ImGuiIO* io = &ImGui::GetIO();
float screenWidth = static_cast<float>(get_width());
float screenHeight = static_cast<float>(get_height());
io->DisplaySize = ImVec2(screenWidth, screenHeight);

// 2. Process Touch Inputs
auto Input_get_touchCount = (int (*)())(Class_Input__get_touchCount);
if (Input_get_touchCount && Class_Input__GetTouch && Class_Input__get_mousePosition && Input_get_touchCount() > 0) {
    auto Input_GetTouch = (Touch(*)(uintptr_t, int))(Class_Input__GetTouch);
    auto Input_get_mousePosition = (Vector3(*)(uintptr_t))(Class_Input__get_mousePosition);

    Touch t0 = Input_GetTouch(Config.ImGuiMenu.thiz, 0);
    Vector3 mousePos = Input_get_mousePosition(Config.ImGuiMenu.thiz);
    ImVec2 mappedPos(mousePos.x, screenHeight - mousePos.y);

    switch (t0.m_Phase) {
        case TouchPhase::Began:
        case TouchPhase::Stationary:
            io->MouseDown[0] = true;
            io->MousePos = mappedPos;
            break;
        case TouchPhase::Moved:
            io->MousePos = mappedPos;
            break;
        case TouchPhase::Ended:
        case TouchPhase::Canceled:
            io->MouseDown[0] = false;
            Config.ImGuiMenu.clearMousePos = true;
            break;
        default:
            break;
    }
} else if (Input_get_touchCount) {
    io->MouseDown[0] = false;
}

// 3. New Frame Start
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplAndroid_NewFrame(static_cast<int>(screenWidth), static_cast<int>(screenHeight));
ImGui::NewFrame();

// 4. Drawing & Features Pipeline
StretchScreen::Apply(screenWidth, screenHeight);

ImDrawList* draw = ImGui::GetBackgroundDrawList();
DrawESP(draw, screenWidth, screenHeight, get_density());

AimBot_Func();

// Hayaan ang StudioUI::Draw() na humawak ng 'show' state sa sarili nito!
StudioUI::Draw(); //[span_7](start_span)[span_7](end_span)[span_8](start_span)[span_8](end_span)


// 5. Final Render Call
ImGui::Render();
      if (HIDEESP && old_eglSwapBuffers != nullptr) {
        EGLDisplay eglDpy = (EGLDisplay)dpy;
        EGLSurface eglSurface = (EGLSurface)surface;
        EGLContext gameCtx = eglGetCurrentContext();
        ZEL_Render(ImGui::GetDrawData(), eglDpy, eglSurface, gameCtx,
                   (android::HideRecAImGui::SwapBuffersFn)old_eglSwapBuffers);
    } else {
        ZEL_Shutdown();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
  return old_eglSwapBuffers(dpy, surface);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Init_Thread1()
{
	while (!m_Anogs) 
    {
        m_Anogs = Tools::GetBaseAddress("libanogs.so");
    
        sleep(2);
    }
//bypass here
/* ===== ᴛᴏᴏʟs ===== */

MemoryPatch::createWithHex("libanogs.so", 0x1F8408, "00 00 80 D2 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x1F87B0, "00 00 80 D2 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x204100, "00 00 80 D2 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x41BA40, "00 00 80 D2 C0 03 5F D6").Modify();

}

void Init_Thread()
{
    while (!m_unity)
    {
        m_unity = Tools::GetBaseAddress("libunity.so");

        sleep(1);
    }

    // SkinHax kit: arm one-shot config collectors as soon as libunity is ready.
    // Install the full Mythic attachment runtime path early as well so live-match
    // Weapon.InitPart / AttachItem creation cannot happen before these hooks exist.
    OblivionSkinHax::InstallCaptureOnly();
    InstallMythicAttachmentRuntimeFresh(m_unity);


	//EXTREME QUALITY
	patchOffset("libunity.so", 0x8835A60, "00 00 80 D2 C0 03 5F D6", checkboxStates["extreme"]);
	patchOffset("libunity.so", 0x9FE5254, "20 00 80 D2 C0 03 5F D6", checkboxStates["extreme"]);
	
	//ANTI FPS DROP
	patchOffset("libunity.so", 0x9FF1E64,"20 00 80 D2 C0 03 5F D6", checkboxStates["anti"]);
	patchOffset("libunity.so", 0x8847A40,"20 00 80 D2 C0 03 5F D6", checkboxStates["anti"]);
	
	patchOffset("libunity.so", 0x91C3010, "1F 20 03 D5 E0 03 13 AA", checkboxStates["wallhack"]);
	
	//UNLI AMMO
	patchOffset("libunity.so", 0x5984C48, "00 00 80 52 C0 03 5F D6", checkboxStates["AMMO"]);
	
	//BR TAGS
	patchOffset("libunity.so", 0x5CD8228, "20 00 80 D2 C0 03 5F D6", checkboxStates["BRTAGS"]);
	
	//HIGH END PERFORMANCE
	patchOffset("libunity.so", 0x9FDE9C8, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x882FB5C, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FDE9D8, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FDFC58, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FDFDCC, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FDFDD4, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]); 
	patchOffset("libunity.so", 0x9FE18E8, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FDFBBC, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FE3C80, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FE3EC4, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FE6864, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FDFB20, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FE8BE4, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x8839D08, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x883D084, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	patchOffset("libunity.so", 0x9FE8298, "20 00 80 D2 C0 03 5F D6", checkboxStates["highend"]);
	DobbyHook((void*)getAbsoluteAddress("libunity.so", 0x9FE01CC), (void*)GetFrameRateValue, (void**)&orig_GetFrameRateValue);
    DobbyHook((void*)getAbsoluteAddress("libunity.so", 0x9FDFB38), (void*)GetUltraFrameRateFinalFPS, (void**)&orig_GetUltraFrameRateFinalFPS);
    DobbyHook((void*)getAbsoluteAddress("libunity.so", 0x9FDF8E0), (void*)hooked_Fpslevel, (void**)&original_Fpslevel);
    DobbyHook((void*)getAbsoluteAddress("libunity.so", 0x9FDF8E0), (void*)hooked_Fpslevel1, (void**)&original_Fpslevel1);
    DobbyHook((void*)getAbsoluteAddress("libunity.so", 0x9FE3FB4), (void*)hooked_Extremequality, (void**)&original_Extremequality);
    DobbyHook((void*)getAbsoluteAddress("libunity.so", 0x9FDEE28), (void*)hooked_SetUltraFrameRateDeviceInfo, (void**)&orig_SetUltraFrameRateDeviceInfo);
	
	//PATCHES SHITS 
    Patches.autosnipe = MemoryPatch::createWithHex("libunity.so",0x6A965B0,"20 00 80 D2 C0 03 5F D6"); // public bool get_IsSingleShot()
    Patches.kinetic = MemoryPatch::createWithHex("libunity.so",0x51B3D64,"20 00 80 D2 C0 03 5F D6"); // public bool get_IsKineticArmor()
    
    // [Movement]
    Patches.jetj = MemoryPatch::createWithHex("libunity.so",0x907D498,"20 00 80 D2 C0 03 5F D6"); // public bool CanJettingByStartJumpTime()
    Patches.jets = MemoryPatch::createWithHex("libunity.so",0x9086B5C,"20 00 80 D2 C0 03 5F D6");
    Patches.NoSlideFatigue = MemoryPatch::createWithHex("libunity.so", 0x8BF722C, "00 00 80 D2 C0 03 5F D6");
    Patches.NoSlideFatiguePve = MemoryPatch::createWithHex("libunity.so", 0x651274C, "00 00 80 D2 C0 03 5F D6"); // public bool CanBoostSlide()
    
    // [Quality of Life]
    Patches.revive = MemoryPatch::createWithHex("libunity.so",0x9681C20,"00 00 80 D2 C0 03 5F D6"); // public bool get_reviveFromDeath()
    Patches.revive = MemoryPatch::createWithHex("libunity.so",0x9687BEC,"20 00 80 D2 C0 03 5F D6"); // public float get_ReviveTime()
    Patches.chute = MemoryPatch::createWithHex("libunity.so",0x5DC662C,"00 00 80 D2 C0 03 5F D6"); // public void OpenParachute(bool isAuto)
    Patches.wing = MemoryPatch::createWithHex("libunity.so",0x54A12F4,"00 00 80 D2 C0 03 5F D6"); // public bool IsAutoOpenWingsuit()
    Patches.rpd = MemoryPatch::createWithHex("libunity.so",0xC14B314, "00 2C 40 BC C0 03 5F D6"); // public float get_AddHotTime()
	Patches.red = MemoryPatch::createWithHex("libunity.so",0x9677554,"20 00 80 D2 C0 03 5F D6");//public bool get_IsInEM3Eye()
    Patches.cost = MemoryPatch::createWithHex("libunity.so",0x6AAF660,"00 00 80 D2 C0 03 5F D6"); // public int get_ShotCost()
    Patches.fire = MemoryPatch::createWithHex("libunity.so",0xC148ED0,"00 2C 40 BC C0 03 5F D6"); // public float get_FireBoltTime()
    Patches.fire = MemoryPatch::createWithHex("libunity.so",0xC1478C0,"00 2C 40 BC C0 03 5F D6"); // public virtual float get_FireInterval()
    Patches.outline = MemoryPatch::createWithHex("libunity.so",0x72cb6b4,"20 00 80 D2 C0 03 5F D6");//private bool IsEffectActive()
    Patches.outline = MemoryPatch::createWithHex("libunity.so",0x72ca8b0,"20 00 80 D2 C0 03 5F D6");//private bool IsEffectActive()
    Patches.range = MemoryPatch::createWithHex("libunity.so",0x50EE1F8,"20 00 80 D2 C0 03 5F D6"); // public float get_WeaponRangeHOOKLIBZ
	
	
   // HOOKLIBZ
    HOOK_LIB("libunity.so", "0x5947970", CheckTargetIsValid_DyingInAzurGameMode_Hook, orig_CheckTargetIsValid_DyingInAzurGameMode);
    HOOK_LIB("libunity.so", "0x5947A24", CheckTargetIsValid_PhysState_Hook, orig_CheckTargetIsValid_PhysState);
    HOOK_LIB("libunity.so", "0x5947C18", CheckTargetIsValid_ManualParameter_Hook, orig_CheckTargetIsValid_ManualParameter);
    HOOK_LIB("libunity.so", "0x5947F1C", CheckTargetIsValid_Ult_Hook, orig_CheckTargetIsValid_Ult);
    HOOK_LIB("libunity.so", "0x5221D00", hook_GetMaxJumpHeight, orig_GetMaxJumpHeight);
    HOOK_LIB("libunity.so", "0x5195B34", hooked_Init, original_Init);
    HOOK_LIB("libunity.so", "0x904DD18", NeedDelayProcess, orig_NeedDelayProcess);
    HOOK_LIB("libunity.so", "0x519676C", ProcessForDelay, orig_ProcessForDelay);

    HOOK_LIB("libunity.so", "0x5DE981C", get_AccelerationForwardSpeedUp, orig_get_AccelerationForwardSpeedUp);
    HOOK_LIB("libunity.so", "0x5DE9880", get_MaxVelocityForwardSpeedUp, orig_get_MaxVelocityForwardSpeedUp);
    HOOK_LIB("libunity.so", "0x5985F8C", getAdvanceUAV, orig_AdvanceUAV);
    HOOK_LIB("libunity.so", "0xC9BAFF8", GetScaleRecoil, orig_GetScaleRecoil);
    HOOK_LIB("libunity.so", "0xC1514C0", SingleLineCheckPhysics, orig_SingleLineCheckPhysics);
    HOOK_LIB("libunity.so", "0x50ED8D4", get_EquipTime, orig_get_EquipTime);
    HOOK_LIB("libunity.so", "0x50EB944", get_AimingTime, orig_get_AimingTime);
    HOOK_LIB("libunity.so", "0x666FD90", GetAssitAimSpeed, orig_GetAssitAimSpeed);
    HOOK_LIB("libunity.so", "0xC14943C", get_ChangeClipTime, orig_get_ChangeClipTime);
	HOOK_LIB("libunity.so", "0x649048C", InitMaxPickUpDistance, orig_InitMaxPickUpDistance); 
	HOOK_LIB("libunity.so", "0x8A5B8C8", get_GulagTicketCount, orig_GetGulagTicketCount);
	HOOK_LIB("libunity.so", "0x31877E0", GetPickUpTime, orig_GetPickUpTime); 
	//AUTO ON
    HOOK_LIB_NO_ORIG("libunity.so", "0x91BD4B4", IsTutorialEnabled); //public static bool IsTutorialEnabled()
	HOOK_LIB_NO_ORIG("libunity.so", "0x6BBD97C", IsEnableVulkan); //public bool get_EnableVulkan() { };
	HOOK_LIB_NO_ORIG("libunity.so", "0x4E75F58", IsEmulator); //public bool IsEmulator() { }; //RVA:
	
	HOOK_LIB("libunity.so", "0x901F988", IsUnlocked, orig_IsUnlocked); // IsUnlocked

    // [System]
    HOOK_LIB_NO_ORIG("libunity.so", "0x9DE0E58", IsTutorialEnabled);
    HOOK_LIB_NO_ORIG("libunity.so", "0x9FEC8AC", IsEnableVulkan);
    HOOK_LIB_NO_ORIG("libunity.so", "0xAE24334", IsEmulator);
	
    // [Weapon]
    HOOK_LIB("libunity.so", "0xC9B6F90", WeaponFireComponent_Instant_CreateBulletLine, oWeaponFireComponent_Instant_CreateBulletLine);
    HOOK_LIB("libunity.so", "0xC9C33A4", CreateBulletProjectile, oCreateBulletProjectile);
    
    // [Movement]
    HOOK_LIB("libunity.so", "0x8BF62D4", h_GetSuperSlideRate, o_GetSuperSlideRate);
    HOOK_LIB("libunity.so", "0x9BDBBA8", h_get_SlideTackleAcclerationSpeed, o_get_SlideTackleAcclerationSpeed);
    HOOK_LIB("libunity.so", "0x51D3AF4", h_PawnGetMaxSpeed, o_PawnGetMaxSpeed);
    HOOK_LIB("libunity.so", "0x8BF7E48", h_TickLocalPlayer, o_TickLocalPlayer);
    HOOK_LIB("libunity.so", "0x8BF71F0", h_get_SlideTackleSpeed, o_get_SlideTackleSpeed);
    HOOK_LIB("libunity.so", "0xB15D104", SameTeamFromPawn, orig_SameTeamFromPawn);
    HOOK_LIB("libunity.so", "0xC9B9618", GetRealSpreadModifier, orig_GetRealSpreadModifier);
    HOOK_LIB("libunity.so", "0x522860C", hooked_get_m_PhysSkisMaxSpeed, get_m_PhysSkisMaxSpeed);
    HOOK_LIB("libunity.so", "0x51D2EB8", hooked_CalcFinalMoveScale, original_CalcFinalMoveScale);
	
    DobbyHook((void*) getAbsoluteAddress("libunity.so", 0x9FDEE28), (void*)&hooked_SetUltraFrameRateDeviceInfo, (void**)&orig_SetUltraFrameRateDeviceInfo);
    
	
    auto swapBuffers = ((uintptr_t) DobbySymbolResolver(OBFUSCATE("libunity.so"), OBFUSCATE("eglSwapBuffers")));
    KittyMemory::ProtectAddr((void *)swapBuffers, sizeof(swapBuffers), PROT_READ | PROT_WRITE | PROT_EXEC);
    DobbyHook((void *) swapBuffers, (void *) hook_eglSwapBuffers, (void **) &old_eglSwapBuffers);
    
}

__attribute__((constructor))
void native_Init(JNIEnv *env, jclass clazz, jobject mContext) {
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    VM = vm;
            ZEL_SetVM(vm);
    
    std::thread(Init_Thread).detach();
    std::thread(Init_Thread1).detach();
    
    return JNI_VERSION_1_6;
}



