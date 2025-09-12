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
    if(flag){
        ofSetColor(255, 0, 0);
    }
    ofTranslate(100, 100);
    ofRotateDeg(angle);
    ofDrawRectangle(0, 0, size.x, size.y);
    // ofDrawRectangle(100, 100, size.x, size.x);
    ofPopMatrix();
    ofPopStyle();

    gui.begin();

    ImGui::Begin("Test");

    IMGUI_AUTOS2(
        size,
        angle, 0.1f,
        flag
    );

    IMGUI_AUTO(choise);

    ImGui::End();

    gui.end();
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
