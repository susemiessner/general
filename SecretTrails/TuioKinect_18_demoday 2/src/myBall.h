
#ifndef	MY_BALL
#define MY_BALL


#include "ofMain.h"
#define NUM_POINTS 100
#define	NUM_COLORS 10


class myBall  {
public:
	void update(float _posX, float posY);
	void draw();
	void draw_1();
	void visibility();
	void updating();
	void colorChange(int _cursorId, int _colorChange);
	void drawColors(int w);
	void getCurrentColor(int _cursorId, int _colorChange);
	
	
	float posX[NUM_POINTS];
	float posY[NUM_POINTS];
	
	float colorR;
	float colorG;
	float colorB;
	float transparency;
	
	bool visible;
	bool updateTrue;
	int myId;
	bool dist;
	
	
	struct ALL_Colors {
		float _r[NUM_COLORS];
		float _g[NUM_COLORS];
		float _b[NUM_COLORS];
	} colors;
	
	myBall(int x, int y, int cursorId);

	
	
};

#endif



