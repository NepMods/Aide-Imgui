#define targetLibName OBFUSCATE("libil2cpp.so")

uintptr_t Touch = 0xE2C5A8;

// pulic class Input

// public static Touch GetTouch(int index) { }

struct Patches {

    MemoryPatch MemoryExample;

} hexPatches;

struct Variables {

    bool BooleanExample = false;

} FT;

void DrawImGui() {

   

    

    ImGui::NewFrame();

    // Render ImGui windows here.

    ImGui::ShowDemoWindow();

    

    ImGui::EndFrame();

    ImGui::Render();

}

void *hack_thread(void *) {

    LOGI(OBFUSCATE("pthread created"));

    sleep(5);

    do {

        sleep(1);

    } while (!isLibraryLoaded(targetLibName));

    

}
uintptr_t Touch = 0xE2C5A8;
// pulic class Input
// public static Touch GetTouch(int index) { }

struct Patches {
    MemoryPatch MemoryExample;
} hexPatches;


struct Variables {
    bool BooleanExample = false;
} FT;

void DrawImGui() {
   
    
    ImGui::NewFrame();

    // Render ImGui windows here.
    ImGui::ShowDemoWindow();
    
    ImGui::EndFrame();
    ImGui::Render();
}


void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    sleep(5);

    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    

}
