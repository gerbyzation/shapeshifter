#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    cone = ofConePrimitive(10, 100, 10, 10);
    
    client.begin("192.168.0.02", 1883);
    client.connect("openframeworks", "try", "try");
    
//    ofAddListener(client.onOnline, this, &ofApp::onOnline);
//    ofAddListener(client.onOffline, this, &ofApp::onOffline);
//    ofAddListener(client.onMessage, this, &ofApp::onMessage);
}

//--------------------------------------------------------------
void ofApp::update(){
    client.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
//    ofRotateX(<#float degrees#>)
//    ofRotateY()
//    ofRotateZ();
    cone.drawWireframe();
    cam.end();
}

void ofApp::exit() {
    client.disconnect();
}

void ofApp::onOnline() {
    ofLog() << "online";
    
    client.subscribe("MOLEGYROX");
    client.subscribe("MOLEGYROY");
    client.subscribe("MOLEGYROZ");
}

void ofApp::onOffline() {
    ofLog() << "offline";
}

void ofApp::onMessage(ofxMQTTMessage &msg) {
    ofLog() << "message: " << msg.topic << " - " << msg.payload;
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
