#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include <tuple>
#include <string>
#include <map>
#include "magic_enum.hpp"

struct ImGuiAutoAny {
    template<typename T>
    ImGuiAutoAny(T&& v) : ptr((void*)&v), is_lvalue(std::is_lvalue_reference<T>::value), type_id(typeid(v).hash_code()), get_ptr([&v]() -> void* { return (void*)&v; }) {}
    void* ptr;
    bool is_lvalue;
    size_t type_id;
    std::function<void*()> get_ptr;
};

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
    static void DrawControlAll(const char* const* labels, Args&&... args) {
        constexpr size_t N = sizeof...(Args);
        ImGuiAutoAny anys[N] = { ImGuiAutoAny(std::forward<Args>(args))... };
        size_t i = 0;
        auto args_tuple = std::forward_as_tuple(std::forward<Args>(args)...);
        while (i < N) {
            if (anys[i].is_lvalue) {
                size_t j = i + 1;
                while (j < N && !anys[j].is_lvalue) ++j;
                if (j == i + 1) {
                    auto& ref = *static_cast<typename std::remove_reference<decltype(std::get<i>(args_tuple))>::type*>(anys[i].ptr);
                    DrawControl(ref, labels[i]);
                } else {
                    auto tuple_call = [&](auto&&... params) {
                        auto& ref = *static_cast<typename std::remove_reference<decltype(std::get<i>(args_tuple))>::type*>(anys[i].ptr);
                        DrawControl(std::tuple_cat(std::make_tuple(std::ref(ref)), std::make_tuple(params...)), labels[i]);
                    };
                    std::apply([&](auto&&... all_args) {
                        tuple_call(std::get<i+1>(args_tuple), std::get<i+2>(args_tuple), std::get<i+3>(args_tuple), std::get<i+4>(args_tuple), std::get<i+5>(args_tuple), std::get<i+6>(args_tuple), std::get<i+7>(args_tuple), std::get<i+8>(args_tuple), std::get<i+9>(args_tuple));
                    }, args_tuple);
                }
                i = j;
            } else {
                ++i;
            }
        }
    }
};

inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::saved_time_left_map;
inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::loaded_time_left_map;


#define IMGUI_EXPAND(x) x

#define IMGUI_AUTO_IMPL1(a) [&](){ const char* labels[] = {#a}; ofxImGuiAuto::DrawControlAll(labels, a); }();
#define IMGUI_AUTO_IMPL2(a, b) [&](){ const char* labels[] = {#a, #b}; ofxImGuiAuto::DrawControlAll(labels, a, b); }();
#define IMGUI_AUTO_IMPL3(a, b, c) [&](){ const char* labels[] = {#a, #b, #c}; ofxImGuiAuto::DrawControlAll(labels, a, b, c); }();
#define IMGUI_AUTO_IMPL4(a, b, c, d) [&](){ const char* labels[] = {#a, #b, #c, #d}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d); }();
#define IMGUI_AUTO_IMPL5(a, b, c, d, e) [&](){ const char* labels[] = {#a, #b, #c, #d, #e}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e); }();
#define IMGUI_AUTO_IMPL6(a, b, c, d, e, f) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f); }();
#define IMGUI_AUTO_IMPL7(a, b, c, d, e, f, g) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g); }();
#define IMGUI_AUTO_IMPL8(a, b, c, d, e, f, g, h) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h); }();
#define IMGUI_AUTO_IMPL9(a, b, c, d, e, f, g, h, i) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i); }();
#define IMGUI_AUTO_IMPL10(a, b, c, d, e, f, g, h, i, j) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j); }();
#define IMGUI_AUTO_IMPL11(a, b, c, d, e, f, g, h, i, j, k) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k); }();
#define IMGUI_AUTO_IMPL12(a, b, c, d, e, f, g, h, i, j, k, l) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l); }();
#define IMGUI_AUTO_IMPL13(a, b, c, d, e, f, g, h, i, j, k, l, m) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m); }();
#define IMGUI_AUTO_IMPL14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n); }();
#define IMGUI_AUTO_IMPL15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o); }();
#define IMGUI_AUTO_IMPL16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p); }();
#define IMGUI_AUTO_IMPL17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q); }();
#define IMGUI_AUTO_IMPL18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r); }();
#define IMGUI_AUTO_IMPL19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s); }();
#define IMGUI_AUTO_IMPL20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t); }();
#define IMGUI_AUTO_IMPL21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u); }();
#define IMGUI_AUTO_IMPL22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v); }();
#define IMGUI_AUTO_IMPL23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w); }();
#define IMGUI_AUTO_IMPL24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w, #x}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x); }();
#define IMGUI_AUTO_IMPL25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w, #x, #y}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y); }();
#define IMGUI_AUTO_IMPL26(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w, #x, #y, #z}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z); }();
#define IMGUI_AUTO_IMPL27(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w, #x, #y, #z, #aa}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa); }();
#define IMGUI_AUTO_IMPL28(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w, #x, #y, #z, #aa, #ab}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab); }();
#define IMGUI_AUTO_IMPL29(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab, ac) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w, #x, #y, #z, #aa, #ab, #ac}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab, ac); }();
#define IMGUI_AUTO_IMPL30(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad) [&](){ const char* labels[] = {#a, #b, #c, #d, #e, #f, #g, #h, #i, #j, #k, #l, #m, #n, #o, #p, #q, #r, #s, #t, #u, #v, #w, #x, #y, #z, #aa, #ab, #ac, #ad}; ofxImGuiAuto::DrawControlAll(labels, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad); }();

#define IMGUI_AUTO_IMPL_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,NAME,...) NAME
#define IMGUI_AUTOS(...) IMGUI_EXPAND(IMGUI_AUTO_IMPL_GET_MACRO(__VA_ARGS__, IMGUI_AUTO_IMPL30, IMGUI_AUTO_IMPL29, IMGUI_AUTO_IMPL28, IMGUI_AUTO_IMPL27, IMGUI_AUTO_IMPL26, IMGUI_AUTO_IMPL25, IMGUI_AUTO_IMPL24, IMGUI_AUTO_IMPL23, IMGUI_AUTO_IMPL22, IMGUI_AUTO_IMPL21, IMGUI_AUTO_IMPL20, IMGUI_AUTO_IMPL19, IMGUI_AUTO_IMPL18, IMGUI_AUTO_IMPL17, IMGUI_AUTO_IMPL16, IMGUI_AUTO_IMPL15, IMGUI_AUTO_IMPL14, IMGUI_AUTO_IMPL13, IMGUI_AUTO_IMPL12, IMGUI_AUTO_IMPL11, IMGUI_AUTO_IMPL10, IMGUI_AUTO_IMPL9, IMGUI_AUTO_IMPL8, IMGUI_AUTO_IMPL7, IMGUI_AUTO_IMPL6, IMGUI_AUTO_IMPL5, IMGUI_AUTO_IMPL4, IMGUI_AUTO_IMPL3, IMGUI_AUTO_IMPL2, IMGUI_AUTO_IMPL1)(__VA_ARGS__))

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
