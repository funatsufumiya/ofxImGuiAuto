#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include <tuple>
#include <string>
#include <map>
#include <typeindex>
#include <cctype>
#include <optional>
#include <functional>
#include "magic_enum.hpp"

#define IMGUI_AUTO_CACHE_PASING_RESULT

class ofxImGuiAuto {
public:
    struct EnumNames {
        template<typename T>
        // EnumNames(const T& arr) : list(arr.begin(), arr.end()) {}
        EnumNames(const T& arr){
            for(auto v: arr){
                list.push_back(v);
            }
        }

        std::vector<std::string_view> list;
    };

    struct EnumValues {
        template<typename T>
        EnumValues(const T& arr){
            for(auto v: arr){
                list.push_back(static_cast<int>(v));
            }
        }

        std::vector<int> list;
    };

    struct ControlParams {
        float v_speed = 1.0f;
        float v_min = 0.0f;
        float v_max = 0.0f;
        const char* format = "%.3f";
        ImGuiSliderFlags flags = 0;
    };

    struct Variant {
        enum class Type {
            TYPE_NONE,
            TYPE_CONST_CHAR,
            TYPE_BOOL,
            TYPE_DOUBLE,
            TYPE_FLOAT,
            TYPE_INT,
            TYPE_VEC2F,
            TYPE_VEC3F,
            TYPE_RECT,
            TYPE_COLOR,
            TYPE_ENUM,
            TYPE_ENUM_NAMES,
            TYPE_ENUM_VALUES
        };
        union LValue {
            const char** s;
            bool* b;
            float* f;
            double* d;
            int* i;
            ofVec2f* v2;
            ofVec3f* v3;
            ofColor* c;
            ofRectangle* r;
            void* e; // enum

            LValue() : s(nullptr) {}
            ~LValue() {}
        } lvalue;

        union RValue {
            const char* s;
            bool b;
            float f;
            double d;
            int i;
            ofVec2f v2;
            ofVec3f v3;
            ofColor c;
            ofRectangle r;
            void* e; // enum

            RValue() : s(nullptr) {}
            ~RValue() {}
        } rvalue;

        Type typ = Type::TYPE_NONE;
        std::type_index enum_type = typeid(void);

        // lvalue constructors
        Variant(const char*& v)         : typ(Type::TYPE_CONST_CHAR), _is_rvalue(false) { lvalue.s = &v; }
        Variant(bool& v)                : typ(Type::TYPE_BOOL), _is_rvalue(false)       { lvalue.b = &v; }
        Variant(float& v)               : typ(Type::TYPE_FLOAT), _is_rvalue(false)      { lvalue.f = &v; }
        Variant(double& v)               : typ(Type::TYPE_DOUBLE), _is_rvalue(false)      { lvalue.d = &v; }
        Variant(int& v)                 : typ(Type::TYPE_INT), _is_rvalue(false)        { lvalue.i = &v; }
        Variant(ofVec2f& v)             : typ(Type::TYPE_VEC2F), _is_rvalue(false)      { lvalue.v2 = &v; }
        Variant(ofVec3f& v)             : typ(Type::TYPE_VEC3F), _is_rvalue(false)      { lvalue.v3 = &v; }
        Variant(ofColor& v)             : typ(Type::TYPE_COLOR), _is_rvalue(false)      { lvalue.c = &v; }
        Variant(ofRectangle& v)         : typ(Type::TYPE_RECT), _is_rvalue(false)       { lvalue.r = &v; }
        Variant(EnumNames& v)         : typ(Type::TYPE_ENUM_NAMES), _is_rvalue(false)       { lvalue.e = &v; }
        Variant(EnumValues& v)         : typ(Type::TYPE_ENUM_VALUES), _is_rvalue(false)       { lvalue.e = &v; }
        template<typename T>
        Variant(T& v, std::enable_if_t<std::is_enum<T>::value>* = nullptr)
            : typ(Type::TYPE_ENUM), _is_rvalue(false), enum_type(typeid(T))            { lvalue.e = (void*)&v; }

