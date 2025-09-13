#include "ofxImGuiAuto.h"

inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::saved_time_left_map;
inline std::map<ImGuiID, float> ofxImGuiAuto::SaveLoadButton::loaded_time_left_map;
inline std::map<std::string, ofxImGuiAuto::EnumNames> ofxImGuiAuto::temp_enum_names_rvalues;
inline std::map<std::string, ofxImGuiAuto::EnumValues> ofxImGuiAuto::temp_enum_values_rvalues;
