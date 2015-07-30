#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofEnableAlphaBlending();
	ofSetCircleResolution(100);

	TIME_SAMPLE_SET_FRAMERATE(60.0);
	width = 1200;
	height = 600;

	float scale = 0.5;
	// Initial Allocation
	//
	fluid.allocate(width, height, scale);

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


	fboReader = new ofxFastFboReader(4);
	fboReader->setAsync(true);

	//float fboScaledown = 1;
	//smallerVelFbo.allocate(width * scale * fboScaledown, height * scale * fboScaledown, GL_RGB32F);

	ofSetWindowShape(width, height);
}

//--------------------------------------------------------------
void testApp::update(){

	//obstacles test
	if(ofGetMousePressed(0)){
		fluid.begin(0, false);
			int wall = 5;
			//ofClear(0);
			ofSetColor(255);
			ofRect(0,0, fluid.getWidth(), wall);
			ofRect(0,fluid.getHeight() - wall, fluid.getWidth(), wall);
			ofRect(0,0, wall, fluid.getHeight());
			ofRect(fluid.getWidth() - wall,0, wall, fluid.getHeight());
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

	ofFloatImage velImg;
	ofFloatPixels velBuffer;

	ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);

	fluid.draw();
	fluid.getObstaclesFbo().draw(0,0,100,100);

//	smallerVelFbo.begin();
//	fluid.getVelocityBuffer().draw(0,0, smallerVelFbo.getWidth(), smallerVelFbo.getHeight());
//	smallerVelFbo.end();

	TS_START("getVelBufferPBO");
	fboReader->readToFloatPixels(fluid.getVelocityBuffer(), velBuffer);
	TS_STOP("getVelBufferPBO");

	TS_START("setFromPix");
	velImg.setFromPixels(velBuffer);
	velImg.draw(200, 0,100,100);
	TS_STOP("setFromPix");

//	int x = ofMap(mouseX, 0, ofGetWidth(), 0, 599, true);
//	int y = ofMap(mouseY, 0, ofGetHeight(), 0, 299, true);
//
//	float xx = velImg.getPixelsRef()[y * 600 * 3 + x * 3]; 		//R
//	float yy = velImg.getPixelsRef()[y * 600 * 3 + x * 3 + 1];	//G
//	float zz = velImg.getPixelsRef()[y * 600 * 3 + x * 3 + 2];	//G
//	cout << xx << ", " << yy << ", " << zz << endl;

	TS_START("drawVectorMesh");
	ofMesh lines;
	lines.setMode(OF_PRIMITIVE_LINES);
	for(int i = 0; i < 600; i += 10){
		for(int j = 0; j < 300; j += 10){
			float xx = 5 * velBuffer.getData()[j * 600 * 3 + i * 3]; 		//R
			float yy = 5 * velBuffer.getData()[j * 600 * 3 + i * 3 + 1];	//G
			lines.addVertex(ofVec2f(i,j));
			lines.addVertex(ofVec2f(i + xx, j + yy));
			lines.addColor(ofColor::white);
			lines.addColor(ofColor::black);
		}
	}
	ofPushMatrix();
	ofScale(2,2,2);
	lines.draw();
	ofPopMatrix();
	TS_STOP("drawVectorMesh");



	TS_START("saveToHDR");
	velImg.save("a.HDR");
	TS_STOP("saveToHDR");



//	TS_START("getVelBuffer");
//	//fluid.getVelocityBuffer(velBuffer);
//	fluid.getVelocityBuffer().readToPixels(velBuffer);
//	velImg.setFromPixels(velBuffer);
//	velImg.draw(100, 0,100,100);
//	TS_STOP("getVelBuffer");

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