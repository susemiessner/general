
#include "TuioKinect.h"

ofxCvKalman *tuioPointSmoothed[32];

TuioPoint updateKalman(int id, TuioPoint tp) {
	if (id>=16) return NULL;
	if(tuioPointSmoothed[id*2] == NULL) {
		tuioPointSmoothed[id*2] = new ofxCvKalman(tp.getX());
		tuioPointSmoothed[id*2+1] = new ofxCvKalman(tp.getY());
	} else {
		tp.update(tuioPointSmoothed[id*2]->correct(tp.getX()),tuioPointSmoothed[id*2+1]->correct(tp.getY()));
	}
	
	return tp;
}

void clearKalman(int id) {
	if (id>=16) return;
	if(tuioPointSmoothed[id*2]) {
		delete tuioPointSmoothed[id*2];
		tuioPointSmoothed[id*2] = NULL;
		delete tuioPointSmoothed[id*2+1];
		tuioPointSmoothed[id*2+1] = NULL;
	}
}


//--------------------------------------------------------------
void TuioKinect::setup()
{
	ofSetWindowTitle("TuioKinect");
	
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
	
	depthImage.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	colorImage.allocate(kinect.width, kinect.height);
	redImage.allocate(kinect.width, kinect.height);
	invertedGrayImage.allocate(kinect.width, kinect.height);

	
	nearThreshold = 100;
	farThreshold  = 210;
	//farThreshold  = 100;

	TuioTime::initSession();	
	tuioServer = new TuioServer();
	tuioServer->setSourceName("TuioKinect");
	tuioServer->enableObjectProfile(false);
	tuioServer->enableBlobProfile(false);
	
	for (int i=0;i<32;i++)
		tuioPointSmoothed[i] = NULL;

	ofSetFrameRate(40);
	ofBackground(0, 0,0);	
	
	
	pointCounter = 0;
	counter=0;
	ballCounter=0;
	maxCursors=0;
	
	
	//colorChange=0;
	
	//____________________________________________________________
	//audio
	
	ofSoundStreamSetup(0, 2, this, 44100, 256, 4);//variables: output chanels, input capture, device u r using, frequency/sample rate, bufferSize, number of Buffers (2 chanels for every channel)
	
	//left = new float [256];
	right = new float [256];
	
	
	//____________________________________________________________
	//class
	
	/*for (int i=0; i<NUM_CURSORS; i++) {
		allBalls.push_back( new myBall(0,0) );
	}*/
	
	//-----------------------------------------------------------------------
	//clap detection
	maxAmplitude=8000;
	
	switch1=0;
	switch2=0;
	timeCounter=0;
	ofSetFrameRate(60);
	
	clap = false;
	clapCounter=0;
	
	maxTime=60*1; //2secs
	minTime=60/4;
	
	
	fullWidth=1280-40; 
	fullHeight=768-40;
	translateX=20; 
	translateY=20;
	
	//___________________________________________________________________
	//___________________________________________________________________
	//SECOND PART OF SCREEN > INTERFACE

	font1.loadFont("Verdana.ttf",18);
	font2.loadFont("Verdana.ttf",10);
	
	clickedId=0;
	
	allStart = false;
	allColor = false;
	allVisible = false;
	
	start = new bool [8];
	color = new bool [8];
	visible = new bool [8];
	colorChange = new int[10];
	for (int i=0; i<8; i++) {
		start[i]=false;
		color[i]=false;
		visible[i]=false;
		colorChange[i]=0;
	}
	
	NUM_COLORS1=10;
	
	colors1._r[0]=239;
	colors1._g[0]=68;
	colors1._b[0]=35;
	
	colors1._r[1]=228;
	colors1._g[1]=158;
	colors1._b[1]=32;
	
	colors1._r[2]=228;
	colors1._g[2]=229;
	colors1._b[2]=31;
	
	colors1._r[3]=193;
	colors1._g[3]=75;
	colors1._b[3]=155;
	
	colors1._r[4]=90;
	colors1._g[4]=90;
	colors1._b[4]=168;
	
	colors1._r[5]=22;
	colors1._g[5]=157;
	colors1._b[5]=72;
	
	colors1._r[6]=88;
	colors1._g[6]=185;
	colors1._b[6]=71;
	
	colors1._r[7]=14;
	colors1._g[7]=76;
	colors1._b[7]=154;
	
	colors1._r[8]=25;
	colors1._g[8]=67;
	colors1._b[8]=36;
	
	colors1._r[9]=107;
	colors1._g[9]=45;
	colors1._b[9]=101;
	
	colors1._r[10]=137;
	colors1._g[10]=24;
	colors1._b[10]=26;
	
	colors1._r[11]=14;
	colors1._g[11]=76;
	colors1._b[11]=154;
	
}

