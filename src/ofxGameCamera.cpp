/*
 *  ofxGameCamera.cpp
 *
 *  Created by James George on 1/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxGameCamera.h"

static float ClampAngle (float angle, float min, float max) {
	if (angle < -360.0f)
		angle += 360.f;
	if (angle > 360.0f)
		angle -= 360.0f;
	return ofClamp(angle, min, max);
}


ofxGameCamera::ofxGameCamera() {
	dampen = false;
	unsavedChanges = false;
	sensitivityX = 0.15f;
	sensitivityY = 0.15f;

	minimumX = -360.0f;
	maximumX =  360.0f;

	minimumY = -60.0f;
	maximumY =  60.0f;

	speed = 10.0f;
	
	lastMouse = ofVec2f(0,0);
	
	invertControls = false;
	usemouse = true;
	autosavePosition = false;
	useArrowKeys = false;

	applyRotation = true;
	applyTranslation = true;

	rollSpeed = 2;
	justResetAngles = false;

	
	cameraPositionFile =  "_gameCameraPosition.xml";
	
    eventsRegistered = false;
}
ofxGameCamera::~ofxGameCamera(){
    if(eventsRegistered){
        ofAddListener(ofEvents().update, this, &ofxGameCamera::update);
        ofAddListener(ofEvents().keyPressed, this, &ofxGameCamera::keyPressed);
        eventsRegistered = false;
    }
}

void ofxGameCamera::setup(){
    if(!eventsRegistered){
        ofAddListener(ofEvents().update, this, &ofxGameCamera::update);
        ofAddListener(ofEvents().keyPressed, this, &ofxGameCamera::keyPressed);
        eventsRegistered = true;
    }
}

void ofxGameCamera::update(ofEventArgs& args){
	bool rotationChanged = false;
	bool positionChanged = false;
	
	//lastRot == lastRot << this is nananers check
	if( (lastRot == lastRot && lastRot != getOrientationQuat()) ||
	    (lastPos == lastPos && lastPos != getPosition()))
	{
		movedManually();
	}

	if(applyTranslation){
		int multiplier = invertControls ? -1 : 1;
		if(ofGetKeyPressed('w') || (useArrowKeys && ofGetKeyPressed(OF_KEY_UP)) ){
			dolly(-speed);
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('s') || (useArrowKeys && ofGetKeyPressed(OF_KEY_DOWN)) ){
			dolly(speed);
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('a') || (useArrowKeys && ofGetKeyPressed(OF_KEY_LEFT)) ){
			truck(-speed);
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('d') || (useArrowKeys && ofGetKeyPressed(OF_KEY_RIGHT)) ){
			truck(speed);
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('c') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_DOWN)) ){
			boom(-speed*multiplier);
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('e') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_UP)) ){
			boom(speed*multiplier);
			positionChanged = true;
		}
	}
	

	if(positionChanged){
		currLookTarget = getPosition() + getLookAtDir();
	}


	ofVec2f mouse( ofGetMouseX(), ofGetMouseY() );
	if(!justResetAngles){
		if(usemouse && applyRotation && ofGetMousePressed(0)){

			float dx = (mouse.x - lastMouse.x) * sensitivityX;
			float dy = (mouse.y - lastMouse.y) * sensitivityY;

			currLookTarget.rotate(-dx, getPosition(), currentUp);
			ofVec3f sideVec = (currentUp).getCrossed(currLookTarget - getPosition());
			lookAt(currLookTarget, currentUp);

			currLookTarget.rotate(dy, getPosition(), sideVec);
			currentUp = ( currLookTarget - getPosition()  ).getCrossed(sideVec);
			lookAt(currLookTarget, currentUp);

			rotationChanged = true;

		}

		if(applyRotation){
			if(ofGetKeyPressed('r')){
				currentUp.rotate(rollSpeed,getLookAtDir());
				lookAt(currLookTarget, currentUp);
				rotationChanged = true;
			}
			if(ofGetKeyPressed('q')){
				currentUp.rotate(-rollSpeed,getLookAtDir());
				lookAt(currLookTarget, currentUp);
				rotationChanged = true;
			}
		}
	}

	lastMouse = mouse;
	justResetAngles = false;
	lastRot = getOrientationQuat();
	lastPos = getPosition();

	//did we make a change?
	unsavedChanges |= positionChanged || rotationChanged;
	
	//if we didnt make a change this frame, save. this helps not saving every frame when moving around
	if( !rotationChanged && !positionChanged && unsavedChanges && autosavePosition){
		saveCameraPosition();
	}
	/*
	 cout << "STATS:" << endl
	 << "	position" << getPosition() << endl
	 << "	current up vec " << currentUp << endl
	 << "	current look " << currLookTarget << endl
	 << "	vec to look " << (currLookTarget-getPosition()) << endl;
	 */
	
}

