#include "ofMain.h"
#include "TuioKinect.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1280*2,800, OF_WINDOW);			// <-------- setup the GL context

	ofRunApp( new TuioKinect );

}
