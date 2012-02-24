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
	
	sensitivityX = 0.15f;
	sensitivityY = 0.15f;

	minimumX = -360.0f;
	maximumX =  360.0f;

	minimumY = -60.0f;
	maximumY =  60.0f;

	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;

	targetXRot = 0.0f;
	targetYRot = 0.0f;
	targetZRot = 0.0f;
	
	speed = 10.0f;
	
	lastMouse = ofVec2f(0,0);

	usemouse = true;
	autosavePosition = false;
	useArrowKeys = true;

	applyRotation = true;
	applyTranslation = true;

	rollSpeed = 2;


	cameraPositionFile =  "_gameCameraPosition.xml";
}

void ofxGameCamera::begin(ofRectangle rect) { 
	ofVec3f startPos = getPosition();
	ofVec2f startRot = ofVec3f(rotationX, rotationY, rotationZ);

	//forward
    if(applyTranslation){
        if(ofGetKeyPressed('w') || (useArrowKeys && ofGetKeyPressed(OF_KEY_UP)) ){
            targetNode.dolly(-speed);
			//dolly(-speed);
        }

        if(ofGetKeyPressed('s') || (useArrowKeys && ofGetKeyPressed(OF_KEY_DOWN)) ){
            targetNode.dolly(speed);
			//dolly(speed);
        }

        if(ofGetKeyPressed('a') || (useArrowKeys && ofGetKeyPressed(OF_KEY_LEFT)) ){
            targetNode.truck(-speed);
			//truck(-speed);
        }

        if(ofGetKeyPressed('d') || (useArrowKeys && ofGetKeyPressed(OF_KEY_RIGHT)) ){
            targetNode.truck(speed);
			//truck(speed);
        }

        if(ofGetKeyPressed('c') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_DOWN)) ){
            targetNode.boom(-speed);
			//boom(-speed);
        }

        if(ofGetKeyPressed('e') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_UP)) ){
            targetNode.boom(speed);
        }
		
		//TODO make variable
		setPosition(getPosition() + (targetNode.getPosition() - getPosition()) *.2);
	}
	
	if(applyRotation){	
		if(ofGetKeyPressed('r')){
			targetZRot += rollSpeed;
//			updateRotation();		
        }
		if(ofGetKeyPressed('q')){
			targetZRot -= rollSpeed;
//			updateRotation();		
		}
	}
	

	ofVec2f mouse( ofGetMouseX(), ofGetMouseY() );
	if(usemouse && applyRotation && ofGetMousePressed(0)){
				
		float dx = (mouse.x - lastMouse.x) * sensitivityX;
		float dy = (mouse.y - lastMouse.y) * sensitivityY;
		targetXRot += dx;
		targetYRot += dy;
		
		targetYRot = ClampAngle(targetYRot, minimumY, maximumY);
		targetXRot = ClampAngle(targetXRot, minimumX, maximumX);
	}
	
	if(applyRotation){
		updateRotation();		
	}
	
	lastMouse = mouse;

	if(!ofGetMousePressed(0) && autosavePosition && (startPos != getPosition() || startRot != ofVec3f(rotationX, rotationY, rotationZ))){
		saveCameraPosition();
	}

	ofCamera::begin(rect);
}

void ofxGameCamera::setAnglesFromOrientation(){
	ofVec3f rotation = getOrientationEuler();
	rotationX = targetXRot = -rotation.y;
	rotationY = targetYRot = -rotation.z;
	rotationZ = targetZRot = -rotation.x;
}

void ofxGameCamera::updateRotation()
{
	rotationX += (targetXRot - rotationX) *.2;
	rotationY += (targetYRot - rotationY) *.2;
	rotationZ += (targetZRot - rotationZ) *.2;
	
	setOrientation(ofQuaternion(0,0,0,1)); //reset	
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationZ, getZAxis()));
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationX, getYAxis()));
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationY, getXAxis()));
		
	targetNode.setOrientation(getOrientationQuat());
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

	savePosition.addTag("rotation");
	savePosition.pushTag("rotation");

	savePosition.addValue("X", rotationX);
	savePosition.addValue("Y", rotationY);
	savePosition.addValue("Z", rotationZ);

	savePosition.popTag(); //pop rotation

	savePosition.popTag(); //camera;

	savePosition.saveFile(cameraPositionFile);
}

void ofxGameCamera::loadCameraPosition()
{
	ofxXmlSettings loadPosition;
	if(loadPosition.loadFile(cameraPositionFile)){

		loadPosition.pushTag("camera");
		loadPosition.pushTag("position");
		// tig: defaulting with floats so as to get floats back.
		setPosition(ofVec3f(loadPosition.getValue("X", 0.),
							loadPosition.getValue("Y", 0.),
							loadPosition.getValue("Z", 0.)));
		targetNode.setPosition(getPosition());
		
		loadPosition.popTag();

		loadPosition.pushTag("rotation");
		targetXRot = rotationX = loadPosition.getValue("X", 0.);
		targetYRot = rotationY = loadPosition.getValue("Y", 0.);
		targetZRot = rotationZ = loadPosition.getValue("Z", 0.);
		float fov = loadPosition.getValue("FOV", -1);
		if(fov != -1){
			setFov(fov);
		}
		
		loadPosition.popTag();

		loadPosition.popTag(); //pop camera;

		updateRotation();
	}
	else{
		ofLog(OF_LOG_ERROR, "ofxGameCamera: couldn't load position!");
	}

}

