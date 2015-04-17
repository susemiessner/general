#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(50, 50, 50);
	threshold=90;
	int maxNumberOfImages=20;
	
	// original video
	string letter="09";
	emptyvideo.loadMovie(letter+"_empty.mov");
	originalPixels= emptyvideo.getPixels(); //load pixels into pixels array
	//-----just for easier accessing
	w=video.width;
	h=video.height;
	
	//contrast video
	video.loadMovie(letter+".mov");
	video.play();
	video.setSpeed(4); //change video speed
	//video.setLoopState(OF_LOOP_NONE); //no looping
	
	differencePixels = new unsigned char [video.width*video.height*3];
	for (int i=0; i<w*h*3; i++) {
		differencePixels[i]=255;
	}
}

//--------------------------------------------------------------
void testApp::update(){
	contrastPixels=video.getPixels();
	//----difference image
	for (int i=0; i<video.width*video.height; i++) {
		for (int j=0; j<3; j++){
			int r1, g1, b1, r2, g2, b2;
			r1=originalPixels[i*3+0];
			g1=originalPixels[i*3+1];
			b1=originalPixels[i*3+2];
			r2=contrastPixels[i*3+0];
			g2=contrastPixels[i*3+1];
			b2=contrastPixels[i*3+2];
			if (abs((r2+g2+b2)-(r1+g1+b1))>threshold) {
				//-----the following lines are different ways of adding up to the endresult
				//-----1- just set it to the background
				//differencePixels[i*3+j]=originalPixels[i*3+j];
				//-----2-just set it to the current frame > you'll see the people
				differencePixels[i*3+j]=contrastPixels[i*3+j];
				//-----3 just another way of adding up
				//differencePixels[i*3+j]=abs(originalPixels[i*3+j]-contrastPixels[i*3+j]);/
			} 
		}
	}
	differencePic.setFromPixels(differencePixels, video.width, video.height, OF_IMAGE_COLOR);
	//-----saving every frame
	//differencePic.saveImage("pic"+ofToString(ofGetFrameNum(),0)+".png");	
}

//--------------------------------------------------------------
void testApp::draw(){ 
	ofPushMatrix();
	ofTranslate(20, 20);
	//-----two videos right
	//-----1
	video.draw(video.width+20,0,video.width/2,video.height/2);
	//-----2
	emptyvideo.draw(video.width+20,video.height/2,video.width/2,video.height/2);
	//-----the final video
	differencePic.draw(0, 0,video.width,video.height);
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key=='s') {
		differencePic.saveImage("pic"+ofToString(ofGetFrameNum(),0)+".png");
	}

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

