#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderDlib.h"
#include "dlib/dnn.h"
#include "dlib/image_io.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// all the dlib conversion are part of the kino namespace. ki===kino
using namespace kino;

//
// https://github.com/davisking/dlib/blob/master/examples/dnn_mmod_face_detection_ex.cpp
//
/*
 This example shows how to run a CNN based face detector using dlib.  The
 example loads a pretrained model and uses it to find faces in images.  The
 CNN model is much more accurate than the HOG based model shown in the
 face_detection_ex.cpp example, but takes much more computational power to
 run, and is meant to be executed on a GPU to attain reasonable speed.  For
 example, on a NVIDIA Titan X GPU, this example program processes images at
 about the same speed as face_detection_ex.cpp.
 Also, users who are just learning about dlib's deep learning API should read
 the dnn_introduction_ex.cpp and dnn_introduction2_ex.cpp examples to learn
 how the API works.  For an introduction to the object detection method you
 should read dnn_mmod_ex.cpp
 
 TRAINING THE MODEL
 Finally, users interested in how the face detector was trained should
 read the dnn_mmod_ex.cpp example program.  It should be noted that the
 face detector used in this example uses a bigger training dataset and
 larger CNN architecture than what is shown in dnn_mmod_ex.cpp, but
 otherwise training is the same.  If you compare the net_type statements
 in this file and dnn_mmod_ex.cpp you will see that they are very similar
 except that the number of parameters has been increased.
 Additionally, the following training parameters were different during
 training: The following lines in dnn_mmod_ex.cpp were changed from
 mmod_options options(face_boxes_train, 40,40);
 trainer.set_iterations_without_progress_threshold(300);
 to the following when training the model used in this example:
 mmod_options options(face_boxes_train, 80,80);
 trainer.set_iterations_without_progress_threshold(8000);
 Also, the random_cropper was left at its default settings,  So we didn't
 call these functions:
 cropper.set_chip_dims(200, 200);
 cropper.set_min_object_size(40,40);
 The training data used to create the model is also available at
 http://dlib.net/files/data/dlib_face_detection_dataset-2016-09-30.tar.gz
 */

template <long num_filters, typename SUBNET>
using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;

template <long num_filters, typename SUBNET>
using con5 = dlib::con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET>
using downsampler = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;

template <typename SUBNET>
using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;



//////////


class dnnMmodFaceDetectionApp : public App {
public:
    static void prepare(Settings* aSettings);
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    gl::TextureRef                  mImageTex;
    vector<dlib::mmod_rect>         mDets;
    float                           mDetScale;
};

void dnnMmodFaceDetectionApp::prepare(Settings* aSettings)
{
    aSettings->setTitle("dnn mmod Face Detection");
    aSettings->setWindowSize(1000, 800);
}

void dnnMmodFaceDetectionApp::setup()
{
    // Set up a timing mechanism for benchmarking.
    auto start = std::chrono::system_clock::now();
    
    net_type net;
    dlib::deserialize(getAssetPath("mmod_human_face_detector.dat").string()) >> net;
    
    
    dlib::matrix<dlib::rgb_pixel> img;
    dlib::load_image(img, getAssetPath("crowd2.jpg").string());
    mImageTex = gl::Texture::create(fromDlib(img));
    mDetScale = 1.0f;
    
    // Upsampling the image will allow us to detect smaller faces but will cause the
    // program to use more RAM and run longer.
    while(img.size() < 1000*1000){
        pyramid_up(img);
        mDetScale *= 2.0;
    }
    
    // Note that you can process a bunch of images in a std::vector at once and it runs
    // much faster, since this will form mini-batches of images and therefore get
    // better parallelism out of your GPU hardware.  However, all the images must be
    // the same size.  To avoid this requirement on images being the same size we
    // process them individually in this example.
    mDets = net(img);
    
    // Finish benchmarking.
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    
    console() << "Finished computation at " << std::ctime(&end_time);
    console() << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;;
    
    // 16 seconds on MacBook Pro (15-inch, Mid 2012), no CUDA support.
    // 1.2 seconds on i7 7700 + Nvidia 1080, CUDA support + MKL libs.
    
    console() << "Found " << mDets.size() << " faces." << endl;
}

void dnnMmodFaceDetectionApp::mouseDown( MouseEvent event )
{
}

void dnnMmodFaceDetectionApp::update()
{
}

void dnnMmodFaceDetectionApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    gl::color(Color::white());
    
    //scale down everything so it fits in our window
    {
        const gl::ScopedModelMatrix scModelMatrix;
        gl::scale(vec3(getWindowWidth() / (float)mImageTex->getWidth()));
        if (mImageTex){
            gl::draw(mImageTex);
        }
        
        gl::scale(vec3(1.0/mDetScale));
        for (auto& det: mDets)
        {
            std::stringstream ss;
            ss << "Confidence: " << det.detection_confidence << std::endl;
            ss << "Ignore: " << det.ignore;
            
            Area rect = kino::fromDlib(det);
            gl::color(ColorAf(1.0, 1.0, 1.0, 0.2));
            gl::drawSolidRect(rect);
            gl::color(ColorAf(1.0, 0.0, 0.0, 0.8));
            gl::drawStringCentered(ss.str(), rect.getCenter());
        }
    }
    
}

CINDER_APP( dnnMmodFaceDetectionApp, RendererGl, &dnnMmodFaceDetectionApp::prepare )
