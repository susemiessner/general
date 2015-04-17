
#include "blob.h"

blob::blob(float _x, float _y, float _theid){

	x = _x;
	y = _y;
	theid = _theid;
}

void blob::update(){
}

void blob::draw(int trans){
	ofSetColor(193,75,155,50+trans/10);
	ofCircle(x, y, 5);
	//-----just to check
	//ofDrawBitmapString(ofToString(theid), x+5, y);
}