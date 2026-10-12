#include <android/log.h>
#include <unistd.h>
#include <thread>
#include <limits>
#include <xdl.h>
#include <KittyUtils.h>
#include <KittyMemory.h>
#include <Il2Cpp.h>
#include <SubstrateHook.h>
#include <CydiaSubstrate.h>
#include "Struct/Gui.hpp"
#include <Struct/main.h>
#include "imgui/fonts/FontAwesome6_solid.h"
#include "imgui/Toggle.h"
#include "zygisk.hpp"
#include "Struct/RamaModzTeam.h"
    // CREADIT: BY RAMA MODZ
    // CREADIT: BY RAMA MODZ
    // CREADIT: BY RAMA MODZ
    // CREADIT: BY RAMA MODZ
    // FUCK YOU DELETE CREDIT 
    // FUCK YOU FAKE DEVELOPER
    // FUCK YOU DELETE CREDIT 
    // FUCK YOU FAKE DEVELOPER
using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

void hack();

class MyModule : public zygisk::ModuleBase {
 public:
  void onLoad(Api *api, JNIEnv *env) override {
    this->api_ = api;
    this->env_ = env;
    genv = env;
  }
  void preAppSpecialize(AppSpecializeArgs *args) override {
    static constexpr const char *packages[] = {
      "com.dts.freefireth","com.dts.freefiremax"
    }; // GAME PACKAGE NAMES
    const char *process = env_->GetStringUTFChars(args->nice_name, nullptr);
    for (const auto *package: packages) {
      is_game_ = (strcmp(process, package) == 0);
      if (is_game_) {
        break;
      }
    }
    env_->ReleaseStringUTFChars(args->nice_name, process);
  }
  void postAppSpecialize(const AppSpecializeArgs *args) override {
    if (is_game_) {
       genv->GetJavaVM(&jvm);
      std::thread{hack}.detach();
    }
  }
 private:
  Api *api_ = nullptr;
  JNIEnv *env_ = nullptr;
  bool is_game_ = false;
};


void SetNeonBlueTheme() {
    ImGuiStyle* style = &ImGui::GetStyle();
    style->Alpha = 1.0f;
    style->WindowRounding = 10.0f;
    style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style->FrameRounding = 10.0f;
    style->FramePadding = ImVec2(4, 7);
    style->ScrollbarSize = 25;

    ImVec4 NeonColor = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);

    ImVec4 bgColor   = ImVec4(0.02f, 0.02f, 0.06f, 1.0f);
    ImVec4 hover     = ImVec4(NeonColor.x, NeonColor.y, NeonColor.z, 0.6f);
    ImVec4 active    = ImVec4(NeonColor.x, NeonColor.y, NeonColor.z, 0.8f);
    ImVec4 dimBorder = ImVec4(NeonColor.x * 0.3f, NeonColor.y * 0.3f, NeonColor.z * 0.3f, 1.0f);

    style->Colors[ImGuiCol_Text]                 = ImVec4(0.9f, 0.9f, 1.0f, 1.0f); 
    style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.2f, 0.3f, 0.4f, 1.0f);
    style->Colors[ImGuiCol_WindowBg]             = bgColor;
    style->Colors[ImGuiCol_ChildBg]              = ImVec4(0.04f, 0.04f, 0.10f, 0.9f);
    style->Colors[ImGuiCol_PopupBg]              = ImVec4(0.05f, 0.05f, 0.1f, 0.95f);
    style->Colors[ImGuiCol_Border]               = dimBorder;
    style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);
    style->Colors[ImGuiCol_FrameBg]              = ImVec4(0.1f, 0.2f, 0.3f, 0.6f);
    style->Colors[ImGuiCol_FrameBgHovered]       = hover;
    style->Colors[ImGuiCol_FrameBgActive]        = active;
    style->Colors[ImGuiCol_TitleBg]              = ImVec4(0.02f, 0.05f, 0.10f, 1.0f);
    style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.05f, 0.10f, 0.20f, 1.0f);
    style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.02f, 0.05f, 0.10f, 0.5f);
    style->Colors[ImGuiCol_MenuBarBg]            = bgColor;
    style->Colors[ImGuiCol_ScrollbarBg]          = bgColor;
    style->Colors[ImGuiCol_ScrollbarGrab]        = hover;
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = active;
    style->Colors[ImGuiCol_ScrollbarGrabActive]  = active;
    style->Colors[ImGuiCol_Separator]            = hover;
    style->Colors[ImGuiCol_CheckMark]            = active;
    style->Colors[ImGuiCol_SliderGrab]           = hover;
    style->Colors[ImGuiCol_SliderGrabActive]     = active;
    style->Colors[ImGuiCol_Button]               = ImVec4(0.0f, 0.3f, 0.6f, 0.8f);
    style->Colors[ImGuiCol_ButtonHovered]        = hover;
    style->Colors[ImGuiCol_ButtonActive]         = active;
    style->Colors[ImGuiCol_Header]               = hover;
    style->Colors[ImGuiCol_HeaderHovered]        = hover;
    style->Colors[ImGuiCol_HeaderActive]         = active;
    style->Colors[ImGuiCol_ResizeGrip]           = hover;
    style->Colors[ImGuiCol_ResizeGripHovered]    = active;
    style->Colors[ImGuiCol_ResizeGripActive]     = active;
    style->Colors[ImGuiCol_Tab]                  = ImVec4(0.0f, 0.3f, 0.6f, 0.7f);
    style->Colors[ImGuiCol_TabHovered]           = hover;
    style->Colors[ImGuiCol_TabActive]            = active;
    style->Colors[ImGuiCol_TextSelectedBg]       = ImVec4(NeonColor.x, NeonColor.y, NeonColor.z, 0.3f);
    style->Colors[ImGuiCol_DragDropTarget]       = active;
    style->Colors[ImGuiCol_NavHighlight]         = active;
    style->Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
}




