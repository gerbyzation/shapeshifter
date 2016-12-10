#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // load settings
    Settings::get().load("settings.json");

    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    kinect0.open(true, true, 0, 2);
    kinect1.open(true, true, 1, 2);
    // Note :
    // Default OpenCL device might not be optimal.
    // e.g. Intel HD Graphics will be chosen instead of GeForce.
    // To avoid it, specify OpenCL device index manually like following.
    // kinect1.open(true, true, 0, 2); // GeForce on MacBookPro Retina
    
    kinect0.start();
    kinect1.start();
    
    mesh0.setUsage(GL_DYNAMIC_DRAW);
    mesh0.setMode(OF_PRIMITIVE_POINTS);
    
    mesh1.setUsage(GL_DYNAMIC_DRAW);
    mesh1.setMode(OF_PRIMITIVE_POINTS);
    
    merged.setUsage(GL_DYNAMIC_DRAW);
    merged.setMode(OF_PRIMITIVE_POINTS);
    
    ecam.setAutoDistance(false);
    ecam.setDistance(200);
    camMouseInput = true;

    gizmo = ofxManipulator();
    gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
    gizmo.setTranslation(ofVec3f(
        Settings::getFloat("xTranslation"),
        Settings::getFloat("yTranslation"),
        Settings::getFloat("zTranslation")
    ));
    gizmo.setRotation(ofQuaternion(
       Settings::getFloat("angle"), ofVec3f(
       Settings::getFloat("xRotation"),
       Settings::getFloat("yRotation"),
       Settings::getFloat("zRotation"))
   ));
   
    showMerged = false;
    showGrid = false;
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect0.update();
    if (kinect0.isFrameNew()) {
        mesh0.clear();
        {
            int step = 2;
            int h = kinect0.getDepthPixelsRef().getHeight();
            int w = kinect0.getDepthPixelsRef().getWidth();
            for(int y = 0; y < h; y += step) {
                for(int x = 0; x < w; x += step) {
                    float dist = kinect0.getDistanceAt(x, y);
                    if(dist > 50 && dist < 500) {
                        ofVec3f pt = kinect0.getWorldCoordinateAt(x, y, dist);
                        
                        ofColor c(0, 0, 255);
                        mesh0.addColor(c);
                        mesh0.addVertex(pt);
                    }
                }
            }
            
        }
    }
    
    kinect1.update();
    if (kinect1.isFrameNew()) {
        mesh1.clear();
        {
            int step = 2;
            int h = kinect1.getDepthPixelsRef().getHeight();
            int w = kinect1.getDepthPixelsRef().getWidth();
            for(int y = 0; y < h; y += step) {
                for (int x = 0; x < w; x += step) {
                    float dist = kinect1.getDistanceAt(x, y);
                    if(dist > 50 && dist < 500) {
                        ofVec3f pt = kinect1.getWorldCoordinateAt(x, y, dist);
                        
                        ofColor c(255, 0, 0);
                        mesh1.addColor(c);
                        mesh1.addVertex(pt);
                    }
                }
            }
        }
    }
    
    if (showMerged) {
        merged.clear();
        merged.addVertices(mesh0.getVertices());
        if (mesh1.hasVertices()) {
            cout << "merging clouds" << endl;
//            merged.addVertices(mesh1.getVertices());
            auto vertices = mesh1.getVertices();
            ofMatrix4x4 transf = gizmo.getMatrix();
            for (int i = 0 ; i < mesh1.getNumVertices(); i++) {
                ofVec3f vertex = vertices[i] * transf;
                merged.addVertex(vertex);
            }
        }
        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    
    
    if (mesh0.getVertices().size() || mesh1.getVertices().size()) {
        ofPushStyle();
        glPointSize(2);
        ecam.begin();

        if (showGrid) {
            ofDrawGrid(100);
        } else {
            ofDrawAxis(100);
        }

        if (!showMerged) {
            ofPushMatrix();
            if (mesh0.getVertices().size()) {
                mesh0.draw();
            }
            if (mesh1.getVertices().size()) {
                ofScale(gizmo.getScale());
                ofRotateX(gizmo.getRotation().x());
                float angle;
                ofVec3f axis;
                auto quat = gizmo.getRotation();
                quat.getRotate(angle, axis);
                //            cout << typeid(quat).name() << endl;
                //            quat.getRotate(angle, axis);
                ofRotate(angle, axis.x, axis.y, axis.z);
                //            ofRotate(gizmo.getRotation().getEuler());
                ofTranslate(gizmo.getTranslation());
                mesh1.draw();
            }
            ofPopMatrix();
        } else {
            if (merged.getVertices().size()) {
                merged.draw();
            }
        }
        gizmo.draw(ecam);
        ecam.end();
        ofPopStyle();
        
        
    }
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("Device Count : " + ofToString(ofxMultiKinectV2::getDeviceCount()), 10, 40);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'c') {
        camMouseInput = !camMouseInput;
        if (camMouseInput) {
            ecam.enableMouseInput();
            gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
        } else if (!camMouseInput) {
            ecam.disableMouseInput();
            gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_TRANSLATION);
        }
    } else if (key == '1') {
        ecam.disableMouseInput();
        gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_TRANSLATION);
    } else if (key == '2') {
        ecam.disableMouseInput();
        gizmo.setManipulatorType(ofxManipulator::MANIPULATOR_ROTATION);
    } else if (key == 's') {
        Settings::getFloat("xTranslation") = gizmo.getTranslation().x;
        Settings::getFloat("yTranslation") = gizmo.getTranslation().y;
        Settings::getFloat("zTranslation") = gizmo.getTranslation().z;
        ofQuaternion rotation = gizmo.getRotation();
        float angle;
        ofVec3f axis;
        rotation.getRotate(angle, axis);
        Settings::getFloat("angle") = angle;
        Settings::getFloat("xRotation") = axis.x;
        Settings::getFloat("yRotation") = axis.y;
        Settings::getFloat("zRotation") = axis.z;
        Settings::get().save("settings.json");
    } else if (key == 'm') {
        showMerged = !showMerged;
    } else if (key == 'g' ) {
        showGrid = !showGrid;
    }

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
