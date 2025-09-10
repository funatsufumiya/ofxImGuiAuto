# ofxImGuiAuto

This addon provides macros named `IMGUI_AUTO`, `IMGUI_AUTOS`, `IMGUI_AUTO_SAVE_LOAD` that automatically generate ImGui DragXXX controls and Save/Load buttons.

Tested on oF v0.12.0 (win/mac)

## Features

- Automatically generates ImGui Drag controls for:
  - `float`
  - `int`
  - `ofVec2f`
  - `ofVec3f`
- Supports additional arguments using tuples.
- Provides Save and Load buttons for easy data management.

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
            baseSize
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
