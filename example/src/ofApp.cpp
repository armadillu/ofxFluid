#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofEnableAlphaBlending();
	ofSetCircleResolution(100);

	TIME_SAMPLE_SET_FRAMERATE(60.0);
	width = 1200;
	height = 600;

	// Initial Allocation
	//
	fluid.allocate(width, height, 0.5);

	// Seting the gravity set up & injecting the background image
	//
	fluid.dissipation = 0.99;
	fluid.velocityDissipation = 0.99;

	fluid.setGravity(ofVec2f(0.0,0.0));
	fluid.setGravity(ofVec2f(0.0,-0.00098));

	//	Set obstacle
	//
	fluid.setUseObstacles(true);

	RUI_SETUP();
	RUI_SHARE_PARAM(fluid.dissipation,0.96, 1);
	RUI_SHARE_PARAM(fluid.velocityDissipation, 0.96, 1);
	RUI_SHARE_PARAM(fluid.temperatureDissipation, 0.96, 1);
	RUI_SHARE_PARAM(fluid.pressureDissipation, 0.96, 1);

	RUI_NEW_GROUP("internal");
	RUI_SHARE_PARAM(fluid.timeStep, 0.01, 0.5); //0.125
	RUI_SHARE_PARAM(fluid.smokeBuoyancy,0.0, 2.0);
	RUI_SHARE_PARAM(fluid.smokeWeight, 0.0, 0.5);
	RUI_SHARE_PARAM(fluid.numJacobiIterations, 1, 50);

	RUI_NEW_GROUP("more");
	RUI_SHARE_COLOR_PARAM(paint);
	RUI_SHARE_PARAM(angle, 0, 360);
	RUI_SHARE_PARAM(vel, 0, 100);
	RUI_SHARE_PARAM(radius, 0, 20);
	RUI_SHARE_PARAM(temperature, -50, 50);
	RUI_SHARE_PARAM(density, 0, 1);


	RUI_LOAD_FROM_XML();


	float v = 1.2;
	// Adding constant forces
	//
	for(int i = 0; i < 10; i++){
		fluid.addConstantForce(ofPoint(width*0.1, 0.05 * height + i * height * 0.1), //pos
							   ofPoint(v,0), //vel
							   ofFloatColor(0.5,0.1,0.0), //color
							   3.f //rad
							   );
	}


	ofSetWindowShape(width, height);
}

//--------------------------------------------------------------
void testApp::update(){

	//obstacles test
	if(ofGetMousePressed(0)){
		fluid.begin();
		ofClear(0);
		ofSetColor(255);
		ofCircle(mouseX, mouseY, 30);
		fluid.end();
	}

	// Adding temporal Force
	//
	if(ofGetMousePressed(1)){
		ofPoint m = ofPoint(mouseX,mouseY);
		ofPoint d = (m - oldM)*10.0;
		oldM = m;
		ofPoint c = ofPoint(640*0.5, 480*0.5) - m;
		c.normalize();
		fluid.addTemporalForce(m, d, ofFloatColor(c.x,c.y,0.5) * sin(ofGetElapsedTimef()),10.0f);
	}

	ofVec2f velv = vel * ofVec2f(cosf(M_PI + angle * DEG_TO_RAD), sinf(M_PI + angle * DEG_TO_RAD));
	fluid.addTemporalForce(ofGetWindowRect().getCenter() , //pos;
						   velv, //vel
						   paint,// * (0.5 + 0.5 * sinf(10 * ofGetElapsedTimef())), //col
						   radius, //radius
						   temperature, //temp
							density
						   );

	//	Update
	//
	fluid.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);

	fluid.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if( key == 'p')
		bPaint = !bPaint;
	if( key == 'o')
		bObstacle = !bObstacle;
	if( key == 'b')
		bBounding = !bBounding;
	if( key == 'c')
		bClear = !bClear;
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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
	
}