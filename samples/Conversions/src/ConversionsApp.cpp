#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "CinderDlib.h"
#include "dlib/image_io.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// all the dlib conversion are part of the kino namespace. ki===kino
using namespace kino;

class ConversionsApp : public App {
public:
    //prepare settings
    static void prepareSettings(Settings* aSettings);
    
    //routine methods
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    //fields
    vector<gl::TextureRef>          mTextures;
};

void ConversionsApp::prepareSettings(Settings* aSettings)
{
    aSettings->setTitle("Conversions");
    aSettings->setWindowSize(1333, 500);
}

void ConversionsApp::setup()
{
    {
        dlib::array2d<dlib::rgb_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::array2d<dlib::bgr_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::array2d<dlib::rgb_alpha_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::array2d<dlib::hsi_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::array2d<dlib::lab_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::array2d<unsigned char> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::array2d<float> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::array2d<unsigned short> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<dlib::rgb_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<dlib::bgr_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<dlib::rgb_alpha_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<dlib::hsi_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<dlib::lab_pixel> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<unsigned char> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<float> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    {
        dlib::matrix<unsigned short> img;
        dlib::load_image(img, getAssetPath("puppy.jpg").string());
        mTextures.push_back(gl::Texture::create(fromDlib(img)));
    }
    
    
    auto d_p = dlib::point(1, 2);
    auto d_dp = dlib::dpoint(1, 2);
    console()<<"d_p is :"<<d_p<<endl;
    console()<<"d_dp is :"<<d_dp<<endl;
    
    auto o_p = fromDlib(d_p);
    auto o_dp = fromDlib(d_dp);
    console()<<"o_p is :"<<o_p<<endl;
    console()<<"o_dp is :"<<o_dp<<endl;
 }

void ConversionsApp::mouseDown( MouseEvent event )
{
    
}

void ConversionsApp::update()
{
    
}

void ConversionsApp::draw()
{
    gl::clear( Color( 0.3, 0.3, 0.3 ) );
    
    const gl::ScopedModelMatrix scModelMatrix;
    for (auto& texture: mTextures)
    {
        auto currentMat = gl::getModelMatrix();
        auto pos = vec3(currentMat[3]);
        if (pos.x + texture->getWidth()> getWindowWidth())
        {
            gl::translate(-pos.x, texture->getHeight());
        }

        gl::draw(texture, texture->getBounds());
        gl::translate(texture->getWidth(), 0);
    }
}

CINDER_APP( ConversionsApp, RendererGl, &ConversionsApp::prepareSettings )
