#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include <tuple>
#include <string>
#include <map>
#include "magic_enum.hpp"

class ofxImGuiAuto {
public:
    class SaveLoadButton {
    public:
        static inline bool Save(const char* name, const char* msg = "Saved!") {
            ImGuiID id = ImGui::GetID(name);
            if(saved_time_left_map.find(id) == saved_time_left_map.end()) saved_time_left_map[id] = 0;
            bool b = ImGui::Button(name);
            if(b) saved_time_left_map[id] = 1.0f;
            if(saved_time_left_map[id] > 0) {
                float dt = ofGetLastFrameTime();
                saved_time_left_map[id] -= dt;
                ImGui::SameLine();
                ImGui::Text(msg);
            }
            return b;
        }
        static inline bool Load(const char* name, const char* msg = "Loaded!") {
            ImGuiID id = ImGui::GetID(name);
            if(loaded_time_left_map.find(id) == loaded_time_left_map.end()) loaded_time_left_map[id] = 0;
            bool b = ImGui::Button(name);
            if(b) loaded_time_left_map[id] = 1.0f;
            if(loaded_time_left_map[id] > 0) {
                float dt = ofGetLastFrameTime();
                loaded_time_left_map[id] -= dt;
                ImGui::SameLine();
                ImGui::Text(msg);
            }
            return b;
        }
    protected:
        static std::map<ImGuiID, float> saved_time_left_map;
        static std::map<ImGuiID, float> loaded_time_left_map;
    };

    template<typename T>
    static void DrawControl(T& value, const char* label) {
        if constexpr (std::is_enum_v<T>) {
            auto names = magic_enum::enum_names<T>();
            int current = static_cast<int>(value);
            if (ImGui::Combo(label, &current, names.data(), names.size())) {
                value = static_cast<T>(magic_enum::enum_value<T>(current));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            ImGui::Checkbox(label, &value);
        } else if constexpr (std::is_same_v<T, float>) {
            ImGui::DragFloat(label, &value);
        } else if constexpr (std::is_same_v<T, int>) {
            ImGui::DragInt(label, &value);
        } else if constexpr (std::is_same_v<T, ofVec2f>) {
            ImGui::DragFloat2(label, &value.x);
        } else if constexpr (std::is_same_v<T, ofVec3f>) {
            ImGui::DragFloat3(label, &value.x);
        } else if constexpr (std::is_same_v<T, ofRectangle>) {
            float rect[4] = { value.x, value.y, value.width, value.height };
            if (ImGui::DragFloat4(label, rect)) {
                value.x = rect[0];
                value.y = rect[1];
                value.width = rect[2];
                value.height = rect[3];
            }
        }
    }

    template<typename T, typename... Args>
    static void DrawControl(std::tuple<T&, Args...> tup, const char* label) {
        DrawControlTuple(label, tup, std::index_sequence_for<Args...>{});
    }

    template<typename T, typename... Args, size_t... I>
    static void DrawControlTuple(const char* label, std::tuple<T&, Args...>& tup, std::index_sequence<I...>) {
        auto& v = std::get<0>(tup);
        if constexpr (std::is_enum_v<T>) {
            auto names = magic_enum::enum_names<T>();
            int current = static_cast<int>(v);
            if (ImGui::Combo(label, &current, names.data(), names.size())) {
                v = static_cast<T>(magic_enum::enum_value<T>(current));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            ImGui::Checkbox(label, &v);
        } else if constexpr (std::is_same_v<T, float>) {
            ImGui::DragFloat(label, &v, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, int>) {
            ImGui::DragInt(label, &v, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, ofVec2f>) {
            ImGui::DragFloat2(label, &v.x, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, ofVec3f>) {
            ImGui::DragFloat3(label, &v.x, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, ofRectangle>) {
            float rect[4] = { v.x, v.y, v.width, v.height };
            if (ImGui::DragFloat4(label, rect, std::get<I+1>(tup)...)) {
                v.x = rect[0];
                v.y = rect[1];
                v.width = rect[2];
                v.height = rect[3];
            }
        }
    }

    template<typename... Args>
    static void DrawControlsVA(const char* const (&labels)[sizeof...(Args)], Args&... args) {
        int i = 0;
        (DrawControl(args, labels[i++]), ...);
    }
};

inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::saved_time_left_map;
inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::loaded_time_left_map;


#define IMGUI_EXPAND(x) x
#define IMGUI_KV1(name) ofxImGuiAuto::DrawControl(name, #name);
#define IMGUI_KV2(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV1(__VA_ARGS__))
#define IMGUI_KV3(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV2(__VA_ARGS__))
#define IMGUI_KV4(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV3(__VA_ARGS__))
#define IMGUI_KV5(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV4(__VA_ARGS__))
#define IMGUI_KV6(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV5(__VA_ARGS__))
#define IMGUI_KV7(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV6(__VA_ARGS__))
#define IMGUI_KV8(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV7(__VA_ARGS__))
#define IMGUI_KV9(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV8(__VA_ARGS__))
#define IMGUI_KV10(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV9(__VA_ARGS__))
#define IMGUI_KV11(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV10(__VA_ARGS__))
#define IMGUI_KV12(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV11(__VA_ARGS__))
#define IMGUI_KV13(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV12(__VA_ARGS__))
#define IMGUI_KV14(name, ...) IMGUI_KV1(name) IMGUI_EXPAND(IMGUI_KV13(__VA_ARGS__))
#define IMGUI_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,NAME,...) NAME

#define IMGUI_KV_EXPAND(...) IMGUI_EXPAND(IMGUI_GET_MACRO(__VA_ARGS__, IMGUI_KV14, IMGUI_KV13, IMGUI_KV12, IMGUI_KV11, IMGUI_KV10, IMGUI_KV9, IMGUI_KV8, IMGUI_KV7, IMGUI_KV6, IMGUI_KV5, IMGUI_KV4, IMGUI_KV3, IMGUI_KV2, IMGUI_KV1)(__VA_ARGS__))

#define IMGUI_AUTOS(...) IMGUI_KV_EXPAND(__VA_ARGS__)

// Helper macros for tuple creation (support zero or more args)


// Macro overload trick for MSVC: count args and select correct tuple macro
#define IMGUI_AUTO_CHOOSER(_1, _2, NAME, ...) NAME
#define IMGUI_AUTO_1(name) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name)), #name);
#define IMGUI_AUTO_N(name, ...) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name), IMGUI_EXPAND(__VA_ARGS__)), #name);
#define IMGUI_AUTO(...) IMGUI_EXPAND(IMGUI_AUTO_CHOOSER(__VA_ARGS__, IMGUI_AUTO_N, IMGUI_AUTO_1)(__VA_ARGS__))

// Save/Load button macro
#define IMGUI_AUTO_SAVE_LOAD(saveFunc, loadFunc, saveLabel, loadLabel) \
    if(ofxImGuiAuto::SaveLoadButton::Save(saveLabel)) { saveFunc; } \
    if(ofxImGuiAuto::SaveLoadButton::Load(loadLabel)) { loadFunc; }
