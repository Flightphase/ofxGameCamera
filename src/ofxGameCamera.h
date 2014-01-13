/*
 *  ofxGameCamera.h
 *
 *  Created by James George on 1/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 *  GameCamera is the official camera of RGBDToolkit http://www.rgbdtoolkit.com
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofxGameCamera : public ofCamera {
  public:
	
	ofxGameCamera();
    ~ofxGameCamera();
    
	void setup();
    void update(ofEventArgs& args);
    void keyPressed(ofKeyEventArgs& args);
	
	//tweak controls
	bool invertControls;
	float sensitivityX;
	float sensitivityY;
	float rollSpeed;
	float speed;
	
	//enable, disable
	bool usemouse;
	bool useArrowKeys;
	bool applyRotation;
	bool applyTranslation;
	
	//saving loading
	bool autosavePosition;
	void saveCameraPosition();
	void loadCameraPosition();

	//adjusting threw code manually
	void movedManually(); //call this whenever you update orientation or position manually
    void reset(); //sets the camera back to zero
	
	//NO LONGER WORK
	float minimumX;
	float maximumX;
	
	float minimumY;
	float maximumY;
	
	bool dampen;
	//GOTTA REIMPLEMENT
protected:
    
	bool eventsRegistered;
    
	ofVec3f currentUp;
	ofVec3f currLookTarget;
	
	ofQuaternion lastRot;
	ofVec3f lastPos;

	void updateRotation();

	ofVec2f lastMouse;
	bool justResetAngles;
	bool unsavedChanges;
	
	string cameraPositionFile;
	
};
