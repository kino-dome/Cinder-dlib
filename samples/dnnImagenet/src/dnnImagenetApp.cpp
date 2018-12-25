#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderDlib.h"
#include "Resnet34.h"

#include "dlib/dnn.h"
#include "dlib/data_io.h"


using namespace ci;
using namespace ci::app;
using namespace std;

//
// From https://github.com/davisking/dlib/blob/master/examples/dnn_imagenet_ex.cpp
//
/*
 This example shows how to classify an image into one of the 1000 imagenet
 categories using the deep learning tools from the dlib C++ Library.  We will
 use the pretrained ResNet34 model available on the dlib website.
 The ResNet34 architecture is from the paper Deep Residual Learning for Image
 Recognition by He, Zhang, Ren, and Sun.  The model file that comes with dlib
 was trained using the dnn_imagenet_train_ex.cpp program on a Titan X for
 about 2 weeks.  This pretrained model has a top5 error of 7.572% on the 2012
 imagenet validation dataset.
 For an introduction to dlib's DNN module read the dnn_introduction_ex.cpp and
 dnn_introduction2_ex.cpp example programs.
 
 Finally, these tools will use CUDA and cuDNN to drastically accelerate
 network training and testing.  CMake should automatically find them if they
 are installed and configure things appropriately.  If not, the program will
 still run but will be much slower to execute.
 */

class dnnImagenetApp : public App {
  public:
    static void prepare(Settings* aSettings);
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void keyDown( KeyEvent event) override;
	void update() override;
	void draw() override;
    
    anet_type                                       mNet;
    vector<string>                                  mLabels;
    dlib::softmax<anet_type::subnet_type>           mSnet;
    dlib::rand                                      mRnd;
    
    gl::TextureRef                                  mCurrentImage;
    std::size_t                                     mCurrentImageIndex;
    vector<fs::path>                                mImagePaths;
    
    bool                                            mShouldLoadNext;
};

void dnnImagenetApp::prepare(Settings* aSettings)
{
    aSettings->setTitle("dnn Imagenet");
    aSettings->setWindowSize(ivec2(700, 700));
}

void dnnImagenetApp::setup()
{
    mCurrentImageIndex = 0;
    mShouldLoadNext = true;
    
    dlib::deserialize(getAssetPath("resnet34_1000_imagenet_classifier.dnn").string()) >> mNet >> mLabels;
    
    // Make a network with softmax as the final layer.  We don't have to do this
    // if we just want to output the single best prediction, since the anet_type
    // already does this.  But if we instead want to get the probability of each
    // class as output we need to replace the last layer of the network with a
    // softmax layer, which we do as follows:
    mSnet.subnet() = mNet.subnet();
    
    // Load paths to a bunch of images.
    for (auto& p: fs::recursive_directory_iterator(getAssetPath("")/"256_ObjectCategories"))
    {
        //only choose images
        if (p.path().extension() == ".png" || p.path().extension() == ".jpg"){
            mImagePaths.push_back(p);
        }
    }

}

void dnnImagenetApp::mouseDown( MouseEvent event )
{
}

void dnnImagenetApp::keyDown( KeyEvent event )
{
    mShouldLoadNext = true;
}

void dnnImagenetApp::update()
{
    if (mShouldLoadNext)
    {
        dlib::array<dlib::matrix<dlib::rgb_pixel>> images;
        
        dlib::matrix<dlib::rgb_pixel> img;
        
        dlib::load_image(img, mImagePaths[mCurrentImageIndex].string());
        
        const int num_crops = 16;
        
        // Grab 16 random crops from the image.  We will run all of them through the
        // network and average the results.
        randomly_crop_images(img, images, mRnd, num_crops);
        
        // p(i) == the probability the image contains object of class i.
        dlib::matrix<float,1,1000> p = dlib::sum_rows(dlib::mat(mSnet(images.begin(), images.end()))) / num_crops;
        
        console()<<"Image: "<<mImagePaths[mCurrentImageIndex]<<endl;
        // Print the 5 most probable labels
        for (int k = 0; k < 5; ++k)
        {
            unsigned long predicted_label = dlib::index_of_max(p);
            
            console() << p(predicted_label) << ": " << mLabels[predicted_label] << endl;
            
            p(predicted_label) = 0; // set it to zero so we find the next with index_of_max.
        }
        console()<<"-------------"<<endl;
        
        mCurrentImage = gl::Texture::create(kino::fromDlib(img));
        mCurrentImageIndex = (mCurrentImageIndex + 1) % mImagePaths.size();
        
        mShouldLoadNext = false;
    }
}

void dnnImagenetApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    if (mCurrentImage){
        gl::ScopedModelMatrix scModelMatrix;
        gl::translate(-mCurrentImage->getWidth()/2.0, -mCurrentImage->getHeight()/2.0);
        gl::translate(vec2(getWindowWidth()/2.0, getWindowHeight()/2.0));
        gl::draw(mCurrentImage);
    }

}

CINDER_APP( dnnImagenetApp, RendererGl, &dnnImagenetApp::prepare )