void(*Bypass1)(void* _this);
void _Bypass1(void* _this) {
    return;
}
void(*Bypass2)(void* _this);
void _Bypass2(void* _this) {
    return;
}
void (*BYPAS)() = 0;
void _BYPAS(void* unk) {
    return;
}

void DrawMainUI() {
        
    ImGui::SetNextWindowSize(ImVec2((float) g_GlWidth * 0.42f, (float) g_GlHeight * 0.74f), ImGuiCond_Once);
    if (ImGui::Begin(ICON_FA_TELEGRAM"  RAMA MODZ | ZYGISK SRC OB55", 0, ImGuiWindowFlags_NoBringToFrontOnFocus)) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
        ImGui::Text(ICON_FA_BULLSEYE " SYSTEM: ACTIVE");
        ImGui::PopStyleColor();
        
        ImGui::SameLine(ImGui::GetWindowWidth() - 240);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.5f, 1.0f));
        ImGui::Text(ICON_FA_USER " @RAMA_MODZ");
        ImGui::PopStyleColor();
        
        ImGui::Separator();
        if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_FittingPolicyScroll)) {
            
            if (ImGui::BeginTabItem(ICON_FA_CROSSHAIRS " AIMBOT")) {
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
                ImGui::Text(ICON_FA_BULLSEYE " AIMBOT CONFIGURATION");
                ImGui::PopStyleColor();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Button, Aimbot ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.15f, 0.15f, 0.25f, 0.9f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Aimbot ? ImVec4(0.0f, 0.7f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.35f, 1.0f));
                
                if (ImGui::Button(Aimbot ? (ICON_FA_CHECK_CIRCLE "  AIMBOT ENABLED") : (ICON_FA_TIMES_CIRCLE "  AIMBOT DISABLED"), ImVec2(-1, 40))) {
                    Aimbot = !Aimbot;
                }
                ImGui::PopStyleColor(2);
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Button, AimSilent ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.15f, 0.15f, 0.25f, 0.9f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AimSilent ? ImVec4(0.0f, 0.7f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.35f, 1.0f));
                
                if (ImGui::Button(AimSilent ? (ICON_FA_CHECK_CIRCLE "  AIM SILENT") : (ICON_FA_TIMES_CIRCLE "  AIM SILENT"), ImVec2(-1, 40))) {
                    AimSilent = !AimSilent;
                }
                ImGui::PopStyleColor(2);
                ImGui::Spacing();
                
                ImGui::BeginGroup();
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.1f, 0.5f));
                ImGui::BeginChild("AimSettings", ImVec2(0, 200), true);
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.8f, 1.0f, 1.0f));
                ImGui::Text("AIM DIRECTION:");
                ImGui::PopStyleColor();
                ImGui::Combo("##AimDir", &AimWhen, dir, IM_ARRAYSIZE(dir));
  
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.8f, 1.0f, 1.0f));
                ImGui::Text("FOV RANGE: %.0f", Fov_Aim);
                ImGui::PopStyleColor();
                ImGui::SliderFloat("##FovSlider", &Fov_Aim, 0.0f, 500.0f, "%.f");
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.8f, 1.0f, 1.0f));
                ImGui::Text("MAX DISTANCE: %.0f", Aimdis);
                ImGui::PopStyleColor();
                ImGui::SliderFloat("##DistSlider", &Aimdis, 0.0f, 1000.0f, "%.f");
                
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::EndGroup();
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.5f, 0.6f, 1.0f));
                ImGui::Text(ICON_FA_INFO_CIRCLE " STATUS: %s | FOV: %.0f | RANGE: %.0f", 
                Aimbot ? "ACTIVE" : "INACTIVE", Fov_Aim, Aimdis);
                ImGui::PopStyleColor();
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem(ICON_FA_EYE " ESP")) {
    
               ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
               ImGui::Text(ICON_FA_BINOCULARS " ESP VISUAL SETTINGS");
               ImGui::PopStyleColor();
               ImGui::Separator();
               ImGui::Spacing();
    
               ImGui::PushStyleColor(ImGuiCol_Button, Enable ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.15f, 0.15f, 0.25f, 0.9f));
               ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Enable ? ImVec4(0.0f, 0.7f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.35f, 1.0f));
    
               if (ImGui::Button(Enable ? (ICON_FA_CHECK_CIRCLE " ESP ENABLED") : (ICON_FA_TIMES_CIRCLE " ESP DISABLED"), ImVec2(-1, 40))) {
                   Enable = !Enable;
               }
               ImGui::PopStyleColor(2);
               ImGui::Spacing();
               ImGui::Spacing();
    
               ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.1f, 0.5f));
               float windowHeight = ImGui::GetWindowHeight();
               float espHeight = windowHeight * 0.38f;
                if (espHeight < 150) espHeight = 150;
                if (espHeight > 220) espHeight = 220;
                ImGui::BeginChild("EspOptions", ImVec2(0, espHeight), true);
                float fullWidth = ImGui::GetWindowWidth() - 20;

                ImGui::PushStyleColor(ImGuiCol_Button, Config.ESP.Line ? ImVec4(0.0f, 0.5f, 1.0f, 0.8f) : ImVec4(0.15f, 0.15f, 0.25f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Config.ESP.Line ? ImVec4(0.0f, 0.6f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.35f, 1.0f));
                if (ImGui::Button(Config.ESP.Line ? (ICON_FA_CHECK_CIRCLE " LINE ESP") : (ICON_FA_TIMES_CIRCLE " LINE ESP"), ImVec2(fullWidth, 35))) {
                    Config.ESP.Line = !Config.ESP.Line;
                }
                ImGui::PopStyleColor(2);
                ImGui::Spacing();

                ImGui::PushStyleColor(ImGuiCol_Button, Config.ESP.Box ? ImVec4(0.0f, 0.5f, 1.0f, 0.8f) : ImVec4(0.15f, 0.15f, 0.25f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Config.ESP.Box ? ImVec4(0.0f, 0.6f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.35f, 1.0f));
                if (ImGui::Button(Config.ESP.Box ? (ICON_FA_CHECK_CIRCLE " BOX ESP") : (ICON_FA_TIMES_CIRCLE " BOX ESP"), ImVec2(fullWidth, 35))) {
                    Config.ESP.Box = !Config.ESP.Box;
                }
                ImGui::PopStyleColor(2);
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Button, Config.ESP.Health ? ImVec4(0.0f, 0.5f, 1.0f, 0.8f) : ImVec4(0.15f, 0.15f, 0.25f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Config.ESP.Health ? ImVec4(0.0f, 0.6f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.35f, 1.0f));
                if (ImGui::Button(Config.ESP.Health ? (ICON_FA_CHECK_CIRCLE " HEALTH ESP") : (ICON_FA_TIMES_CIRCLE " HEALTH ESP"), ImVec2(fullWidth, 35))) {
                    Config.ESP.Health = !Config.ESP.Health;
                }
                ImGui::PopStyleColor(2);
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Button, Config.ESP.RamaMods ? ImVec4(0.0f, 0.5f, 1.0f, 0.8f) : ImVec4(0.15f, 0.15f, 0.25f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Config.ESP.RamaMods ? ImVec4(0.0f, 0.6f, 1.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.35f, 1.0f));
                if (ImGui::Button(Config.ESP.RamaMods ? (ICON_FA_CHECK_CIRCLE " INFO ESP") : (ICON_FA_TIMES_CIRCLE " INFO ESP"), ImVec2(fullWidth, 35))) {
                    Config.ESP.RamaMods = !Config.ESP.RamaMods;
                }
                ImGui::PopStyleColor(2);
                
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::Spacing();
    
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.5f, 0.6f, 1.0f));
                ImGui::Text(ICON_FA_INFO_CIRCLE " ACTIVE: %s", Enable ? "YES" : "NO");
                ImGui::PopStyleColor();
                ImGui::EndTabItem();
                }

            if (ImGui::BeginTabItem(ICON_FA_COGS " EXTRA")) {
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
                ImGui::Text(ICON_FA_COGS " EXTRA FEATURES");
                ImGui::PopStyleColor();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.1f, 0.5f));
                ImGui::BeginChild("EXTRA FEATURES", ImVec2(0, 250), true);
                ImGui::Columns(2, "ExtraGrid", false);
                
                ImGui::PushID("JumpHhight");
                ImGui::PushStyleColor(ImGuiCol_Button, JumpHight ? ImVec4(0.0f, 0.5f, 1.0f, 0.7f) : ImVec4(0.15f, 0.15f, 0.25f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, JumpHight ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.2f, 0.2f, 0.35f, 0.9f));
                if (ImGui::Button(JumpHight ? (ICON_FA_CHECK " JUMP HIGHT") : (ICON_FA_TIMES " JUMP HIGHT"), ImVec2(-1, 35))) {
                    JumpHight = !JumpHight;
                }
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                ImGui::NextColumn();

                
                
                ImGui::PushID("SpeedHack");
                ImGui::PushStyleColor(ImGuiCol_Button, SpeedHack1 ? ImVec4(0.0f, 0.5f, 1.0f, 0.7f) : ImVec4(0.15f, 0.15f, 0.25f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SpeedHack1 ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.2f, 0.2f, 0.35f, 0.9f));
                if (ImGui::Button(SpeedHack1 ? (ICON_FA_CHECK " SPEED HACK") : (ICON_FA_TIMES " SPEED HACK"), ImVec2(-1, 35))) {
                    SpeedHack1 = !SpeedHack1;
                }
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                ImGui::NextColumn();
                
                ImGui::PushID("SpeedTimer");
                ImGui::PushStyleColor(ImGuiCol_Button, SpeedHack ? ImVec4(0.0f, 0.5f, 1.0f, 0.7f) : ImVec4(0.15f, 0.15f, 0.25f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SpeedHack ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.2f, 0.2f, 0.35f, 0.9f));
                if (ImGui::Button(SpeedHack ? (ICON_FA_CHECK " SPEED TIMER") : (ICON_FA_TIMES " SPEED TIMER"), ImVec2(-1, 35))) {
                    SpeedHack = !SpeedHack;
                }
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                ImGui::NextColumn();

                ImGui::PushID("CameraHack");
                ImGui::PushStyleColor(ImGuiCol_Button, camxa ? ImVec4(0.0f, 0.5f, 1.0f, 0.7f) : ImVec4(0.15f, 0.15f, 0.25f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, camxa ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.2f, 0.2f, 0.35f, 0.9f));
                if (ImGui::Button(camxa ? (ICON_FA_CHECK " CAMERA HACK") : (ICON_FA_TIMES " CAMERA HACK"), ImVec2(-1, 35))) {
                    camxa = !camxa;
                }
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                ImGui::NextColumn();
                
                ImGui::PushID("ResetGuest");
                ImGui::PushStyleColor(ImGuiCol_Button, Guest ? ImVec4(0.0f, 0.5f, 1.0f, 0.7f) : ImVec4(0.15f, 0.15f, 0.25f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Guest ? ImVec4(0.0f, 0.6f, 1.0f, 0.9f) : ImVec4(0.2f, 0.2f, 0.35f, 0.9f));
                if (ImGui::Button(Guest ? (ICON_FA_CHECK " RESET GUEST") : (ICON_FA_TIMES " RESET GUEST"), ImVec2(-1, 35))) {
                    Guest = !Guest;
                }
                ImGui::PopStyleColor(2);
                ImGui::PopID();
                ImGui::NextColumn();
                
                ImGui::Columns(1);
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                int activeCount = 0;
                if (JumpHight) activeCount++;
                if (SpeedHack1) activeCount++; if (SpeedHack) activeCount++; if (camxa) activeCount++; if (Guest) activeCount++;
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.5f, 0.6f, 1.0f));
                ImGui::Text(ICON_FA_INFO_CIRCLE " ACTIVE FEATURES: %d/5", activeCount);
                ImGui::PopStyleColor();
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem(ICON_FA_PAINT_BRUSH " COLOR")) {
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
                ImGui::Text(ICON_FA_PAINT_BRUSH " COLOR CUSTOMIZATION");
                ImGui::PopStyleColor();
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.1f, 0.5f));
                ImGui::BeginChild("ColorSettings", ImVec2(0, 150), true);
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.8f, 1.0f, 1.0f));
                ImGui::Text(ICON_FA_TINT " Theme Color:");
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                if (ImGui::ColorEdit4("##NeonColor", (float*)&NeonColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
                    SetNeonBlueTheme();
                }
                
                ImGui::EndChild();
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.5f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.6f, 1.0f));
                if (ImGui::Button(ICON_FA_UNDO "  RESET DEFAULT COLOR", ImVec2(-1, 40))) {
                    NeonColor = ImVec4(0.0f, 0.8f, 1.0f, 1.0f);
                    SetNeonBlueTheme();
                }
                ImGui::PopStyleColor(2);
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem(ICON_FA_INFO_CIRCLE " ABOUT")) {
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
                ImGui::Text(ICON_FA_USER_CIRCLE " ZYGISK RAMA MODZ");
                ImGui::PopStyleColor();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.1f, 0.5f));
                ImGui::BeginChild("AboutInfo", ImVec2(0, 180), true);
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
                
                ImGui::Text(ICON_FA_USER_CIRCLE "  Developer:");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
                ImGui::Text("RAMA MODZ");
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                ImGui::Text(ICON_FA_TELEGRAM "  Telegram:");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
                ImGui::Text("@RAMA_MODZ");
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                ImGui::Text(ICON_FA_TELEGRAM "  Channel:");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
                ImGui::Text("@ramachanel");
                ImGui::PopStyleColor();
                ImGui::Spacing();
                
                ImGui::Separator();
                ImGui::Spacing();
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.5f, 0.6f, 1.0f));
                ImGui::Text(ICON_FA_COPYRIGHT "  ZYGISK MODULE BY RAMA MODZ");
                ImGui::Text(ICON_FA_SHIELD_ALT "  ALL RIGHTS RESERVED");
                ImGui::PopStyleColor();
                
                ImGui::PopStyleColor();
                ImGui::EndChild();
                ImGui::PopStyleColor();
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    
        ImGui::EndChild();
    ImGui::End();
    ImGui::Render();
}



