#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    fbo.allocate(400, 400, GL_RGBA);
}

//--------------------------------------------------------------
void ofApp::update(){
    fbo.begin();
    ofClear(0, 0, 0, 0);
    ofSetColor(ofColor::red);
    ofFill();
    ofDrawCircle(0, 0, 100);
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