//--------------------------------------------------------------
void TuioKinect::update()
{
	counter++;
	kinect.update();

	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	grayImage.mirror(true, true);
	
	unsigned char * pix = grayImage.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight()-1;
	
	depthImage.setFromPixels(pix, kinect.width, kinect.height);
	depthImage.flagImageChanged();
	
	colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
	colorImage.mirror(false, true);
	colorImage.convertToGrayscalePlanarImage(redImage, 0);
	
	unsigned char * invertedPix=grayImage.getPixels();
	
	for(int i = numPixels; i > 0 ; i--){
		if( pix[i] > nearThreshold && pix[i] < farThreshold ){
			pix[i] = 255;
		}else{
			pix[i] = 0;
		}
	}
	//update the cv image
	grayImage.flagImageChanged();
	
	unsigned char * red = redImage.getPixels();
	numPixels = redImage.getWidth() * redImage.getHeight();
	
	contourFinder.findContours(grayImage, 900/2, (kinect.width*kinect.height), NUM_CURSORS, false);

	TuioTime frameTime = TuioTime::getSessionTime();
	tuioServer->initFrame(frameTime);
	
	std::vector<ofxCvBlob>::iterator blob;
	for (blob=contourFinder.blobs.begin(); blob!= contourFinder.blobs.end(); blob++) {
		float xpos = (*blob).centroid.x;
		float ypos = (*blob).centroid.y;
				
		TuioPoint tp(xpos/kinect.width,ypos/kinect.height);
		
		//if ((tp.getY() > 0.8) && (tp.getX()>0.25) && (tp.getX()<0.75)) continue;
		
		TuioCursor *tcur = tuioServer->getClosestTuioCursor(tp.getX(),tp.getY());
		if ((tcur==NULL) || (tcur->getDistance(&tp)>0.2)) { 
			tcur = tuioServer->addTuioCursor(tp.getX(), tp.getY());
			updateKalman(tcur->getCursorID(),tcur);
		} else {
			TuioPoint kp = updateKalman(tcur->getCursorID(),tp);
			tuioServer->updateTuioCursor(tcur, kp.getX(), kp.getY());
		}
	}

	tuioServer->stopUntouchedMovingCursors();
	
	std::list<TuioCursor*> dead_cursor_list = tuioServer->getUntouchedCursors();
	std::list<TuioCursor*>::iterator dead_cursor;
	for (dead_cursor=dead_cursor_list.begin(); dead_cursor!= dead_cursor_list.end(); dead_cursor++) {
		clearKalman((*dead_cursor)->getCursorID());
	}
	
	tuioServer->removeUntouchedStoppedCursors();
	tuioServer->commitFrame();
	
	if(pointCounter < NUM_POINTS){
		pointCounter++;
	}
	
		
}

