#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class dnnMmodFaceLandmarkDetectionApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void dnnMmodFaceLandmarkDetectionApp::setup()
{
}

void dnnMmodFaceLandmarkDetectionApp::mouseDown( MouseEvent event )
{
}

void dnnMmodFaceLandmarkDetectionApp::update()
{
}

void dnnMmodFaceLandmarkDetectionApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( dnnMmodFaceLandmarkDetectionApp, RendererGl )
