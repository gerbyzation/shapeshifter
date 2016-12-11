#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    client.begin("192.168.0.2", 1883);
    client.connect("of", "try", "try");
    
    ofAddListener(client.onOnline, this, &ofApp::onOnline);
    ofAddListener(client.onOffline, this, &ofApp::onOffline);
    ofAddListener(client.onMessage, this, &ofApp::onMessage);
}

//--------------------------------------------------------------
void ofApp::update(){
    client.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

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

void ofApp::exit() {
    client.disconnect();
}

void ofApp::onOnline() {
    ofLog() << "online";
    
    client.subscribe("MOLEGYRO");
}

void ofApp::onOffline() {
    ofLog() << "offline";
}

void ofApp::onMessage(ofxMQTTMessage &msg) {
    ofLog() << "message: " << msg.topic << " - " << msg.payload << endl;
}
