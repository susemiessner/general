#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
	//-----------------------------------------
	// my variables
	//-----------------------------------------
	
	
	int w, h; //size of images
	int threshold;
	string filename; //for screenshots
	int filecounter;
	
	//---------- video player
	ofVideoPlayer video;
	ofVideoPlayer emptyvideo;
	
	
	ofImage originalPic; //picture to which all other pic r compared to
	unsigned char * originalPixels;
	
	ofImage  contrastPic; ////picture which is compared to the original one
	unsigned char * contrastPixels;
	
	ofImage  differencePic;
	unsigned char * differencePixels;
	
	

};

#endif
