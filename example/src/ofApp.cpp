#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    gui.setup();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushStyle();
    ofPushMatrix();
    ofDrawBitmapString(magic_enum::enum_name(choise), 500, 100);
    if(color_on){
        ofSetColor(color);
    }
    ofTranslate(300, 100);
    ofRotateDeg(angle);
    ofDrawRectangle(0, 0, size.x, size.y);
    ofPopMatrix();
    ofPopStyle();

    gui.begin();

    ImGui::Begin("Test");

    IMGUI_AUTOS2(
        size, 1.0f, 10.0f, 1000.0f, "%.0f",
        angle, 0.1f,
        color,
        color_on,
    );
    IMGUI_AUTO(choise);
    IMGUI_AUTO_SAVE_LOAD(save(), load());

    ImGui::End();

    gui.end();
}

void ofApp::save(){
    ofLog() << "save() does nothing!";
}

void ofApp::load(){
    ofLog() << "load() does nothing!";
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
