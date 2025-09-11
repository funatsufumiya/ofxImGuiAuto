#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include <tuple>
#include <string>
#include <cctype>
#include <map>
#include "magic_enum.hpp"

class ofxImGuiAuto {

public:
    struct ImGuiAutoVariant {
        enum class Type {
            TYPE_NONE,
            TYPE_BOOL,
            TYPE_FLOAT,
            TYPE_INT,
            TYPE_VEC2F,
            TYPE_VEC3F,
            TYPE_RECT,
            TYPE_ENUM
        };
        // for l_value
        union {
            bool* b;
            float* f;
            int* i;
            ofVec2f* v2;
            ofVec3f* v3;
            ofRectangle* r;
            void* e; // keep enum as void*
        } data = {};
        // rvalue value keep
        float f_value = 0.0f;
        int i_value = 0;
        bool b_value = false;
        ofVec2f v2_value;
        ofVec3f v3_value;
        ofRectangle r_value;

        // lvalue reference
        ImGuiAutoVariant(bool& v)      : typ(Type::TYPE_BOOL),  _is_lvalue(true)  { data.b = &v; }
        ImGuiAutoVariant(float& v)     : typ(Type::TYPE_FLOAT), _is_lvalue(true)  { data.f = &v; }
        ImGuiAutoVariant(int& v)       : typ(Type::TYPE_INT),   _is_lvalue(true)  { data.i = &v; }
        ImGuiAutoVariant(ofVec2f& v)   : typ(Type::TYPE_VEC2F), _is_lvalue(true)  { data.v2 = &v; }
        ImGuiAutoVariant(ofVec3f& v)   : typ(Type::TYPE_VEC3F), _is_lvalue(true)  { data.v3 = &v; }
        ImGuiAutoVariant(ofRectangle& v): typ(Type::TYPE_RECT), _is_lvalue(true)  { data.r = &v; }
        template<typename T>
        ImGuiAutoVariant(T& v, std::enable_if_t<std::is_enum<T>::value>* = nullptr)
            : typ(Type::TYPE_ENUM), _is_lvalue(true) { data.e = (void*)&v; }

        // rvalue (temporary, etc.)
        ImGuiAutoVariant(bool&& v)      : typ(Type::TYPE_BOOL),  _is_lvalue(false), b_value(v)  { data.b = &b_value; }
        ImGuiAutoVariant(float&& v)     : typ(Type::TYPE_FLOAT), _is_lvalue(false), f_value(v)  { data.f = &f_value; }
        ImGuiAutoVariant(int&& v)       : typ(Type::TYPE_INT),   _is_lvalue(false), i_value(v)  { data.i = &i_value; }
        ImGuiAutoVariant(ofVec2f&& v)   : typ(Type::TYPE_VEC2F), _is_lvalue(false), v2_value(v) { data.v2 = &v2_value; }
        ImGuiAutoVariant(ofVec3f&& v)   : typ(Type::TYPE_VEC3F), _is_lvalue(false), v3_value(v) { data.v3 = &v3_value; }
        ImGuiAutoVariant(ofRectangle&& v): typ(Type::TYPE_RECT), _is_lvalue(false), r_value(v)  { data.r = &r_value; }
        template<typename T>
        ImGuiAutoVariant(T&& v, std::enable_if_t<std::is_enum<T>::value>* = nullptr)
            : typ(Type::TYPE_ENUM), _is_lvalue(false) { data.e = (void*)&v; }

        Type get_type() const { return typ; }
        bool is_float() const { return typ == Type::TYPE_FLOAT; }
        bool is_lvalue() const { return _is_lvalue; }