        // rvalue constructors
        Variant(const char*&& v)          : typ(Type::TYPE_CONST_CHAR), _is_rvalue(true)  { rvalue.s = v; }
        Variant(bool&& v)                 : typ(Type::TYPE_BOOL), _is_rvalue(true)        { rvalue.b = v; }
        Variant(float&& v)                : typ(Type::TYPE_FLOAT), _is_rvalue(true)       { rvalue.f = v; }
        Variant(double&& v)                : typ(Type::TYPE_DOUBLE), _is_rvalue(true)       { rvalue.d = v; }
        Variant(int&& v)                  : typ(Type::TYPE_INT), _is_rvalue(true)         { rvalue.i = v; }
        Variant(ofVec2f&& v)              : typ(Type::TYPE_VEC2F), _is_rvalue(true)       { rvalue.v2 = v; }
        Variant(ofVec3f&& v)              : typ(Type::TYPE_VEC3F), _is_rvalue(true)       { rvalue.v3 = v; }
        Variant(ofColor&& v)              : typ(Type::TYPE_COLOR), _is_rvalue(true)       { rvalue.c = v; }
        Variant(ofRectangle&& v)          : typ(Type::TYPE_RECT), _is_rvalue(true)        { rvalue.r = v; }
        Variant(EnumNames&& v)          : typ(Type::TYPE_ENUM_NAMES), _is_rvalue(true)        { rvalue.e = &v; } // WORKAROUND
        Variant(EnumValues&& v)          : typ(Type::TYPE_ENUM_VALUES), _is_rvalue(true)        { rvalue.e = &v; } // WORKAROUND
        template<typename T>
        Variant(T&& v, std::enable_if_t<std::is_enum<T>::value>* = nullptr)
            : typ(Type::TYPE_ENUM), _is_rvalue(true), enum_type(typeid(T))             { rvalue.e = nullptr; } // WORKAROUND

        ~Variant() {} // Explicit destructor to satisfy union requirements

        Type get_type() const { return typ; }
        bool is_float() const { return typ == Type::TYPE_FLOAT; }
        bool is_lvalue() const { return !_is_rvalue; }
        bool is_rvalue() const { return _is_rvalue; }
        const std::type_index& get_enum_type() const { return enum_type; }

    protected:
        bool _is_rvalue = false;
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
        DrawControl(std::make_tuple(std::ref(value)), label);
    }

    template<typename T, typename... Args>
    static void DrawControl(std::tuple<T&, Args...> tup, const char* label) {
        DrawControlTuple(label, tup, std::index_sequence_for<Args...>{});
    }

    template<typename T, typename... Args>
    static void DrawControl(std::tuple<T&, Args...> tup, const char* label, const EnumNames& enum_names, const EnumValues& enum_values) {
        DrawControlTuple(label, enum_names, enum_values, tup, std::index_sequence_for<Args...>{});
    }

