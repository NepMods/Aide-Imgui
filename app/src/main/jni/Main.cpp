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
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "includes/Dobby/dobby.h"
#define targetLibName OBFUSCATE("libil2cpp.so")
#include "Includes/Macros.h"
#include "Includes/Loader.h"


struct Patches {
    MemoryPatch MemoryExample;
} hexPatches;

struct Variables {
    bool BooleanExample = false;
} FT;

/*

bool Orig_Example(void *player);
bool Hook_Example(void *player) {
    if (player != NULL) {
        if(FT.BooleanExample) {
            return false;
        }
    }
    return Orig_Example(player);
}


*/

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);
   
	if (!setup) {
        SetupImgui();
        setup = true;
    }

    ImGuiIO &io = ImGui::GetIO();
	switch (g_LastTouchEvent.action) {
        case TOUCH_ACTION_MOVE:
            if (g_LastTouchEvent.pointers > 1) {
                io.MouseWheel = g_LastTouchEvent.y_velocity;
                io.MouseDown[0] = false;
            }
            else {
                io.MouseWheel = 0;
            }
            break;
        case TOUCH_ACTION_DOWN:
            io.MouseDown[0] = true;
            break;
        case TOUCH_ACTION_UP:
            io.MouseDown[0] = false;
            //g_KeyEventQueues[event_key_code].push(event_action);
            break;
        default:
            break;
    }
	
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Render ImGui windows here.
    //Show Your Window Here
    ImGui::ShowDemoWindow();

    // Rendering
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    return old_eglSwapBuffers(dpy, surface);
}



void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    sleep(5);

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    address = findLibrary(targetLibName);

    // Hook eglSwapBuffers
    auto addr = (uintptr_t)dlsym(RTLD_NEXT, "eglSwapBuffers");
    LOGD("eglSwapBuffers address: 0x%X", addr);
    DobbyHook((void *)addr, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
 
    /*
    
    
           
            hexPatches.MemoryExample = MemoryPatch:createWithHex(targetLibName,
                                                                 string2Offset(OBFUSCATE("0x000000")),
                                                                 OBFUACATE("00 00 00 00 00 00 00 00")));
        
            HOOK_LIB(targetLibName, "0x000000", Hook_Example, Orig_Example);
    
    
    */
    
    
    pthread_exit(nullptr);
    return nullptr;


}
void StartBackend(JNIEnv* env){
      //Input  
    DobbyHook((void*)env->functions->RegisterNatives, (void*)hook_RegisterNatives, (void **)&old_RegisterNatives);

}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *globalEnv;
    vm->GetEnv((void **) &globalEnv, JNI_VERSION_1_6);

	StartBackend(globalEnv);
	
	pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
    return JNI_VERSION_1_6;
}