uintptr_t il2cpp_base = 0;
void *getRealAddr(ulong offset) {
    return reinterpret_cast<void*>(il2cpp_base + offset);
};

namespace Settings
{
    static int Tab = 1;
}




bool clearMousePos = true;
bool initImGui = false;

inline EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
inline EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);
    if (!g_IsSetup) {
        prevWidth = g_GlWidth;
        prevHeight = g_GlHeight;
        SetupImgui();
        
        g_IsSetup = true;
    }
    static bool isTouching = false;
    static float touchStartTime = 0.0f;
    SetNeonBlueTheme();
    ImGuiIO& io = ImGui::GetIO(); ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
	ImGui::NewFrame();
    int touchCount = ((int (*)())(Class_Input__get_touchCount))();
    if (touchCount > 0) {
        UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields(*)(int))(Class_Input__GetTouch))(0);
        float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;
        io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
        if (!isTouching) {
            touchStartTime = ImGui::GetTime();}
        io.MouseDown[0] = true;
        isTouching = true;}
    else {if (isTouching && (ImGui::GetTime() - touchStartTime < 0.06f)) {
        io.MouseDown[0] = true;} else {
        UpdateSpeedhack();
        io.MouseDown[0] = false;
        isTouching = false;
   }  
}

    DrawESP(g_GlWidth, g_GlHeight);
    DrawMainUI();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        return old_eglSwapBuffers(dpy, surface);
}





