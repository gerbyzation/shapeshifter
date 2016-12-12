#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

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
        )
   );
   
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
        )
    );
    
    yThreshold.set(Settings::getFloat("yThreshold"));

    showMerged = false;
    showGrid = false;
    flattened.allocate(1000, 500);
    
    gui.setup();
    gui.add(threshold.set("threshold", Settings::getInt("threshold"), 0, 150));
    
    gui.add(minArea.set("minArea", Settings::getInt("minArea"), 0, 1000));
    gui.add(maxArea.set("maxArea", Settings::getInt("maxArea"), 0, 1000 * 500));
    gui.add(nConsidered.set("nConsidered", Settings::getInt("nConsidered"), 0, 500));
//    gui.add(xMin.set("X min", Settings::getFloat("xMin"), -300, 300));
//    gui.add(xMax.set("X max", Settings::getFloat("xMax"), 500, 1200));
//    gui.add(yMin.set("Y min", Settings::getFloat("yMin"), -300, 300));
//    gui.add(yMax.set("Y max", Settings::getFloat("yMax"), 200, 700));
//    gui.add(zMin.set("Z min", Settings::getFloat("zMin"), -300, 300));
//    gui.add(zMax.set("Z max", Settings::getFloat("zMax"), 0, 200));
}

//bool ofApp::inBoundaries(ofVec3f point) {
//    if (point.x < xMin) return false;
//    if (point.x > xMax) return false;
//    if (point.y < yMin) return false;
//    if (point.y > yMax) return false;
//    if (-point.z < zMin) return false;
//    if (-point.z > xMax) return false;
//    return true;
//}

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
        
        flattened.begin();
        ofEnableAlphaBlending();
        ofClear(0, 0, 0);
        
        ofMatrix4x4 mergedTransformation = mergedManipulator.getMatrix();
        auto vertices0 = mesh0.getVertices();
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        for (int i = 0; i < mesh0.getNumVertices(); i++) {
            ofVec3f point = vertices0[i];
            // add to merged pointcloud
            merged.addVertex(point);
            
            float minY = 50;
            // generate flattened image
            point = point * mergedTransformation;
//            if (inBoundaries(point)) {m
                ofSetColor(255, 255, 255, 5);
                ofFill();
                ofDrawCircle(point.x, -point.z, 2);
//            }
        };
        if (mesh1.hasVertices()) {
            auto vertices = mesh1.getVertices();
            ofMatrix4x4 transf = mesh1Manipulator.getMatrix();
            for (int i = 0 ; i < mesh1.getNumVertices(); i++) {
                ofVec3f vertex = vertices[i] * transf;
                merged.addVertex(vertex);
                
                ofVec3f point = vertex * mergedTransformation;
//                if(inBoundaries(point)) {
                    ofSetColor(255, 255, 255, 5);
                    ofFill();
                    ofDrawCircle(point.x, -point.z, 2);
//                }
            }
        }
        
        ofDisableAlphaBlending();
        ofDisableBlendMode();
        flattened.end();
        
        ofPixels pixels;
        ofPixels grayscalePixels;
        pixels.allocate(flattened.getWidth(), flattened.getHeight(), OF_IMAGE_COLOR_ALPHA);
        flattened.readToPixels(pixels);
        grayscalePixels.allocate(pixels.getWidth(), pixels.getHeight(), OF_IMAGE_GRAYSCALE);
        for (int x = 0; x < pixels.getWidth(); x++) {
            for(int y = 0; y < pixels.getHeight(); y++) {
                ofColor bright = pixels.getColor(x, y).getBrightness();
                grayscalePixels.setColor(x, y, bright);
            }
        }

        flatGray.clear();
        flatGray.setFromPixels(grayscalePixels);
        flatGray.threshold(threshold);
        contourFinder.findContours(flatGray, minArea, maxArea, nConsidered, false);
        if (contourFinder.nBlobs > 1) {
            for (int i = 0; i < contourFinder.nBlobs; i++) {
                ofxCvBlob blob = contourFinder.blobs[i];
                if (!mainBlob.area || mainBlob.area < blob.area) {
                    mainBlob = blob;
                }
            }
        } else if (contourFinder.nBlobs == 1) {
            mainBlob = contourFinder.blobs[0];
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableDepthTest();
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
                ofRotate(angle, axis.x, axis.y, axis.z);
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
        
        ecam.end();
        ofPopStyle();
        
    }
    if (showMerged) {
//        flatGray.draw(0, 0, 1000, 500);
        contourFinder.draw(0, 0, 1000, 500);
        ofPushStyle();
        ofSetColor(ofColor::red);
        ofFill();
        ofDrawCircle(mainBlob.centroid.x, mainBlob.centroid.y, 5);
        ofPopStyle();
    }

    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    ofDrawBitmapStringHighlight("Device Count : " + ofToString(ofxMultiKinectV2::getDeviceCount()), 10, 40);
    
    stringstream reportStr;
    reportStr << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), 20, 600);
    
    ofDisableDepthTest();
    gui.draw();
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
        
        Settings::getInt("threshold") = threshold;
        Settings::getInt("minArea") = minArea;
        Settings::getInt("maxArea") = maxArea;
        Settings::getInt("nConsidered") = nConsidered;
        
//        Settings::getFloat("xMin") = xMin;
//        Settings::getFloat("xMax") = xMax;
//        Settings::getFloat("yMin") = yMin;
//        Settings::getFloat("yMax") = yMax;
//        Settings::getFloat("zMin") = zMin;
//        Settings::getFloat("zMax") = zMax;

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
