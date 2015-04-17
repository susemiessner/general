
#ifndef _BLOB
#define _BLOB

#include "ofMain.h"

class blob{

public:
	blob(float x, float y, float theid);
	
	void update();
	void draw(int a);
	
	float x;
	float y;
	float theid;
	
	blob();
};

#endif