//--------------------------------------------------------------
void TuioKinect::draw()
{		
	/*if (counter>60*60) {					
		counter=0;
		ofSaveScreen("screenshot_"+ofToString(ofGetYear())+"-"+ofToString(ofGetMonth())+"-"+ ofToString(ofGetDay())+"_"+ ofToString(ofGetHours())+":"+ofToString(ofGetMinutes())+":"+ofToString(ofGetSeconds())+".png");
		
	}	*/	
		std::list<TuioCursor*> alive_cursor_list = tuioServer->getTuioCursors();
		std::list<TuioCursor*>::iterator alive_cursor;
		for (alive_cursor=alive_cursor_list.begin(); alive_cursor!= alive_cursor_list.end(); alive_cursor++) {
			TuioCursor *ac = (*alive_cursor);
			if (ac->getCursorID()>=allBalls.size()) {
			//if (ac->getCursorID()>=ballCounter) {
				//maxCursors=ac->getCursorID();
				allBalls.push_back( new myBall( ac->getX()*fullWidth+translateX, ac->getY()*fullHeight+translateY, ac->getCursorID()));
				ofSetColor(200, 200, 200);
				ofDrawBitmapString(ofToString(ac->getX()*fullWidth+translateX), 100, 100);	
				ballCounter++;
			}
			/*for (int i=0; i<allBalls.size(); i++) {
				allBalls[i]->update(ac->getX()*ofGetWidth(), ac->getY()*ofGetHeight());
			}*/
			allBalls[ac->getCursorID()]->update(ac->getX()*fullWidth+translateX, ac->getY()*fullHeight+translateY);
						
		}
	

	//that's the real draw
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2, 0, 0);
	for (int i=0; i<allBalls.size(); i++) {
		allBalls[i]->draw();
	}
	ofPopMatrix();
	
		char reportStr[1024];
		sprintf(reportStr, "set near threshold %i (press: + -)\nset far threshold %i (press: < >)\nnum blobs found %i, fps: %i", nearThreshold, farThreshold, (int)contourFinder.blobs.size(), (int)ofGetFrameRate());
		ofDrawBitmapString(reportStr, 20, 650);
	
	
	//----------------------------------------
	//audio
	
	float amplitude=0;
	
	for (int i=0; i<256; i+=10) {
		amplitude+=right[i];
	}
	amplitude=amplitude*1000000/256;	
	ofSetColor(200, 0, 0);
	ofDrawBitmapString(ofToString(amplitude), 10, 10);
	
	/*if (colorChange>NUM_COLORS) {
		colorChange=0;
	}*/
	//-----------------------------------------------------------------------
	//clap detection
	
	ofBackground(0, 0, 0);
	
	if (clapCounter != 0) {
		timeCounter++;
		ofBackground(100, 100, 100);
	}
	
	if (clap==false && amplitude > maxAmplitude) {
		clapCounter++;
		clap=true;
	}
	if (amplitude < maxAmplitude-100) {
		clap=false;
	}
	
	if (clapCounter==1 && timeCounter>maxTime) {
		if (switch1==0) {
			for (int i=0; i<allBalls.size(); i++) {
				allBalls[i]->updating();
			}		}else {
			for (int i=0; i<allBalls.size(); i++) {
				allBalls[i]->updating();
			}		}
		clapCounter=0;
		timeCounter=0;
		if (allStart==false) {
			allStart=true;
		}else {
			allStart=false;
		}
	}
	
	if (clapCounter==2 && timeCounter<maxTime && timeCounter>minTime) {
		if (switch2==0) {
			for (int i=0; i<allBalls.size(); i++) {
				allBalls[i]->visibility();
			}
		}else {
			for (int i=0; i<allBalls.size(); i++) {
				allBalls[i]->visibility();
			}		}
		clapCounter=0;
		timeCounter=0;
		if (allVisible==false) {
			allVisible=true;
		}else {
			allVisible=false;
		}
	}
	if (clapCounter>=2) {
		clapCounter=0;
	}
	

	ofSetColor(200, 0, 0);
	string _clap;
	if (clap==true) {
		_clap="true";
	} else {
		_clap="false";
	}
	
	
	//_______________________________________________________________
	/*
	ofSetColor(200, 200, 200);
	ofDrawBitmapString(ofToString(colorChange), 40,50);
	
	ofDrawBitmapString("INSTRUCTIONS \n q for colorChange \n NUMBERS to make different circles visible/invisible \n updating/not updating different circles by SHIFT + NUMBER \n mouseX"+ofToString(mouseX, 1)+
					   "\n fullWidth:"+ofToString(fullWidth)+
					   "\n ofGetWidth:"+ofToString(ofGetWidth()), 40, 60);*/

	//___________________________________________________________________
	//___________________________________________________________________
	//SECOND PART OF SCREEN > INTERFACE
	ofSetColor(0, 0, 0);
	ofRect(0, 0, ofGetWidth()/2+translateX*2, ofGetHeight());
	
	
	//draw current picture
	ofSetColor(50, 50, 50);
	ofRect(70, 25, ofGetWidth()/4-60, ofGetHeight()/2);
	for (int i=0; i<allBalls.size(); i++) {
		allBalls[i]->draw_1();
	}
	
	ofSetColor(0, 0, 0);
	ofRect(0, 0, 70, ofGetHeight());
	ofRect(0, 0, ofGetWidth()/2, 25);
	ofRect(0, ofGetHeight()/2, ofGetWidth()/2,100);
	ofRect(ofGetWidth()/4-60, 0, 100, ofGetHeight());
	
	
	//colors
	ofSetColor(50, 50, 50);
	ofRect(70, ofGetHeight()-195, ofGetWidth()/2-2*70, 140);
	ofSetColor(200, 200, 200);
	font1.drawString("color palette", 74, ofGetHeight()-170);
	//------------------------>the long way
	for (int num=0; num<NUM_COLORS1; num++) {
		ofSetColor(colors1._r[num], colors1._g[num], colors1._b[num]);
		ofRect(80+num*(ofGetWidth()-80*4)/(2*10), ofGetHeight()-150, (ofGetWidth()-80*4)/(2*10), 80);
	}	
	
	
	//allBalls[0] -> drawColors((ofGetWidth()-80*4)/(2*10));
	
	//-----graphical interface
	ofSetColor(50, 50, 50);
	ofRect(ofGetWidth()/4-10, 25, 578, 210);
	ofSetColor(200, 200, 200);
	font1.drawString("graphical interface", ofGetWidth()/4, 52);
	font2.drawString("press buttons to change", ofGetWidth()/4, 52+20);
	
	ofPushMatrix();
	ofTranslate(0, 60, 0);
	//take snapshot
	ofSetColor(100, 100, 100);
	ofRect(ofGetWidth()/4, 52+20*4, 160, 30);
	ofSetColor(200, 200, 200);
	font2.drawString("take screenshot", ofGetWidth()/4 +15, 52+20*4+15);
	
	//text
	ofSetColor(200, 200, 200);
	font2.drawString("start/stop drawing", ofGetWidth()/4, 52);
	font2.drawString("change color", ofGetWidth()/4, 52+20);
	font2.drawString("change visibility", ofGetWidth()/4, 52+20*2);
	font2.drawString("current color", ofGetWidth()/4, 52+20*3);

	font2.drawString("all", ofGetWidth()/4+170, 32);
	for (int i=0; i<allBalls.size(); i++) {
		font2.drawString(ofToString(i), ofGetWidth()/4+200+i*20, 32);
	}
	//buttons 
	
	//start stop
	if (allStart==false ) {
		ofSetColor(100, 100, 100);
	}else {
		ofSetColor(200, 100, 100);}
	ofRect(ofGetWidth()/4+170, 42, 15, 15);
	
	//colorChange
	if (allColor==false ) {
		ofSetColor(100, 100, 100);
	}else {
		ofSetColor(200, 100, 100);}
	ofRect(ofGetWidth()/4+170, 42+20, 15, 15);
	
	//visibility
	if (allVisible==false ) {
		ofSetColor(100, 100, 100);
	}else {
		ofSetColor(200, 100, 100);}
	ofRect(ofGetWidth()/4+170, 42+20*2, 15, 15);
	
	//--------------single buttons
	//start/stop
	for (int j=0; j<allBalls.size(); j++) {
		if (start[j]==false ) {
			ofSetColor(100, 100, 100);
		}else {
			ofSetColor(200, 100, 100);}
		ofRect(ofGetWidth()/4+200+20*j, 42, 15, 15);
	}
	//colorChange
	for (int j=0; j<allBalls.size(); j++) {
		if (color[j]==false ) {
			ofSetColor(100, 100, 100);
		}else {
			ofSetColor(200, 100, 100);}
		ofRect(ofGetWidth()/4+200+20*j, 42+20, 15, 15);
	}
	//visibility
	for (int j=0; j<allBalls.size(); j++) {
		if (visible[j]==false ) {
			ofSetColor(100, 100, 100);
		}else {
			ofSetColor(200, 100, 100);}
		ofRect(ofGetWidth()/4+200+20*j, 42+2*20, 15, 15);
	}
	//current color
	for (int j=0; j<allBalls.size(); j++) {
		allBalls[j] ->getCurrentColor(j,colorChange[j]);
		//ofSetColor(200, 100, 100);
		ofRect(ofGetWidth()/4+200+20*j, 42+3*20, 15, 15);
	}
	ofPopMatrix();
	
	
	//-------------------------------------------------------------
	//clap detection
	ofSetColor(50, 50, 50);
	ofRect(68, ofGetHeight()/2+17, 510, 120);
	ofSetColor(200, 200, 200);
	font1.drawString("clap detection", 73, ofGetHeight()/2+40);
	font2.drawString(" 1 clap  > start/stop drawing \n 2 claps > change visibility", 250, ofGetHeight()/2+33);
	font2.drawString(//" current amplitude:"+ofToString(amplitude)+
					   "\n maxAmplitude: "+ofToString(maxAmplitude)+
					   //   "\n Clap:"+_clap+
					   "\n maxTime: "+ofToString(maxTime)+
					   "\n timeCounter: "+ofToString(timeCounter)+
					   "\n clapCounter: "+ofToString(clapCounter)//+
					   /*"\n switch1:"+ofToString(switch1)+
					   "\n switch2:"+ofToString(switch2)*/, 65, ofGetHeight()/2+50);
	
	if (clapCounter!=0) {
		ofSetColor(200, 100, 100);
		ofRect(180, 425, 30, 30);
	}
	//----------------
	//general info
	ofSetColor(50, 50, 50);
	ofRect(ofGetWidth()/4-10, 250, 578, 230);
	ofSetColor(200, 200, 200);
	font1.drawString("about", ofGetWidth()/4, 275);
	font2.drawString("The idea for the “drawing floor” installation was based on the interest of how \npeople move in space. It was my interest for some time now to reveal how \nspaces are used. But this installation brings the idea to another level. By \nprojecting the people’s movement in space back, it becomes an interactive \npiece. Passing through the tracked area people will leave a trail. \nSo don't forget",ofGetWidth()/4,300);
	font1.drawString("YOUR BODY IS YOUR PEN",ofGetWidth()/4,400);

}

