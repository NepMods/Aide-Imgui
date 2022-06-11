#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "includes/Dobby/dobby.h"

#include "Includes/Macros.h"
#include "Color.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "ByNameModding/BNM.hpp"
#include "Main.cpp"
using namespace BNM;

auto getClass(const char *C) {
   auto clasz = LoadClass("", C);
   return clasz;
}
auto getClass(const char *N, const char *C) {
   auto clasz = LoadClass(N, C);
   return clasz;
}

uintptr_t getOffset(LoadClass clasz, const char *M, int param = 0) {
    uintptr_t offset = clasz.GetMethodOffsetByName(M, param);
    return offset;
}

uintptr_t getOffset(LoadClass clasz, const char *M, std::vector<string>params_names) {
    uintptr_t offset = clasz.GetMethodOffsetByName(M, params_names);
    return offset;
}
#define AddField(offset,classs,field) offset = classs.GetFieldOffset(field);
#define H(offset, ptr, orig) MSHookFunction((void *)offset, (void *) ptr, (void **) &orig);
#define Add(pointer, offset) InitFunc(pointer, offset);
#define PB(offset, hex, IsOn) if(IsOn) {MemoryPatch("libil2cpp.so", offset, hex, 4).Modify();} else {MemoryPatch("libil2cpp.so", offset, "", 4).Modify();};
#define PBL(lib, offset, hex, IsOn) if(IsOn) {MemoryPatch(lib, offset, hex, 4).Modify();} else {MemoryPatch(lib, offset, "", 4).Modify();};
#define PO(offset, hex) MemoryPatch("libil2cpp.so", offset, hex, 4).Modify();
#define PL(lib,offset, hex) MemoryPatch(lib, offset, hex, 4).Modify();

uintptr_t address = 0;

void SetupImgui() {
  
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

  
    int (*Screen$$get_height)();
    int (*Screen$$get_width)();
    InitResolveFunc(Screen$$get_height, OBFUSCATE_BNM("UnityEngine.Screen::get_height")); // #define InitResolveFunc(x, y)
    InitResolveFunc(Screen$$get_width, OBFUSCATE_BNM("UnityEngine.Screen::get_width"));
    LOGD("Display size: %fx%f", (float)Screen$$get_width(), (float)Screen$$get_height());
    io.DisplaySize = ImVec2((float)Screen$$get_width(), (float)Screen$$get_height());

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    LOGD("opengl problem?");
    ImGui_ImplOpenGL3_Init("#version 100");
    LOGD("no problem?");

    // We load the default font with increased size to improve readability on many devices with "high" DPI.
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&font_cfg);

    // Arbitrary scale-up
    ImGui::GetStyle().ScaleAllSizes(3.0f);
}

bool clearMousePos = true, setup = false;

struct UnityEngine_Vector2_Fields {
    float x;
    float y;
};

struct UnityEngine_Vector2_o {
    UnityEngine_Vector2_Fields fields;
};

enum TouchPhase {
    Began = 0,
    Moved = 1,
    Stationary = 2,
    Ended = 3,
    Canceled = 4
};
struct UnityEngine_Touch_Fields {
    int32_t m_FingerId;
    struct UnityEngine_Vector2_o m_Position;
    struct UnityEngine_Vector2_o m_RawPosition;
    struct UnityEngine_Vector2_o m_PositionDelta;
    float m_TimeDelta;
    int32_t m_TapCount;
    int32_t m_Phase;
    int32_t m_Type;
    float m_Pressure;
    float m_maximumPossiblePressure;
    float m_Radius;
    float m_RadiusVariance;
    float m_AltitudeAngle;
    float m_AzimuthAngle;
};
void StartDrawImGui();
EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {

    if (!setup) {
        SetupImgui();
        setup = true;
    }

    ImGuiIO &io = ImGui::GetIO();
    int (*Input$$touchCount)();
    InitResolveFunc(Input$$touchCount, OBFUSCATE_BNM("UnityEngine.Input::get_touchCount"));
    int touchCount = Input$$touchCount();
    if (touchCount > 0) {
       UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields (*)(int))(address+Touch))(0); // public static Touch GetTouch(int index) { }
        float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;
        switch (touch.m_Phase) {
            case TouchPhase::Began:
            case TouchPhase::Stationary:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                io.MouseDown[0] = true;
                break;
            case TouchPhase::Ended:
            case TouchPhase::Canceled:
                io.MouseDown[0] = false;
                clearMousePos = true;
                break;
            case TouchPhase::Moved:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                break;
            default:
                break;
        }
    }
    ImGui_ImplOpenGL3_NewFrame();
  DrawImGui();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
if (clearMousePos) {
        io.MousePos = ImVec2(-1, -1);
        clearMousePos = false;
    }

    return old_eglSwapBuffers(dpy, surface);
}
void *ImGui_Thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    sleep(5);

    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    address = findLibrary(targetLibName);
    auto addr = (uintptr_t)dlsym(RTLD_NEXT, "eglSwapBuffers");
    LOGD("eglSwapBuffers address: 0x%X", addr);
    DobbyHook((void *)addr, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
    pthread_exit(nullptr);
    return nullptr;


}
__attribute__((constructor))
void lib_main() {
    pthread_t ptid;
    pthread_create(&ptid, NULL, ImGui_Thread, NULL);
    pthread_t HacksTh;
    pthread_create(&HacksTh, NULL, hack_thread, NULL);
}
