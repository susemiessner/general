#include "myBall.h"

myBall::myBall(int x, int y, int cursorId)
{
	for (int i=0; i<NUM_POINTS; i++) {
		posX[i]=x;
		posY[i]=y;
	}	
	myId=cursorId;

	colors._r[0]=239;
	colors._g[0]=68;
	colors._b[0]=35;
	
	colors._r[1]=228;
	colors._g[1]=158;
	colors._b[1]=32;
	
	colors._r[2]=228;
	colors._g[2]=229;
	colors._b[2]=31;
	
	colors._r[3]=193;
	colors._g[3]=75;
	colors._b[3]=155;
	
	colors._r[4]=90;
	colors._g[4]=90;
	colors._b[4]=168;
	
	colors._r[5]=22;
	colors._g[5]=157;
	colors._b[5]=72;
	
	colors._r[6]=88;
	colors._g[6]=185;
	colors._b[6]=71;
	
	colors._r[7]=14;
	colors._g[7]=76;
	colors._b[7]=154;
	
	colors._r[8]=25;
	colors._g[8]=67;
	colors._b[8]=36;
	
	colors._r[9]=107;
	colors._g[9]=45;
	colors._b[9]=101;
	
	colors._r[10]=137;
	colors._g[10]=24;
	colors._b[10]=26;
	
	colors._r[11]=14;
	colors._g[11]=76;
	colors._b[11]=154;
	
	
	//-----------------------------------------
	visible = true;
	updateTrue = true;
	
	colorR = colors._r[cursorId];
	colorG = colors._g[cursorId];
	colorB = colors._b[cursorId];
	transparency=100;
	dist=false;
}

void myBall::updating(){
	if (updateTrue==true) {
		updateTrue=false;
	}else {
		updateTrue=true;
	}
}

void myBall::visibility()
{	
	if (visible==true) {
		visible=false;
	}else {
		visible=true;
	}
}

void myBall::update(float _posX, float _posY){
	if (updateTrue==true) {
		for (int i=1; i<NUM_POINTS; i++) {
			posX[i-1]=posX[i];
			posY[i-1]=posY[i];
		}
		posX[NUM_POINTS-1]=_posX;
		posY[NUM_POINTS-1]=_posY;
	}
}

void myBall::draw(){
	
	if (visible==true) {
		for (int i=0; i<NUM_POINTS; i++) {
			ofEnableAlphaBlending();
			ofSetColor(colorR, colorG, colorB, transparency/NUM_POINTS*i);
			ofCircle(posX[i], posY[i], 50);
			ofDisableAlphaBlending();
			
		}
	}	
}
void myBall::draw_1(){
	
		for (int i=0; i<NUM_POINTS; i++) {
			ofEnableAlphaBlending();
			ofSetColor(colorR, colorG, colorB, transparency/NUM_POINTS*i);
			ofCircle(10+posX[i]/2, 10+posY[i]/2, 50/2);
			ofDisableAlphaBlending();
			
			ofSetColor(0, 0, 0);
			ofDrawBitmapString(ofToString(myId), 10+posX[NUM_POINTS-1]/2, 10+posY[NUM_POINTS-1]/2);
		}
		
}


void myBall::colorChange(int _cursorId, int _colorChange)
{	if(_cursorId+_colorChange >= NUM_COLORS-1)
	{
		_colorChange=NUM_COLORS-_colorChange;
		_cursorId=0;
	}
	colorR = colors._r[_cursorId+_colorChange];
	colorG = colors._g[_cursorId+_colorChange];
	colorB = colors._b[_cursorId+_colorChange];
}



void myBall::getCurrentColor(int _cursorId, int _colorChange){
	ofSetColor(colors._r[_cursorId+_colorChange], colors._g[_cursorId+_colorChange], colors._b[_cursorId+_colorChange]);
}