    protected:
        bool _is_lvalue = false;
        Type typ = Type::TYPE_NONE;
    };

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
            auto values = magic_enum::enum_values<T>();
            auto currentName = magic_enum::enum_name(value);
            if (ImGui::BeginCombo(label, std::string(currentName).c_str())) {
                for (int i = 0; i < names.size(); i++) {
                    bool is_selected = (value == values[i]);
                    if (ImGui::Selectable(std::string(names[i]).c_str(), is_selected)) {
                        value = values[i];
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
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
            auto values = magic_enum::enum_values<T>();
            auto currentName = magic_enum::enum_name(v);
            if (ImGui::BeginCombo(label, std::string(currentName).c_str())) {
                for (int i = 0; i < names.size(); i++) {
                    bool is_selected = (v == values[i]);
                    if (ImGui::Selectable(std::string(names[i]).c_str(), is_selected)) {
                        v = values[i];
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
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

    // template<typename... Args>
    // static void DrawControlsVA(const char* const (&labels)[sizeof...(Args)], Args&... args) {
    //     int i = 0;
    //     (DrawControl(args, labels[i++]), ...);
    // }

    template<typename... Args>
    // static void DrawControlsVA(const char* const* labels, Args&&... args) {
    static void DrawControlsVA(const char* labels_str, ImGuiAutoVariant* variants) {
        auto labels = SplitAndTrimLabels(labels_str);
        size_t N = labels.size();
        vector<bool> is_labels;
        for(size_t i=0; i<N; ++i){
            is_labels.push_back(is_label(labels[i]));
        }

        size_t i = 0;
        while (i < N) {
            if (is_labels[i]) {
                ofLog() << "label: " << labels[i];
                auto var = variants[i].data.f;
                size_t j = i + 1;
                std::vector<float> params;
                while (j < N && !is_labels[j]) {
                    params.push_back(*variants[j].data.f);
                    ++j;
                }
                if (params.empty()) {
                    DrawControl(*var, labels[i].c_str());
                } else if (params.size() == 1) {
                    DrawControl(std::make_tuple(std::ref(*var), params[0]), labels[i].c_str());
                } else if (params.size() == 2) {
                    DrawControl(std::make_tuple(std::ref(*var), params[0], params[1]), labels[i].c_str());
                } else if (params.size() == 3) {
                    DrawControl(std::make_tuple(std::ref(*var), params[0], params[1], params[2]), labels[i].c_str());
                }
                i = j;
            } else {
                ++i;
            }
        }
    }

    // template<typename... Args>
    // static void DrawControlsVA(const char* const* labels, Args&&... args) {
    //     constexpr size_t N = sizeof...(Args);
    //     ImGuiAutoAny anys[N] = { ImGuiAutoAny(std::forward<Args>(args))... };
    //     size_t i = 0;
    //     auto args_tuple = std::forward_as_tuple(std::forward<Args>(args)...);
    //     while (i < N) {
    //         if (anys[i].is_lvalue) {
    //             size_t j = i + 1;
    //             while (j < N && !anys[j].is_lvalue) ++j;
    //             if (j == i + 1) {
    //                 auto& ref = *static_cast<typename std::remove_reference<decltype(std::get<i>(args_tuple))>::type*>(anys[i].ptr);
    //                 DrawControl(ref, labels[i]);
    //             } else {
    //                 auto tuple_call = [&](auto&&... params) {
    //                     auto& ref = *static_cast<typename std::remove_reference<decltype(std::get<i>(args_tuple))>::type*>(anys[i].ptr);
    //                     DrawControl(std::tuple_cat(std::make_tuple(std::ref(ref)), std::make_tuple(params...)), labels[i]);
    //                 };
    //                 std::apply([&](auto&&... all_args) {
    //                     tuple_call(std::get<i+1>(args_tuple), std::get<i+2>(args_tuple), std::get<i+3>(args_tuple), std::get<i+4>(args_tuple), std::get<i+5>(args_tuple), std::get<i+6>(args_tuple), std::get<i+7>(args_tuple), std::get<i+8>(args_tuple), std::get<i+9>(args_tuple));
    //                 }, args_tuple);
    //             }
    //             i = j;
    //         } else {
    //             ++i;
    //         }
    //     }
    // }

    protected:
        static std::vector<std::string> SplitAndTrimLabels(const char* labels) {
            std::vector<std::string> result;
            std::istringstream ss(labels);
            std::string item;
            while (std::getline(ss, item, ',')) {
                // trim
                item.erase(item.begin(), std::find_if(item.begin(), item.end(), [](unsigned char ch) { return !std::isspace(ch); }));
                item.erase(std::find_if(item.rbegin(), item.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), item.end());
                result.push_back(item);
            }
            return result;
        }

        static bool is_label(const std::string& str) {
            if (str.empty()) return false;
            char c = str[0];
            if (c == '\"' || c == '\'' || c == '.') return false;
            if (std::isdigit(static_cast<unsigned char>(c))) return false;
            return true;
        }
};

inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::saved_time_left_map;
inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::loaded_time_left_map;


#define IMGUI_AUTOS(...) [&](){ \
    const char* labels_str = #__VA_ARGS__; \
    ofxImGuiAuto::ImGuiAutoVariant variants[] = {__VA_ARGS__}; \
    ofxImGuiAuto::DrawControlsVA(labels_str, variants); \
}()

// Helper macros for tuple creation (support zero or more args)


// Macro overload trick for MSVC: count args and select correct tuple macro
#define IMGUI_AUTO_CHOOSER(_1, _2, NAME, ...) NAME
#define IMGUI_AUTO_1(name) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name)), #name);
#define IMGUI_AUTO_N(name, ...) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name), IMGUI_EXPAND(__VA_ARGS__)), #name);
/// @brief IMGUI_AUTO(var_a) or IMGUI_AUTO(var_a, param1, param2, ...)
#define IMGUI_AUTO(...) IMGUI_EXPAND(IMGUI_AUTO_CHOOSER(__VA_ARGS__, IMGUI_AUTO_N, IMGUI_AUTO_1)(__VA_ARGS__))


// Save/Load button macro overloads
#define IMGUI_AUTO_SAVE_LOAD_CHOOSER(_1,_2,_3,_4,NAME,...) NAME

#define IMGUI_AUTO_SAVE_LOAD_2(saveFunc, loadFunc) \
    if(ofxImGuiAuto::SaveLoadButton::Save("save")) { IMGUI_EXPAND(saveFunc); } \
    if(ofxImGuiAuto::SaveLoadButton::Load("load")) { IMGUI_EXPAND(loadFunc); }
#define IMGUI_AUTO_SAVE_LOAD_4(saveFunc, loadFunc, saveLabel, loadLabel) \
    if(ofxImGuiAuto::SaveLoadButton::Save(saveLabel)) { IMGUI_EXPAND(saveFunc); } \
    if(ofxImGuiAuto::SaveLoadButton::Load(loadLabel)) { IMGUI_EXPAND(loadFunc); }
/// @brief IMGUI_AUTO_SAVE_LOAD(save(), load()) or IMGUI_AUTO_SAVE_LOAD(save(), load(), "save", "load")
#define IMGUI_AUTO_SAVE_LOAD(...) IMGUI_EXPAND(IMGUI_AUTO_SAVE_LOAD_CHOOSER(__VA_ARGS__, IMGUI_AUTO_SAVE_LOAD_4, unused, IMGUI_AUTO_SAVE_LOAD_2)(__VA_ARGS__))
