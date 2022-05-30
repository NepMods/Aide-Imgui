#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"

uintptr_t address = 0;
int  glWidth, glHeight;
void SetupImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)glWidth, (float)glHeight);
    ImGui::StyleColorsDark();
    LOGD("opengl problem?");
    ImGui_ImplOpenGL3_Init("#version 100");
    LOGD("no problem?");
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&font_cfg);
    ImGui::GetStyle().ScaleAllSizes(3.0f);
}
bool setup = false;
bool HandleInputEvent(JNIEnv *env, int motionEvent, int x, int y, int p);
typedef enum { TOUCH_ACTION_DOWN = 0, TOUCH_ACTION_UP, TOUCH_ACTION_MOVE } TOUCH_ACTION;
typedef struct {
    TOUCH_ACTION action;
    float x;
    float y;
    int pointers;
    float y_velocity;
    float x_velocity;
}TOUCH_EVENT;
 TOUCH_EVENT g_LastTouchEvent;
bool  HandleInputEvent(JNIEnv *env, int motionEvent, int x, int y, int p) {
    float velocity_y = (float)((float)y - g_LastTouchEvent.y) / 100.f;
    g_LastTouchEvent = {.action = (TOUCH_ACTION) motionEvent, .x = static_cast<float>(x), .y = static_cast<float>(y), .pointers = p, .y_velocity = velocity_y};
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos.x = g_LastTouchEvent.x;
    io.MousePos.y = g_LastTouchEvent.y;
    if(motionEvent == 2){
        if (g_LastTouchEvent.pointers > 1) {
            io.MouseWheel = g_LastTouchEvent.y_velocity;
            io.MouseDown[0] = false;
        }
        else {
            io.MouseWheel = 0;
        }
    }
    if(motionEvent == 0){
        io.MouseDown[0] = true;
    }
    if(motionEvent == 1){
        io.MouseDown[0] = false;
    }
    return true;
}
bool (*old_nativeInjectEvent )(JNIEnv*, jobject ,jobject event);
bool hook_nativeInjectEvent(JNIEnv* env, jobject instance,jobject event){
        jclass MotionEvent = env->FindClass(("android/view/MotionEvent"));
        if(!MotionEvent){
            LOGI("Can't find MotionEvent!"); 
        }
        
        if(!env->IsInstanceOf(event, MotionEvent)){
            return old_nativeInjectEvent(env, instance, event);
        }
        LOGD("Processing Touch Event!");
        jmethodID id_getAct = env->GetMethodID(MotionEvent, ("getActionMasked"), ("()I"));
        jmethodID id_getX = env->GetMethodID(MotionEvent, ("getX"), ("()F"));
        jmethodID id_getY = env->GetMethodID(MotionEvent, ("getY"), ("()F"));
        jmethodID id_getPs = env->GetMethodID(MotionEvent, ("getPointerCount"), ("()I"));
        HandleInputEvent(env, env->CallIntMethod(event, id_getAct),env->CallFloatMethod(event, id_getX), env->CallFloatMethod(event, id_getY), env->CallIntMethod(event, id_getPs));
        if (!ImGui::GetIO().MouseDownOwnedUnlessPopupClose[0]){
            return old_nativeInjectEvent(env, instance, event);
        }
        return false;
}
jint (*old_RegisterNatives )(JNIEnv*, jclass, JNINativeMethod*,jint);
jint hook_RegisterNatives(JNIEnv* env, jclass destinationClass, JNINativeMethod* methods,
                          jint totalMethodCount){

    int currentNativeMethodNumeration;
    for (currentNativeMethodNumeration = 0; currentNativeMethodNumeration < totalMethodCount; ++currentNativeMethodNumeration )
    {
        if (!strcmp(methods[currentNativeMethodNumeration].name, ("nativeInjectEvent")) ){
            DobbyHook(methods[currentNativeMethodNumeration].fnPtr, (void*)hook_nativeInjectEvent, (void **)&old_nativeInjectEvent);
        }
    }
    return old_RegisterNatives(env, destinationClass, methods, totalMethodCount);
}
