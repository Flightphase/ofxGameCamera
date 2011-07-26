/*
 *  ofxGameCamera.cpp
 *
 *  Created by James George on 1/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxGameCamera.h"

static float ClampAngle (float angle, float min, float max)
{
	if (angle < -360.0f)
		angle += 360.f;
	if (angle > 360.0f)
		angle -= 360.0f;
	return ofClamp(angle, min, max);
}


ofxGameCamera::ofxGameCamera()
{
	sensitivityX = 0.15f;
	sensitivityY = 0.15f;
	
	minimumX = -360.0f;
	maximumX =  360.0f;
	
	minimumY = -60.0f;
	maximumY =  60.0f;
	
	rotationX = 0.0f;
	rotationY = 0.0f;

	speed = 1.0;
}

void ofxGameCamera::begin(ofRectangle rect)
{	
	//forward
	if(ofGetKeyPressed('w')){
		dolly(-speed);
	}
	
	if(ofGetKeyPressed('s')){
		dolly(speed);
	}
	
	if(ofGetKeyPressed('a')){
		truck(-speed);
	}
	
	if(ofGetKeyPressed('d')){
		truck(speed);
	}

	if(ofGetKeyPressed('c')){
		boom(-speed);
	}

	if(ofGetKeyPressed('e')){
		boom(speed);
	}
	
	ofVec2f mouse( ofGetMouseX(), ofGetMouseY() );
	
	if(ofGetMousePressed(0)){
		
		rotationX += (mouse.x - lastMouse.x) * sensitivityX;
		rotationX = ClampAngle(rotationX, minimumX, maximumX);
		
		rotationY += (mouse.y - lastMouse.y) * sensitivityY;
		rotationY = ClampAngle(rotationY, minimumY, maximumY);
		
		setOrientation(ofQuaternion()); //reset
		rotate(ofQuaternion(-rotationX, getYAxis()));
		rotate(ofQuaternion(-rotationY, getXAxis()));
	}
	
	lastMouse = mouse;
		
	ofCamera::begin(rect);
}

