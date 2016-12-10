#pragma once

#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxGui.h"
#include "ofxManipulator.h"
#include "ofxJSONSettings.h"

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

    ofxManipulator gizmo;
};