    template<typename T, typename... Args, size_t... I>
    static void DrawControlTuple(const char* label, const EnumNames& enum_names, const EnumValues& enum_values, std::tuple<T&, Args...>& tup, std::index_sequence<I...>) {
        auto& v = std::get<0>(tup);
        auto names = enum_names.list;
        int current = static_cast<int>(v);
        auto values = enum_values.list;
        auto currentName = names[v];
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
        } else if constexpr (std::is_same_v<T, int>) {
            ImGui::DragInt(label, &v, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, float>) {
            ImGui::DragFloat(label, &v, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, ofVec2f>) {
            ImGui::DragFloat2(label, &v.x, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, ofVec3f>) {
            ImGui::DragFloat3(label, &v.x, std::get<I+1>(tup)...);
        } else if constexpr (std::is_same_v<T, ofColor>) {
            float col[4] = { v.r / 255.0f, v.g / 255.0f, v.b / 255.0f, v.a / 255.0f };
            if(ImGui::ColorEdit4(label, col)){
                v.r = col[0] * 255.0f;
                v.g = col[1] * 255.0f;
                v.b = col[2] * 255.0f;
                v.a = col[3] * 255.0f;
            }
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

    static void DrawControlsVA(const char* labels_str, Variant* variants, std::optional<size_t> cache_key = std::nullopt) {
        std::vector<std::string> labels;

        #if defined(IMGUI_AUTO_CACHE_PASING_RESULT) && !defined(IMGUI_AUTO_NO_CACHE)
        if (cache_key) {
            labels = SplitAndTrimLabelsCached(labels_str, cache_key.value());
        } else {
            labels = SplitAndTrimLabels(labels_str);
        }
        #else
        labels = SplitAndTrimLabels(labels_str);
        #endif // IMGUI_AUTO_CACHE_PASING_RESULT

        size_t N = labels.size();
        std::vector<bool> is_labels;
        for(size_t i=0; i<N; ++i){
            is_labels.push_back(is_label(labels[i]));
        }

        size_t i = 0;
        while (i < N) {
            if (is_labels[i]) {
                // ofLog() << "label: " << labels[i];
                Variant& v = variants[i];
                size_t j = i + 1;
                EnumNames* enum_names;
                EnumValues* enum_values;
                ControlParams params;
                    if (j < N && !is_labels[j]) {
                        if (variants[j].get_type() == Variant::Type::TYPE_ENUM_NAMES) {
                            if(variants[j].is_lvalue()){
                                enum_names = reinterpret_cast<EnumNames*>(variants[j].lvalue.e);
                            }else{
                                enum_names = reinterpret_cast<EnumNames*>(variants[j].rvalue.e);
                            }
                        }else if (variants[j].get_type() == Variant::Type::TYPE_FLOAT) {
                            params.v_speed = variants[j].rvalue.f;
                        } else if (variants[j].get_type() == Variant::Type::TYPE_DOUBLE) {
                            params.v_speed = static_cast<float>(variants[j].rvalue.d);
                        } else if (variants[j].get_type() == Variant::Type::TYPE_INT) {
                            params.v_speed = static_cast<float>(variants[j].rvalue.i);
                        }
                        ++j;
                    }
                    if (j < N && !is_labels[j]) {
                        if (variants[j].get_type() == Variant::Type::TYPE_ENUM_VALUES) {
                            if(variants[j].is_lvalue()){
                                enum_values = reinterpret_cast<EnumValues*>(variants[j].lvalue.e);
                            }else{
                                enum_values = reinterpret_cast<EnumValues*>(variants[j].rvalue.e);
                            }
                        } else if (variants[j].get_type() == Variant::Type::TYPE_FLOAT) {
                            params.v_min = variants[j].rvalue.f;
                        } else if (variants[j].get_type() == Variant::Type::TYPE_DOUBLE) {
                            params.v_min = static_cast<float>(variants[j].rvalue.d);
                        } else if (variants[j].get_type() == Variant::Type::TYPE_INT) {
                            params.v_min = static_cast<float>(variants[j].rvalue.i);
                        }
                        ++j;
                    }
                    if (j < N && !is_labels[j]) {
                        if (variants[j].get_type() == Variant::Type::TYPE_FLOAT) {
                            params.v_max = variants[j].rvalue.f;
                        } else if (variants[j].get_type() == Variant::Type::TYPE_DOUBLE) {
                            params.v_max = static_cast<float>(variants[j].rvalue.d);
                        } else if (variants[j].get_type() == Variant::Type::TYPE_INT) {
                            params.v_max = static_cast<float>(variants[j].rvalue.i);
                        }
                        ++j;
                    }
                if (j < N && !is_labels[j]) { params.format  = variants[j].rvalue.s; ++j; }
                if (j < N && !is_labels[j]) { params.flags   = variants[j].rvalue.i; ++j; }
                switch (v.get_type()) {
                    case Variant::Type::TYPE_FLOAT:
                        callDrawControl(v.lvalue.f, params, labels[i].c_str());
                        break;
                    case Variant::Type::TYPE_INT:
                        callDrawControl(v.lvalue.i, params, labels[i].c_str());
                        break;
                    case Variant::Type::TYPE_BOOL:
                        callDrawControl(v.lvalue.b, params, labels[i].c_str());
                        break;
                    case Variant::Type::TYPE_VEC2F:
                        callDrawControl(v.lvalue.v2, params, labels[i].c_str());
                        break;
                    case Variant::Type::TYPE_VEC3F:
                        callDrawControl(v.lvalue.v3, params, labels[i].c_str());
                        break;
                    case Variant::Type::TYPE_RECT:
                        callDrawControl(v.lvalue.r, params, labels[i].c_str());
                        break;
                    case Variant::Type::TYPE_COLOR:
                        callDrawControl(v.lvalue.c, params, labels[i].c_str());
                        break;
                    case Variant::Type::TYPE_ENUM:
                        if(enum_names != nullptr && enum_values != nullptr){
                            callDrawControl(reinterpret_cast<int*>(v.lvalue.e), *enum_names, *enum_values, params, labels[i].c_str());
                        }else{
                            callDrawControl<int>(reinterpret_cast<int*>(v.lvalue.e), params, labels[i].c_str());
                        }
                        break;
                    case Variant::Type::TYPE_CONST_CHAR:
                    case Variant::Type::TYPE_NONE:
                    default:
                        break;
                }
                i = j;
            } else {
                ++i;
            }
        }
    }

    protected:
        static std::map<size_t, std::vector<std::string>>& GetLabelCache() {
            static std::map<size_t, std::vector<std::string>> cache;
            return cache;
        }
        static std::vector<std::string> SplitAndTrimLabelsCached(const char* labels, size_t cache_key) {
            auto& cache = GetLabelCache();
            auto it = cache.find(cache_key);
            if (it != cache.end()) return it->second;
            std::vector<std::string> result;
            std::istringstream ss(labels);
            std::string item;
            while (std::getline(ss, item, ',')) {
                // trim
                item.erase(item.begin(), std::find_if(item.begin(), item.end(), [](unsigned char ch) { return !std::isspace(ch); }));
                item.erase(std::find_if(item.rbegin(), item.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), item.end());
                result.push_back(item);
            }
            cache[cache_key] = result;
            return cache[cache_key];
        }
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
            if (c == '\"' || c == '\'' || c == '.' || c == '[' || c == '{' || c == '(') return false;
            if (std::isdigit(static_cast<unsigned char>(c))) return false;
            return true;
        }

        template<typename T>
        static void callDrawControl(T* v, const ControlParams& params, const char* label) {
            if (!v) return;
            DrawControl(std::make_tuple(std::ref(*v), params.v_speed, params.v_min, params.v_max, params.format, params.flags), label);
        }

        static void callDrawControl(int* v, const EnumNames& enum_names, const EnumValues& enum_values, const ControlParams& params, const char* label) {
            if (!v) return;
            DrawControl(std::make_tuple(std::ref(*v), params.v_speed, params.v_min, params.v_max, params.format, params.flags), label, enum_names, enum_values);
        }
};

inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::saved_time_left_map;
inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::loaded_time_left_map;

/// @brief IMGUI_AUTOS2(var_a, param1, param2, var_b, var_c, param1, ...)
#define IMGUI_AUTOS2(...) [&](){ \
    const char* labels_str = #__VA_ARGS__; \
    const size_t cache_key = std::hash<const char*>()(labels_str); \
    ofxImGuiAuto::Variant variants[] = {__VA_ARGS__}; \
    ofxImGuiAuto::DrawControlsVA(labels_str, variants, cache_key); \
}();

/// @brief IMGUI_AUTOS2_NO_CACHE(var_a, param1, param2, var_b, var_c, param1, ...)
#define IMGUI_AUTOS2_NO_CACHE(...) [&](){ \
    const char* labels_str = #__VA_ARGS__; \
    ofxImGuiAuto::Variant variants[] = {__VA_ARGS__}; \
    ofxImGuiAuto::DrawControlsVA(labels_str, variants); \
}();

/// @brief IMGUI_AUTOS2_NC(var_a, param1, param2, var_b, var_c, param1, ...)
#define IMGUI_AUTOS2_NC(...) [&](){ \
    const char* labels_str = #__VA_ARGS__; \
    ofxImGuiAuto::Variant variants[] = {__VA_ARGS__}; \
    ofxImGuiAuto::DrawControlsVA(labels_str, variants); \
}();

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

