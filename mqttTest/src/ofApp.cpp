#include "ofApp.h"

void ofApp::setup(){
    client.begin("broker.shiftr.io", 1883);
    client.connect("openframeworks", "try", "try");
    
    ofAddListener(client.onOnline, this, &ofApp::onOnline);
    ofAddListener(client.onOffline, this, &ofApp::onOffline);
    ofAddListener(client.onMessage, this, &ofApp::onMessage);
}

void ofApp::update() {
    client.update();
}

void ofApp::draw() {
//    ofRotateX(xRotation);
//    ofRotateY(yRotation);
//    ofRotateZ(zRotation);
    ofDrawCone(0, 0, 0, 50, 100);
}

void ofApp::exit(){
    client.disconnect();
}

void ofApp::onOnline(){
    ofLog() << "online";
    
    client.subscribe("MOLEGYRO");
}

void ofApp::onOffline(){
    ofLog() << "offline";
}

void ofApp::onMessage(ofxMQTTMessage &msg){
    ofLog() << "message: " << msg.topic << " - " << msg.payload;
}

