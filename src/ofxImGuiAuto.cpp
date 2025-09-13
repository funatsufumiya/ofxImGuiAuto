#include "ofxImGuiAuto.h"

inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::saved_time_left_map;
inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::loaded_time_left_map;
inline ofxImGuiAuto::EnumNames ofxImGuiAuto::temp_enum_names_rvalue;
inline ofxImGuiAuto::EnumValues ofxImGuiAuto::temp_enum_values_rvalue;