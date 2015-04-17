

#ifndef _TUIO_KINECT
#define _TUIO_KINECT

#define NUM_CURSORS 7

#include "ofMain.h"
#include "myBall.h"
#include<vector>

#include "ofxFenster.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxCvKalman.h"
#include "TuioServer.h"
using namespace TUIO;

class TuioKinect : public ofBaseApp, public ofxFensterListener{

	public:

		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
		ofxKinect kinect;

		ofxCvGrayscaleImage	grayImage;
		ofxCvGrayscaleImage	depthImage;
		ofxCvColorImage		colorImage;
		ofxCvGrayscaleImage	redImage;
		ofxCvGrayscaleImage invertedGrayImage;

		ofxCvContourFinder 	contourFinder;
		TuioServer *tuioServer;

		int nearThreshold;
		int	farThreshold;
	
	
	int pointCounter;
	int maxCursors;
	int counter;
	int ballCounter;
	
	
	
	
	//int colorChange; //to shift the colors around
	
	//_______________________________________________________________________
	//audio
	
	//built in of methods; chanel left, second right
	void audioReceived(float * input, int bufferSize, int nChannels );
	
	float * left;
	float * right;
	
		
	//-----------------------------------------------------------------------
	//my Balls class
	
	vector<myBall*> allBalls;
	
	int clickedId;
	int * colorChange;


	
private:
	myBall** TheBall;
	
	//-----------------------------------------------------------------------
	//clap detection
	int switch1;
	int switch2;
	int timeCounter;
	int clapCounter;
	bool clap;
	int maxTime;
	int minTime;
	int maxAmplitude;
	
	//___________________________________________________________________
	//___________________________________________________________________
	//SECOND PART OF SCREEN > INTERFACE
	
	int fullWidth, fullHeight;
	int translateX, translateY;
	
	ofTrueTypeFont font1, font2;
	int NUM_COLORS1;
	
	//booleans for buttons
	bool	allStart, allColor, allVisible;
	bool * start;
	bool * color;
	bool * visible;
	struct ALL_Colors {
		float _r[NUM_COLORS];
		float _g[NUM_COLORS];
		float _b[NUM_COLORS];
	} colors1;
};

#endif
