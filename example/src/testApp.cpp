#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(30);
	for(int i = 0; i < 100; i++){
		ofNode n;
		n.setPosition(ofVec3f(ofRandom(-ofGetWidth(),ofGetWidth()), ofRandom(-ofGetHeight(), ofGetHeight()), ofRandom(-1000, 1000)));
		nodes.push_back(n);
	}
    
	camera.setup();
//	camera.autosavePosition = true;
//	camera.loadCameraPosition();	
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(0);
	
	camera.begin();
	
	for(int i = 0; i <  nodes.size(); i++){
		nodes[i].draw();
	}
	camera.end();
	
	ofSetColor(255);
	
	ofDrawBitmapString("camera controls: click and drag the mouse to look.\nw: forward, s: backwards\na: strafe left\nd: strafe right\ne: boom up\nc: boom down", ofPoint(30, 30));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
