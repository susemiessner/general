#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(50, 50, 50);
	ofEnableAlphaBlending();
	
	//---------videoplayer
	string letter="09";
	video.loadMovie(letter+".mov");
	video.play();
	//video.setSpeed(2); //settin the speed up if you don't have the patiance to wait 
	//video.setLoopState(OF_LOOP_NONE); //no looping
	
	emptyvideo.loadMovie(letter+"_empty.mov");
	
	//----------original image
	originalTexture.allocate(video.width,video.height,GL_RGB);
	originalPixels= new unsigned char [video.width*video.height*3];
	
	originalPixels=emptyvideo.getPixels();
	
	//just for easier access here
	w=video.width;
	h=video.height;
	
	//-----------current image
	vidTexture.allocate(video.width,video.height,GL_RGB);
	vidPixels=new unsigned char [video.width*video.height,GL_RGB];
	
	//---------------openCv
	colorImage.allocate(w,h);
	grayImage.allocate(w,h);
	backgroundColorImage.allocate(w, h);
	grayBackground.allocate(w,h);
	grayDifference.allocate(w,h);
	
	bLearnBackground=true;
	threshold=30;
	minBlobSize=370;//270;
	
	//-----saving images
	screen.allocate(w, h, OF_IMAGE_COLOR);  
	filecounter=0;
}

//--------------------------------------------------------------
void testApp::update(){
	//-----
	vidPixels=video.getPixels();
	//-----
	//1. color image
	colorImage.setFromPixels(vidPixels,w,h);
	//2. gray image
	grayImage = colorImage;
	//3. background
	backgroundColorImage.setFromPixels(originalPixels,w,h);
	grayBackground=backgroundColorImage;
	//4. difference
	grayDifference.absDiff(grayImage, grayBackground);
	grayDifference.threshold(threshold);
	//grayDifference.blur(5);
	
	//5. contours, blobs
	contourFinder.findContours(grayDifference, minBlobSize,grayDifference.width*grayDifference.height/3, 10, blobInBlob);
	
		
}

//--------------------------------------------------------------
void testApp::draw(){
	ofPushMatrix();
	ofTranslate(20, 20);
	//for black rectangle > output image
	ofSetColor(0, 0, 0);
	ofRect(0, 0, w, h);
	ofSetColor(200, 200, 200);
	//the 4 small images on the right
	//----1
	grayImage.draw(w+20,0, w/4,h/4); 
	//----2
	grayBackground.draw(w+20,h/4, w/4,h/4); //
	//----3
	grayDifference.draw(w+20,h/2, w/4,h/4);//
	contourFinder.draw(w+20,h/2,w/4,h/4); //
	//----4
	ofSetColor(0, 0, 0);
	ofRect(w+20, 3*h/4, w/4, h/4);
	ofSetColor(200, 200, 200);
	for (int i=0; i<contourFinder.blobs.size(); i++) {
		ofPushMatrix();
		ofTranslate(w+20, 3*h/4);
		float blobX[contourFinder.blobs.size()];
		float blobY[contourFinder.blobs.size()];
		blobX[i]= (contourFinder.blobs[i].centroid.x/colorImage.width)*w; 
		blobY[i]= contourFinder.blobs[i].centroid.y/colorImage.height*h;
		ofScale(0.25, 0.25); //scale on 1/4th
		ofBeginShape();
		ofSetColor(255, 0, 0);
		for (int j=0; j<contourFinder.blobs[i].nPts; j++)
		{
			ofVertex(contourFinder.blobs[i].pts[j].x/colorImage.width*w, contourFinder.blobs[i].pts[j].y/colorImage.height*h);
		}
		ofEndShape(true);
		ofSetColor(200, 200, 200);
		ofCircle(blobX[i], blobY[i], 10);
		ofDrawBitmapString(ofToString(i), blobX[i]+3, blobY[i]);
		ofPopMatrix();
		//ofDrawBitmapString(ofToString(contourFinder.blobs.size(),0), 10, 10);
		//------------class
		for(int j=0; j<contourFinder.blobs.size();j++){
			//-----add the blob to blob per frame vector
			blobPerFrame.push_back(blob(blobX[j],blobY[j],i)); 
			/*//just checkin if I do it right
			ofSetColor(100, 0, 0);
			ofRect(w, 0, w+100, 50);
			ofSetColor(200, 200, 200);
			ofDrawBitmapString("added"+ofToString(i), w, 10+i*10);
			ofDrawBitmapString("blobPerFrame.size "+ofToString(blobPerFrame.size(), 0), w, 40);
			 */
		}
		
	}
	//----- add blob per frame vector to all frames vector 
	allFrames.push_back(blobPerFrame);
	
	//>>>>now we actually have a two-dimensional vector
	
	//-----clear content of the vector so it doesn't add up all the time
	blobPerFrame.clear();
	//-----just to check
	/*ofDrawBitmapString("pushed back frame", w+60, 10);
	ofDrawBitmapString("allframes.size="+ofToString(allFrames.size(), 0), w+200, 10); //gives me 50
	 */
	if (allFrames.size()==MaxNumberStored) {
		allFrames.erase(allFrames.begin(),allFrames.begin()+1);
		//-----just to check
		//ofDrawBitmapString("erased Frame", w+400, 10);
	}
	
	//----------------drawing
	
	for (int a=0; a<allFrames.size(); a++) {
		//-----just to check
		/*ofSetColor(100, 0, 0);
		ofRect(w, 40, w+25, 100);
		ofSetColor(200, 200, 200);
		ofDrawBitmapString("allframes no"+ofToString(a,0), w, 50);
		ofDrawBitmapString("allFrames.size:"+ofToString(allFrames.size(), 0), w, 60); //gives me 49
		 */
		
		for (int b=0; b<allFrames.at(a).size(); b++) {
			//-----just to check
			/*ofSetColor(100, 0, 0);
			ofRect(w, 90, 1.5*w, 200);
			ofSetColor(200, 200, 200);
			ofDrawBitmapString("blobPerFrame at "+ofToString(a, 0)+"draws at position "+ofToString(b, 0), w, 100);
			ofDrawBitmapString("allFrames.at a.size:"+ofToString(allFrames.at(a).size(), 0), w, 110);*/
			//-----draw circles
			allFrames[a][b].draw(a);
			
		}
	}
		
	//so the user sees the threshold and minBlobSize and how to change it
	ofSetColor(200, 200, 200);
	ofDrawBitmapString("threshold="+ofToString(threshold, 0), w+20+w/4+10, 10);
	ofDrawBitmapString("minBlobSize"+ofToString(minBlobSize, 0), w+20+w/4+10, 25);
	ofDrawBitmapString("'<'=increase threshold \n'z'=decrease threshold \n'1'=decrease minimum blob size \n'2'=increase minimum blob size \n's' to save the current output \nimage \n'esc' to exit \ncurrent trail length="+ofToString(MaxNumberStored,0), w+20+w/4+10, 40);
	ofPopMatrix();
	
	//------------------saving images
	screen.grabScreen(20, 20, w, h);  
	filename="save_"+ofToString(ofGetFrameNum(), 0)+".png";
	//for saving every frame
	//screen.saveImage(filename);
	
	//---- exit after last frame if you want
	if (video.getIsMovieDone()==true) {
		//std::exit(1);
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key=='s') {
		screen.saveImage(filename);
	}
	if (key=='<') {
		threshold--;
	}
	if (key=='z') {
		threshold++;
	}
	if (key=='1') {
		minBlobSize--;
	}
	if (key=='2') {
		minBlobSize++;
	}
	if (key=='p') {
		
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

