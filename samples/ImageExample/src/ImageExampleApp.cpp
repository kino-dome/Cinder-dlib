#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"

#include "CinderDlib.h"
#include "dlib/image_io.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// all the dlib conversion are part of the kino namespace. ki===kino
using namespace kino;

class ImageExampleApp : public App {
  public:
    //prepare settings
    static void prepareSettings(Settings* aSettings);
    
    //routine methods
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    //fields
    SurfaceRef          mImage;
    gl::TextureRef      mImageTex, mGrayTex, mBlurTex, mEdgeTex, mHeatEdgeTex, mJetEdgeTex, mRandomEdgeTex;
};

void ImageExampleApp::prepareSettings(Settings* aSettings)
{
    aSettings->setWindowSize(640, 480);
    aSettings->setTitle("Image Example");
    aSettings->setResizable(false);
}

void ImageExampleApp::setup()
{
    mImage = Surface::create(loadImage(loadAsset("puppy.jpg")));
    
    Channel gray = Channel(*mImage);
    
    // ofxDlib includes wrappers to translate ofPixels into of dlib::generic_image.
    
    // Declare our result image (note that it has not yet been allocated, but
    // will be within the dlib::gaussian_blur function below).
    Channel blurred_img;
    
    // Blur the image.
    dlib::gaussian_blur(gray, blurred_img, 1);
    
    // Now find the horizontal and vertical gradient images.
    Channel32f horz_gradient;
    Channel32f vert_gradient;
    Channel edge_image;

    dlib::sobel_edge_detector(blurred_img, horz_gradient, vert_gradient);
    
    // Now we do the non-maximum edge suppression step so that our edges are
    // nice and thin.
    dlib::suppress_non_maximum_edges(horz_gradient, vert_gradient, edge_image);
    
    // We can also easily display the edge_image as a heatmap or using the jet
    // color scheme like so.
    dlib::matrix<dlib::rgb_pixel> heat_edge_matrix = dlib::heatmap(edge_image);
    
    // Here we use create the heat_edge_image.
    // This is a shallow copy and shares the same underlying data with
    // heat_edge_matrix. Thus, it is important that heat_edge_matrix does not
    // go out of scope before heat_edge_image.
    Surface heat_edge_image (fromDlib(heat_edge_matrix));
    
    //    std::cout << "The same underlying data? " <<  (dlib::image_data(heat_edge_matrix) == dlib::image_data(heat_edge_image)) << std::endl;
    
    //   Since dlib::heatmap and similar objects return dlib::matrix_exp, our
    //   toOf helper functions correctly resolve the expression without an
//     additional assignment to a dlib::matrix before passing to ofxDlib::toOf.
//    
//     Thus, ofxDlib::toOf will preserve the speedy templated expression system
//     in dlib when possible.
    Surface jet_edge_image = fromDlib(dlib::jet(edge_image));
    Surface random_edge_image = fromDlib(dlib::randomly_color_image(edge_image));
    
    mImageTex = gl::Texture::create(*mImage);
    mGrayTex = gl::Texture::create(gray);
    mBlurTex = gl::Texture::create(blurred_img);
    mEdgeTex = gl::Texture::create(edge_image);
    mHeatEdgeTex = gl::Texture::create(heat_edge_image);;
    mJetEdgeTex = gl::Texture::create(jet_edge_image);
    mRandomEdgeTex = gl::Texture::create(random_edge_image);
    
}

void ImageExampleApp::mouseDown( MouseEvent event )
{
}

void ImageExampleApp::update()
{
}

void ImageExampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    int w = mImageTex->getWidth();
    int h = mImageTex->getHeight();
    auto drawRect = Rectf(0, 0, w, h);
    // draw images
    {
        const gl::ScopedModelMatrix scModelMatrix;
        gl::translate(50, 0);
        gl::draw(mImageTex, drawRect);
        gl::translate(w * 1.2, 0);
        gl::draw(mGrayTex, drawRect);
        gl::translate(w * 1.2, 0);
        gl::draw(mBlurTex, drawRect);
    }
    // draw processed images
    {
        const gl::ScopedModelMatrix scModelMatrix;
        gl::translate(0, h * 1.2);
        gl::draw(mEdgeTex, drawRect);
        gl::translate(w, 0);
        gl::draw(mHeatEdgeTex, drawRect);
        gl::translate(w, 0);
        gl::draw(mJetEdgeTex, drawRect);
        gl::translate(w, 0);
        gl::draw(mRandomEdgeTex, drawRect);
    }
}

CINDER_APP( ImageExampleApp, RendererGl, &ImageExampleApp::prepareSettings )
