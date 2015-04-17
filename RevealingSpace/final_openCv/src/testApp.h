#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxOpenCv.h"
#include <vector>

#include "blob.h"

#define MaxNumberStored 100

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
	int minBlobSize;
	
	int w, h; //size of images
	string filename; //for screenshots
	
	//---------- video player
	ofVideoPlayer video;
	ofVideoPlayer emptyvideo;
	
	ofTexture vidTexture; //current image
	unsigned char * vidPixels; //current pixels
	
	ofTexture originalTexture; //original image
	unsigned char * originalPixels; //original pixels
	
	//----------OpenCv
	ofxCvColorImage  colorImage;
	ofxCvColorImage	backgroundColorImage;
	
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBackground;
	ofxCvGrayscaleImage grayDifference;
	ofxCvContourFinder contourFinder;
	
	bool bLearnBackground;
	bool blobInBlob;
	
	int threshold;
	
	//----saving images
	ofImage screen; 
	int filecounter;

private: 
	blob * myblob; 
	vector <blob> blobPerFrame;
	vector < vector <blob> >allFrames;
	

};

#endif
