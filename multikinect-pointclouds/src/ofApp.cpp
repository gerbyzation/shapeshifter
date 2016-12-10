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

    mesh1Manipulator = ofxManipulator();
    mesh1Manipulator.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
    mesh1Manipulator.setTranslation(ofVec3f(
        Settings::getFloat("mesh/xTranslation"),
        Settings::getFloat("mesh/yTranslation"),
        Settings::getFloat("mesh/zTranslation")
    ));
    mesh1Manipulator.setRotation(ofQuaternion(
       Settings::getFloat("mesh/angle"), ofVec3f(
       Settings::getFloat("mesh/xRotation"),
       Settings::getFloat("mesh/yRotation"),
       Settings::getFloat("mesh/zRotation"))
   ));
   
    mergedManipulator = ofxManipulator();
    mergedManipulator.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
    mergedManipulator.setTranslation(ofVec3f(
        Settings::getFloat("merged/xTranslation"),
        Settings::getFloat("merged/yTranslation"),
        Settings::getFloat("merged/zTranslation")
    ));
    mergedManipulator.setRotation(ofQuaternion(
    Settings::getFloat("merged/angle"), ofVec3f(
        Settings::getFloat("merged/xRotation"),
        Settings::getFloat("merged/yRotation"),
        Settings::getFloat("merged/zRotation"))
    ));
    
    yThreshold.set(Settings::getFloat("yThreshold"));

    showMerged = false;
    showGrid = false;

//    slider = new ofxDatGuiSlider(yThreshold.set("Y Threshold", 500));
//    slider->setWidth(ofGetWidth() * .2, .2);
//    slider->setPosition(10, 50);
    
}

//--------------------------------------------------------------
void ofApp::update(){

//    slider->update();
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
                        ofMatrix4x4 corr = ofMatrix4x4().newScaleMatrix(-1.0, 1.0, -1.0);
                        ofVec3f pt = kinect0.getWorldCoordinateAt(x, y, dist) * corr;
                        if (pt.y < yThreshold) {
                            ofColor c(0, 0, 255);
                            mesh0.addColor(c);
                            mesh0.addVertex(pt);
                        }
                       
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
                        ofMatrix4x4 corr = ofMatrix4x4().newScaleMatrix(-1.0, 1.0, -1.0);
                        ofVec3f pt = kinect1.getWorldCoordinateAt(x, y, dist) * corr;
                        
                        if (pt.y < yThreshold) {
                            ofColor c(255, 0, 0);
                            mesh1.addColor(c);
                            mesh1.addVertex(pt);
                        }
                    }
                }
            }
        }
    }
    
    if (showMerged) {
        merged.clear();
        merged.addVertices(mesh0.getVertices());
        if (mesh1.hasVertices()) {
            auto vertices = mesh1.getVertices();
            ofMatrix4x4 transf = mesh1Manipulator.getMatrix();
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
            
            if (mesh0.getVertices().size()) {
                mesh0.draw();
            }
            if (mesh1.getVertices().size()) {
                ofPushMatrix();
                ofScale(mesh1Manipulator.getScale());
                float angle;
                ofVec3f axis;
                auto quat = mesh1Manipulator.getRotation();
                quat.getRotate(angle, axis);
                //            cout << typeid(quat).name() << endl;
                //            quat.getRotate(angle, axis);
                ofRotate(angle, axis.x, axis.y, axis.z);
                //            ofRotate(mesh1Manipulator.getRotation().getEuler());
                ofTranslate(mesh1Manipulator.getTranslation());
                mesh1.draw();
                ofPopMatrix();
                mesh1Manipulator.draw(ecam);
            }
            
        } else {
            if (merged.getVertices().size()) {
                ofPushMatrix();
                ofScale(mergedManipulator.getScale());
                float angle;
                ofVec3f axis;
                ofQuaternion quaternate = mergedManipulator.getRotation();
                quaternate.getRotate(angle, axis);
                ofRotate(angle, axis.x, axis.y, axis.z);
                ofTranslate(mergedManipulator.getTranslation());
                merged.draw();
                ofPopMatrix();
                mergedManipulator.draw(ecam);
            }
        }
        ofSpherePrimitive sphere = ofSpherePrimitive();
        sphere.setRadius(50);
        sphere.setPosition(100, 100, 100);
        sphere.draw();
        
        ecam.end();
        ofPopStyle();
    }
    
    
    
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("Device Count : " + ofToString(ofxMultiKinectV2::getDeviceCount()), 10, 40);
    
//    slider->draw();

    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'c') {
        camMouseInput = !camMouseInput;
        if (camMouseInput) {
            ecam.enableMouseInput();
            mesh1Manipulator.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
            mergedManipulator.setManipulatorType(ofxManipulator::MANIPULATOR_NONE);
        } else if (!camMouseInput) {
            ecam.disableMouseInput();
            mesh1Manipulator.setManipulatorType(previous);
            mergedManipulator.setManipulatorType(previous);
        }
    } else if (key == '1') {
        ecam.disableMouseInput();
        mesh1Manipulator.setManipulatorType(ofxManipulator::MANIPULATOR_TRANSLATION);
        mergedManipulator.setManipulatorType(ofxManipulator::MANIPULATOR_TRANSLATION);
        previous = ofxManipulator::MANIPULATOR_TRANSLATION;
    } else if (key == '2') {
        ecam.disableMouseInput();
        mesh1Manipulator.setManipulatorType(ofxManipulator::MANIPULATOR_ROTATION);
        mergedManipulator.setManipulatorType(ofxManipulator::MANIPULATOR_ROTATION);
        previous = ofxManipulator::MANIPULATOR_ROTATION;
    } else if (key == 's') {
        // Save transformation for mesh1
        Settings::getFloat("mesh/xTranslation") = mesh1Manipulator.getTranslation().x;
        Settings::getFloat("mesh/yTranslation") = mesh1Manipulator.getTranslation().y;
        Settings::getFloat("mesh/zTranslation") = mesh1Manipulator.getTranslation().z;
        ofQuaternion rotation = mesh1Manipulator.getRotation();
        float angle;
        ofVec3f axis;
        rotation.getRotate(angle, axis);
        Settings::getFloat("mesh/angle") = angle;
        Settings::getFloat("mesh/xRotation") = axis.x;
        Settings::getFloat("mesh/yRotation") = axis.y;
        Settings::getFloat("mesh/zRotation") = axis.z;
        
        // Save transformation for merged mesh
        Settings::getFloat("merged/xTranslation") = mergedManipulator.getTranslation().x;
        Settings::getFloat("merged/yTranslation") = mergedManipulator.getTranslation().y;
        Settings::getFloat("merged/zTranslation") = mergedManipulator.getTranslation().z;
        
        ofQuaternion rotationMerged = mergedManipulator.getRotation();
        float angleMerged;
        ofVec3f axisMerged;
        rotationMerged.getRotate(angleMerged, axisMerged);
        Settings::getFloat("merged/angle") = angleMerged;
        Settings::getFloat("merged/xRotation") = axisMerged.x;
        Settings::getFloat("merged/yRotation") = axisMerged.y;
        Settings::getFloat("merged/zRotation") = axisMerged.z;
        
        Settings::getFloat("yThreshold") = yThreshold;

        // save to file
        Settings::get().save("settings.json");
    } else if (key == 'm') {
        showMerged = !showMerged;
    } else if (key == 'g' ) {
        showGrid = !showGrid;
    } else if (key == '+') {
        yThreshold += 10.0;
    } else if (key == '-') {
        yThreshold -= 10;
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
