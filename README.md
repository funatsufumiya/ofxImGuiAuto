# ofxImGuiAuto

This addon provides macros named `IMGUI_AUTO`, `IMGUI_AUTOS`, `IMGUI_AUTO_SAVE_LOAD` that automatically generate ImGui DragXXX controls and Save/Load buttons.

C++17 or higher is needed.

Tested on oF v0.12.0 (win/mac)

## Features

- Automatically generates ImGui (Drag) controls for:
    - `float`
    - `int`
    - `ofVec2f`
    - `ofVec3f`
    - `ofRectangle`
    - `bool` (as Checkbox)
    - `enum` (as Combo)

### Example

```cpp
class MyApp : public ofBaseApp {
public:
    ofVec2f position;
    ofVec2f size;
    float scale;
    ofVec2f offset;
    ofVec2f baseSize;
    float speed;
    float fadeIn;
    float duration;
    float fadeOut;
    bool enabled;

    void setup() {
        position = ofVec2f(0, 0);
        size = ofVec2f(100, 70);
        scale = 1.0f;
        offset = ofVec2f(10, 10);
        baseSize = ofVec2f(50, 50);
        speed = 1.0f;
        fadeIn = 0.5f;
        duration = 1.0f;
        fadeOut = 0.5f;
        enabled = true;
        load();
    }

    void save(){
        // something to save
    }

    void load(){
        // something to load
    }

    void drawGui() {
        ImGui::Begin("My Controls");
        IMGUI_AUTOS(
            position,
            size,
            scale,
            offset,
            baseSize,
            enabled
        );
        IMGUI_AUTO(speed, 0.1f);
        IMGUI_AUTO(fadeIn, 0.1f);
        IMGUI_AUTO(duration, 0.1f);
        IMGUI_AUTO(fadeOut, 0.1f);
        IMGUI_AUTO_SAVE_LOAD(save(), load(), "save", "load");
        ImGui::End();
    }
};
```

## License

0BSD or WTFPL

(Please note that [magic_enum](https://github.com/Neargye/magic_enum) is [MIT License](https://github.com/Neargye/magic_enum/blob/master/LICENSE).)