typedef unsigned long DWORD;
 static uintptr_t libBase;



inline void StartGUI() {
    void *ptr_eglSwapBuffer = DobbySymbolResolver("/system/lib/libEGL.so", "eglSwapBuffers");
    if (ptr_eglSwapBuffer != nullptr) {
        DobbyHook((void *)ptr_eglSwapBuffer, (void *)hook_eglSwapBuffers, (void **)&old_eglSwapBuffers);
        LOGD("GUI started successfully");
    }
}


void hack_thread(pid_t pid) {
    LOGD("Hack Thread initiated for PID: %i", pid);

    for (int i = 0; i < 10; i++) {
        il2cpp_base = get_module_base(pid, "libil2cpp.so");
        if (il2cpp_base != 0) break;
        sleep(10);
    }

    if (il2cpp_base == 0) {
        LOGE("libil2cpp.so not found in thread %d", pid);
        std::terminate();
    }

    LOGD("IL2CPP base address: 0x%" PRIxPTR, il2cpp_base);
    sleep(10);
    Il2CppAttach();
    InitBypass();
    // CREADIT: BY RAMA MODZ
    // TELEGRAM OWNER: @RAMA_MODZ
    // CHANEL TELEGRAM: t.me/ramachanel
    // FUCK YOU DELETE CREDIT 
DobbyHook(Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("FollowCamera"), OBFUSCATE("get_OffsetForNormal"), 0),(void *)_GetCameraHeightRateValue, (void **)&GetCameraHeightRateValue);
DobbyHook(Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW"), OBFUSCATE("GameConfig"), OBFUSCATE("get_ResetGuest"), 0), (void *)_ResetGuest, (void **)&ResetGuest);
DobbyHook((void *)Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("GetCurrentDashSpeed"), 0),(void *)hook_Rama_Modz,(void **)&old_Rama_Modz);
	
DobbyHook(Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_MaxJumpHeight"), 0), (void*)my_get_MaxJumpHeight, (void**)&orig_get_MaxJumpHeight);
DobbyHook(Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_CustomGravity"), 0), (void*)my_get_CustomGravity, (void**)&orig_get_CustomGravity);
DobbyHook(Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_RisingGravity"), 0), (void*)my_get_RisingGravity, (void**)&orig_get_RisingGravity);
DobbyHook(Il2CppGetMethodOffset(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE("COW.GamePlay"), OBFUSCATE("Player"), OBFUSCATE("get_FallingGravity"), 0), (void*)my_get_FallingGravity, (void**)&orig_get_FallingGravity);
	

    StartGUI();
}


    
void hack() {
     LOGD("Inject Ok");
    std::thread thread_hack(hack_thread, getpid());
    thread_hack.detach();
    
}

REGISTER_ZYGISK_MODULE(MyModule)