//--------------------------------------------------------------
void TuioKinect::exit(){
	kinect.close();
	
}

//--------------------------------------------------------------
void TuioKinect::keyPressed (int key)
{
	switch (key)
	{	
		case '<':
			for (int i=0; i<allBalls.size(); i++) {
				colorChange[i]++;
				allBalls[i]->colorChange(i,colorChange[i]);
				/*ofSetColor(200, 200, 200);
				ofDrawBitmapString(ofToString(i+colorChange), 40,50);*/
			}
			break;

		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
		case 'c':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
		//visible	
		case '0':
			allBalls[0]->visibility();
			break;
		case '1':
			allBalls[1]->visibility();
			break;
		case '2':
			allBalls[2]->visibility();
			break;
		case '3':
			allBalls[3]->visibility();
			break;
		case '4':
			allBalls[4]->visibility();
			break;
		case '5':
			allBalls[5]->visibility();
			break;
		case '6':
			allBalls[6]->visibility();
			break;
		case '7':
			allBalls[7]->visibility();
			break;
		case '8':
			allBalls[8]->visibility();
			break;
		case '9':
			allBalls[9]->visibility();
			break;
		//update	
		case 'q':
			allBalls[1]->updating();
			break;
		case 'w':
			allBalls[2]->updating();
			break;
		case 'e':
			allBalls[3]->updating();
			break;
		case 'r':
			allBalls[4]->updating();
			break;
		case 't':
			allBalls[5]->updating();
			break;
		case 'y':
			allBalls[6]->updating();
			break;
		case 'u':
			allBalls[7]->updating();
			break;
		case 'i':
			allBalls[8]->updating();
			break;
		case 'o':
			allBalls[9]->updating();
			break;
		case 'p':
			allBalls[0]->updating();
			break;
		//color
		case 'a':
			colorChange[1]++;
			allBalls[1]->colorChange(1,colorChange[1]);
			break;
		case 's':
			colorChange[2]++;
			allBalls[2]->colorChange(2,colorChange[2]);
			break;
		case 'd':
			colorChange[3]++;
			allBalls[3]->colorChange(3,colorChange[3]);
			break;
		case 'f':
			colorChange[4]++;
			allBalls[4]->colorChange(4,colorChange[4]);
			break;
		case 'g':
			colorChange[5]++;
			allBalls[5]->colorChange(5,colorChange[5]);
			break;
		case 'h':
			colorChange[6]++;
			allBalls[6]->colorChange(6,colorChange[6]);
			break;
		case 'j':
			colorChange[7]++;
			allBalls[7]->colorChange(7,colorChange[7]);
			break;
		case 'k':
			colorChange[8]++;
			allBalls[8]->colorChange(8,colorChange[8]);
			break;
		case 'l':
			colorChange[9]++;
			allBalls[9]->colorChange(9,colorChange[9]);
			break;
		case 'ö':
			colorChange[0]++;
			allBalls[0]->colorChange(0,colorChange[0]);
			break;
			

			
			
		case '+':
			maxAmplitude+=10;
			break;
		case '-':
			maxAmplitude-=10;
			break;
	}
	if (key=='z') {
		ofSaveScreen("screenshot_"+ofToString(ofGetYear())+"-"+ofToString(ofGetMonth())+"-"+ ofToString(ofGetDay())+"_"+ ofToString(ofGetHours())+":"+ofToString(ofGetMinutes())+":"+ofToString(ofGetSeconds())+".png");
	}
	
}

