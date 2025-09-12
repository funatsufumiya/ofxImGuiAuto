#pragma once

#include "ofMain.h"
#include "ofxImGuiAuto.h"

enum class MyChoise {
	A,
	B,
	C
};

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void save();
		void load();

		ofxImGui::Gui gui;

		ofVec2f size = ofVec2f(100, 100);
		ofColor color = ofColor::red;
		float angle;
		bool color_on = true;
		MyChoise choise;
};
