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
	
	invertControls = false;
	usemouse = true;
	autosavePosition = false;
	useArrowKeys = false;

	applyRotation = true;
	applyTranslation = true;

	rollSpeed = 2;
	justResetAngles = false;

	positionChanged = false;
	rotationChanged = false;

	currentUp = ofVec3f(0,1,0);
	currLookTarget = ofVec3f(0,0,-1);
	lookAt(currLookTarget, currentUp);
	cameraPositionFile =  "_gameCameraPosition.xml";
}

void ofxGameCamera::setup(){
	ofAddListener(ofEvents().update, this, &ofxGameCamera::update);
	ofAddListener(ofEvents().keyPressed, this, &ofxGameCamera::keyPressed);
}

void ofxGameCamera::update(ofEventArgs& args){	
	rotationChanged = false;
	positionChanged = false;
	if(applyTranslation){
		int multiplier = invertControls ? -1 : 1;
		if(ofGetKeyPressed('w') || (useArrowKeys && ofGetKeyPressed(OF_KEY_UP)) ){
			if(dampen){
				targetNode.dolly(-speed);
			}else{
				dolly(-speed);
			}
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('s') || (useArrowKeys && ofGetKeyPressed(OF_KEY_DOWN)) ){
			if(dampen){
				targetNode.dolly(speed);
			}else{
				dolly(speed);
			}
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('a') || (useArrowKeys && ofGetKeyPressed(OF_KEY_LEFT)) ){
			if(dampen){
				targetNode.truck(-speed);
			}else{
				truck(-speed);
			}
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('d') || (useArrowKeys && ofGetKeyPressed(OF_KEY_RIGHT)) ){
			if(dampen){
				targetNode.truck(speed);
			}
			else{
				truck(speed);
			}
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('c') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_DOWN)) ){
			if(dampen){
				targetNode.boom(-speed*multiplier);
			}
			else{
				boom(-speed*multiplier);
			}
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('e') || (useArrowKeys && ofGetKeyPressed(OF_KEY_PAGE_UP)) ){
			if(dampen){
				targetNode.boom(speed*multiplier);
			}
			else{
				boom(speed*multiplier);
			}
			positionChanged = true;
		}
	}
	

	if(positionChanged){
		currLookTarget = getPosition() + getLookAtDir();
	}


	if(dampen){
		//setPosition(getPosition() + (targetNode.getPosition() - getPosition()) *.2);
	}

	ofVec2f mouse( ofGetMouseX(), ofGetMouseY() );
	if(usemouse && applyRotation && ofGetMousePressed(0)){
        if(!justResetAngles){

			float dx = (mouse.x - lastMouse.x) * sensitivityX;
			float dy = (mouse.y - lastMouse.y) * sensitivityY;

//			cout << "b4 DX DY! " << dx << " " << dy << " " << targetXRot << " " << targetYRot << endl;
			//targetXRot += dx * (invertControls ? -1 : 1);
			//targetYRot += dy * (invertControls ? -1 : 1);

			currLookTarget.rotate(-dx, getPosition(), currentUp);
			ofVec3f sideVec = (currentUp).getCrossed(currLookTarget - getPosition());
			lookAt(currLookTarget, currentUp);

			currLookTarget.rotate(dy, getPosition(), sideVec);
			currentUp = ( currLookTarget - getPosition()  ).getCrossed(sideVec);
			lookAt(currLookTarget, currentUp);

//			targetYRot = ClampAngle(targetYRot, minimumY, maximumY);
//			targetXRot = ClampAngle(targetXRot, minimumX, maximumX);
//			cout << "after DX DY! " << dx << " " << dy << " " << targetXRot << " " << targetYRot << endl;
			rotationChanged = true;
		}
		justResetAngles = false;
	}

	if(applyRotation){
		if(ofGetKeyPressed('r')){
			currentUp.rotate(rollSpeed,getLookAtDir());
			lookAt(currLookTarget, currentUp);
			//targetZRot += rollSpeed;
			//rotationChanged = true;
		}
		if(ofGetKeyPressed('q')){
			currentUp.rotate(-rollSpeed,getLookAtDir());
			lookAt(currLookTarget, currentUp);
			//targetZRot -= rollSpeed;
			//rotationChanged = true;
		}
	}
	
	/*
	cout << "STATS:" << endl 
		 << "	position" << getPosition() << endl 
		 << "	current up vec " << currentUp << endl 
		 << "	current look " << currLookTarget << endl 
		 << "	vec to look " << (currLookTarget-getPosition()) << endl;
	 */

	if(rotationChanged){
		//updateRotation();		
	}
	
	lastMouse = mouse;
    
	if(!ofGetMousePressed(0) && autosavePosition && (rotationChanged || positionChanged)){
		saveCameraPosition();
	}
}

void ofxGameCamera::keyPressed(ofKeyEventArgs& args){
	
}

void ofxGameCamera::setAnglesFromOrientation(){
	ofVec3f rotation = getOrientationEuler();
	rotationX = targetXRot = -rotation.y;
	rotationY = targetYRot = -rotation.z;
	rotationZ = targetZRot = -rotation.x;
//	cout << "rotation is " << ofVec3f(rotationX,rotationY,rotationZ) << endl;;
	justResetAngles = true;
}

void ofxGameCamera::updateRotation(){
	
	if(!applyRotation) return;
	
//	cout << "update rotation!" << endl;
	if(dampen){
		rotationX += (targetXRot - rotationX) *.2;
		rotationY += (targetYRot - rotationY) *.2;
		rotationZ += (targetZRot - rotationZ) *.2;
	}
	else{
		rotationX = targetXRot;
		rotationY = targetYRot;
		rotationZ = targetZRot;
	}
	
	//ofMatrix4x4 m = ofMatrix4x4::makeRotationMatrix(rotationX,rotationY,rotationZ);
/*
	setOrientation(ofQuaternion(0,0,0,1)); //reset
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationZ, getZAxis()));
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationX, getYAxis()));
	setOrientation(getOrientationQuat() * ofQuaternion(-rotationY, getXAxis()));
	*/

	//targetNode.setOrientation(getOrientationQuat());
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

	savePosition.popTag(); //camera;

	savePosition.saveFile(cameraPositionFile);
}

void ofxGameCamera::loadCameraPosition()
{
	ofxXmlSettings loadPosition;
	if(loadPosition.loadFile(cameraPositionFile)){

		loadPosition.pushTag("camera");

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

		float fov = loadPosition.getValue("camera:FOV", -1);
		if(fov != -1){
			setFov(fov);
		}

		lookAt(currLookTarget, currentUp);

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
}