//--------------------------------------------------------------
void TuioKinect::mouseMoved(int x, int y)
{}

//--------------------------------------------------------------
void TuioKinect::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void TuioKinect::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void TuioKinect::mouseReleased(int x, int y, int button)
{	
	//graphical interface
	//ALL
	//--------------------
	
	//start/stop drawing
	if (x>ofGetWidth()/4+170 && x<ofGetWidth()/4+170+15 && y>42+60 && y<42+15+60) {
		for (int i=0; i<allBalls.size(); i++) {
			allBalls[i]->updating();
		}
		if (allStart==false) {
			allStart=true;
		}else {
			allStart=false;
		}

	}
	//change color
	if (x>ofGetWidth()/4+170 && x<ofGetWidth()/4+170+15 && y>42+20+60 && y<42+20+15+60) {
		//colorChange++;
		for (int i=0; i<allBalls.size(); i++) {
			colorChange[i]=colorChange[i]+1;
			allBalls[i]->colorChange(i,colorChange[i]);
		}
		if (allColor==false) {
			allColor=true;
		}else {
			allColor=false;
		}
	}
	//visiblility
	if (x>ofGetWidth()/4+170 && x<ofGetWidth()/4+170+15 && y>42+20*2+60 && y<42+20*2+15+60) {
		for (int i=0; i<allBalls.size(); i++) {
			allBalls[i]->visibility();
		}	
		if (allVisible==false) {
			allVisible=true;
		}else {
			allVisible=false;
		}
	}
	
	//SINGLE ONES
	//start-stop
	for (int j=0; j<allBalls.size(); j++) {
		if (x>ofGetWidth()/4+200+20*j && x<ofGetWidth()/4+200+20*j+15 && y>42+60 && y<42+15+60) {
			allBalls[j]->updating();

			if (start[j]==false) {
				start[j]=true;
			}else {
				start[j]=false;
			}
		}
	}
	//color
	for (int j=0; j<allBalls.size(); j++) {
		if (x>ofGetWidth()/4+200+20*j && x<ofGetWidth()/4+200+20*j+15 && y>42+20+60 && y<42+20+15+60) {
			colorChange[j]++;
			allBalls[j]->colorChange(j,colorChange[j]);
			if (color[j]==false) {
				color[j]=true;
			}else {
				color[j]=false;
			}
		}
	}
	
	//visibility
	//ofRect(ofGetWidth()/4+200+20*j, 42+2*20, 15, 15);
	for (int j=0; j<allBalls.size(); j++) {
		if (x>ofGetWidth()/4+200+20*j && x<ofGetWidth()/4+200+20*j+15 && y>42+2*20+60 && y<42+2*20+15+60) {
			allBalls[j]->visibility();
			
			if (visible[j]==false) {
				visible[j]=true;
			}else {
				visible[j]=false;
			}
		}
	}
	
	//take screenshot
	if (x>ofGetWidth()/4 && x<ofGetWidth()/4+160 && y>52+20*4+60 && y<52+20*4+30+60) {
		ofImage screenshot;
		screenshot.allocate(ofGetWidth()/2, ofGetHeight(), OF_IMAGE_COLOR);
		screenshot.grabScreen(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetHeight());
		screenshot.saveImage("screenshot_"+ofToString(ofGetYear())+"-"+ofToString(ofGetMonth())+"-"+ ofToString(ofGetDay())+"_"+ ofToString(ofGetHours())+":"+ofToString(ofGetMinutes())+":"+ofToString(ofGetSeconds())+".png");
	}
}

//--------------------------------------------------------------
void TuioKinect::windowResized(int w, int h)
{}

void TuioKinect::audioReceived(float * input, int bufferSize, int nChannels){
	//audio is "interleaved" (left, right, left, right,...)
	for (int i=0; i<bufferSize; i++) {
		
		//left[i]=input[i*2]; //even left
		right[i]=input[i*2+1]; //uneven right
	}
}