void ofxGameCamera::keyPressed(ofKeyEventArgs& args){
	
}

void ofxGameCamera::movedManually(){
	
	currLookTarget = getPosition() + getLookAtDir();
	currentUp = getUpDir();
	justResetAngles = true;
}

void ofxGameCamera::saveCameraPosition()
{
	ofxXmlSettings savePosition;
	savePosition.addTag("camera");
	savePosition.pushTag("camera");

	savePosition.addTag("position");
	savePosition.pushTag("position");
	savePosition.addValue("X", getPosition().x);
	savePosition.addValue("Y", getPosition().y);
	savePosition.addValue("Z", getPosition().z);
	savePosition.popTag(); //pop position

	savePosition.addTag("up");
	savePosition.pushTag("up");
	savePosition.addValue("X", currentUp.x);
	savePosition.addValue("Y", currentUp.y);
	savePosition.addValue("Z", currentUp.z);
	savePosition.popTag(); //pop up

	savePosition.addTag("look");
	savePosition.pushTag("look");
	savePosition.addValue("X", currLookTarget.x);
	savePosition.addValue("Y", currLookTarget.y);
	savePosition.addValue("Z", currLookTarget.z);
	savePosition.popTag(); //pop look
	
	savePosition.addValue("FOV", getFov());

	savePosition.popTag(); //camera;

	savePosition.saveFile(cameraPositionFile);
	
	unsavedChanges = false;
}

void ofxGameCamera::loadCameraPosition()
{
	ofxXmlSettings loadPosition;
	cout << "Camera pos " << cameraPositionFile << endl;
	if(loadPosition.loadFile(cameraPositionFile)){
//		reset();
//		loadPosition.pushTag("camera");
		cout << "loaded camera pos" << endl;
		// tig: defaulting with floats so as to get floats back.
		setPosition(ofVec3f(loadPosition.getValue("camera:position:X", 0.),
							loadPosition.getValue("camera:position:Y", 0.),
							loadPosition.getValue("camera:position:Z", 0.)));
		
		currentUp = ofVec3f(loadPosition.getValue("camera:up:X", 0.),
							loadPosition.getValue("camera:up:Y", 1.),
							loadPosition.getValue("camera:up:Z", 0.));

		currLookTarget = ofVec3f(loadPosition.getValue("camera:look:X", 0.),
								 loadPosition.getValue("camera:look:Y", 1.),
								 loadPosition.getValue("camera:look:Z", 0.));

		float fov = loadPosition.getValue("camera:FOV", -1.0);
		if(fov != -1.0){
			setFov(fov);
		}

		lookAt(currLookTarget, currentUp);
		movedManually();
		
	}
	else{
		ofLog(OF_LOG_ERROR, "ofxGameCamera: couldn't load position!");
	}

}

void ofxGameCamera::reset(){

	currentUp = ofVec3f(0,1,0);
	currLookTarget = ofVec3f(0,0,1);

    setPosition(ofVec3f(0,0,0));
    setOrientation(ofQuaternion());
	
	movedManually();
}