/// @brief IMGUI_AUTOS(var_a, var_b, var_c, ...)
#define IMGUI_AUTOS(...) IMGUI_KV_EXPAND(__VA_ARGS__)

#define IMGUI_AUTO_CHOOSER(_1,_2,_3,_4,_5,NAME,...) NAME
#define IMGUI_AUTO_1(name) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name)), #name);
#define IMGUI_AUTO_2(name, p1) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name), IMGUI_EXPAND(p1)), #name);
#define IMGUI_AUTO_3(name, p1, p2) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name), IMGUI_EXPAND(p1), IMGUI_EXPAND(p2)), #name);
#define IMGUI_AUTO_4(name, p1, p2, p3) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name), IMGUI_EXPAND(p1), IMGUI_EXPAND(p2), IMGUI_EXPAND(p3)), #name);
#define IMGUI_AUTO_5(name, p1, p2, p3, p4) ofxImGuiAuto::DrawControl(std::make_tuple(std::ref(name), IMGUI_EXPAND(p1), IMGUI_EXPAND(p2), IMGUI_EXPAND(p3), IMGUI_EXPAND(p4)), #name);
/// @brief IMGUI_AUTO(var_a) or IMGUI_AUTO(var_a, param1, param2, ...)
#define IMGUI_AUTO(...) IMGUI_EXPAND(IMGUI_AUTO_CHOOSER(__VA_ARGS__, IMGUI_AUTO_5, IMGUI_AUTO_4, IMGUI_AUTO_3, IMGUI_AUTO_2, IMGUI_AUTO_1)(__VA_ARGS__))

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

#define ENUM_(enum_value) enum_value, (ofxImGuiAuto::EnumNames(magic_enum::enum_names<decltype(enum_value)>())), (ofxImGuiAuto::EnumValues(magic_enum::enum_values<decltype(enum_value)>()))
