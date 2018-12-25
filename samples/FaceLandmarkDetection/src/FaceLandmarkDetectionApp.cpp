#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Resize.h"

#include "CinderDlib.h"
#include "dlib/image_io.h"
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/full_object_detection.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// all the dlib conversion are part of the kino namespace. ki===kino
using namespace kino;

class FaceLandmarkDetectionApp : public App {
  public:
    //prepare settings
    static void prepare(Settings* aSettings);
    
    //routine methods
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    //fields
    gl::TextureRef                          mImageTex;
    vector<dlib::full_object_detection>     mShapes;
    vector<gl::TextureRef>                  mFaceChips;
};

void FaceLandmarkDetectionApp::prepare(Settings* aSettings)
{
    aSettings->setTitle("Face Landmark Detection");
    aSettings->setWindowSize(1000, 750);
}

void FaceLandmarkDetectionApp::setup()
{
    // Set up a timing mechanism for benchmarking.
    auto start = std::chrono::system_clock::now();
    
    // We need a face detector. We will use this to get bounding boxes for
    // each face in an image.
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    
    // And we also need a shape_predictor.  This is the tool that will predict face
    // landmark positions given an image and face bounding box.  Here we are just
    // loading the model from the shape_predictor_68_face_landmarks.dat.
    dlib::shape_predictor sp;
    
    dlib::deserialize(getAssetPath("shape_predictor_68_face_landmarks.dat").string()) >> sp;
    
    dlib::array2d<dlib::rgb_pixel> img;
    
    dlib::load_image(img, getAssetPath("crowd.jpg").string());
    
    double detScale = 1;
    
    // Make the image larger so we can detect small faces.
    while (img.size() < 2400 * 2400)
    {
        dlib::pyramid_up(img);
        detScale *= 2;
    }

    // Now tell the face detector to give us a list of bounding boxes
    // around all the faces in the image.
    std::vector<dlib::rectangle> dets = detector(img);
    
    // Finish benchmarking.
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    
    console() << "Finished computation at " << std::ctime(&end_time);
    console() << "Elapsed time: " << elapsed_seconds.count() << "s";
    console() << std::endl;
    
    console() << "Number of faces detected: " << dets.size() << std::endl;
//     Now we will go ask the shape_predictor to tell us the pose of
//     each face we detected.
    mImageTex = gl::Texture::create(fromDlib(img));
    
    for (std::size_t j = 0; j < dets.size(); ++j)
    {
        dlib::full_object_detection shape = sp(img, dets[j]);
        mShapes.push_back(shape);
    }
    
    // We can also extract copies of each face that are cropped, rotated upright,
    // and scaled to a standard size as shown here:
    
    std::vector<dlib::chip_details> chipDetails = dlib::get_face_chip_details(mShapes);
    
    dlib::array<dlib::array2d<dlib::rgb_pixel>> face_chips;
    
    dlib::extract_image_chips(img,
                              chipDetails,
                              face_chips);
    
    for (auto& f: face_chips)
    {
        mFaceChips.push_back(gl::Texture::create(fromDlib(f)));
    }
}

void FaceLandmarkDetectionApp::mouseDown( MouseEvent event )
{
}

void FaceLandmarkDetectionApp::update()
{
}

void FaceLandmarkDetectionApp::draw()
{
	gl::clear(Color(0, 0, 0));
    
    {
        const gl::ScopedBlendAlpha scAlphaBlend;
        const gl::ScopedModelMatrix scModelMatrix;
        
        gl::scale(vec3(getWindowWidth() / (float)mImageTex->getWidth()));
        gl::color(Color::white());
        gl::draw(mImageTex);
        
        for (auto& shape: mShapes)
        {
            gl::color(ColorAf(.3 ,.5, .8, .3));
            gl::drawSolidRect(fromDlib(shape.get_rect()));
            
            gl::color(ColorAf(.6 ,.2, .8, .5));
            for (std::size_t i = 0; i < shape.num_parts(); ++i)
            {
                gl::drawSolidCircle(fromDlib(shape.part(i)), 5);
            }
        }
        
        float x = 0;
        float y = 0;
        
        for (std::size_t i = 0; i < mFaceChips.size(); ++i)
        {
            auto& face = mFaceChips[i];
            
            if (i != 0 && x + face->getWidth() > getWindowWidth())
            {
                y += face->getHeight();
                x = 0;
            }
            
            gl::color(Color::white());
            gl::draw(face, Rectf(x, y, x + face->getWidth(), y + face->getHeight()));
            
            x += face->getWidth();
        }
    }
}

CINDER_APP( FaceLandmarkDetectionApp, RendererGl, &FaceLandmarkDetectionApp::prepare)
