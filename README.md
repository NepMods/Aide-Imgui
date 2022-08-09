Unity Imgui 
<br>

**Change log:**

```
-> Fixed for Aide
-> Added ByNameModding library
-> Touch Fixed [ Need One Offset ]
-> Made Code Cleaner 
-> Made Easy 
-> I Wont Work On This Anymore
-> Added Univarsal Touch
```
**Still Not For Newbies**

**Invoke Code**
```

    const-string v0, "nepGUI"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
```

**Start A Window**
```
   ImGui::Begin("Window Name", bool_isActive, ImGuiWindowFlags);
   ImGui::End();
   
   Example:
   
   ImGui::Begin("Test Windoe, nullptr);
   ImGui::End();
   
```

**Add A Widget**

```
   bool GodMod = false;
   ImGui::Checkbox("My Checkbox", &GodMod);
   
   if(ImGui::Button("My Button")) {
       GodMod = !GodMode;
   }
```

**More**
<br>

For More, Check the <a href="https://github.com/NepMods/Aide-Imgui/blob/master/app/src/main/jni/ImGui/imgui_demo.cpp">ImGui_Demo</a>
<br><br>https://github.com/NepMods/Aide-Imgui

join For Help:
https://t.me/CheaterNeverDies
<br>
https://t.me/YourAnnonOne
