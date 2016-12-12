#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxManipulator.h"
#include "ofxJSONSettings.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    ofxMultiKinectV2 kinect0;
    ofxMultiKinectV2 kinect1;
    ofEasyCam ecam;
    bool camMouseInput;
    ofVboMesh mesh0;
    ofVboMesh mesh1;
    ofVboMesh merged;
    bool showMerged;
    
    bool showGrid;

    ofxManipulator mesh1Manipulator;
    ofxManipulator mergedManipulator;
    ofxManipulator::MANIPULATOR_TYPE previous;

    ofParameter<float> yThreshold;
    ofFbo flattened;
    
    ofxCvGrayscaleImage flatGray;
    
    ofxCv::ContourFinder contourFinder2;
    ofxCvContourFinder contourFinder;
    
    ofxPanel gui;
    
    ofParameter<int> threshold;
    
    ofParameter<int> minArea;
    ofParameter<int> maxArea;
    ofParameter<int> nConsidered;
    
    ofParameter<float> yMin;
    ofParameter<float> yMax;
    ofParameter<float> xMin;
    ofParameter<float> xMax;
    ofParameter<float> zMin;
    ofParameter<float> zMax;
    ofxCvBlob mainBlob;
    
    ofFbo route;
    void drawRoute(int height);
//
    bool inBoundaries(ofVec3f);
};
