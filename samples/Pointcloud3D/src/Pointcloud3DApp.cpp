#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"

#include "CinderDlib.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// all the dlib conversion are part of the kino namespace. ki===kino
using namespace kino;

class Pointcloud3DApp : public App {
public:
    static void prepare(Settings* aSettings);
    void setup() override;
    void resize() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    CameraPersp             mCamera;
    CameraUi                mCamUi;
    gl::VboMeshRef          mPointcloudMesh;
};

void Pointcloud3DApp::prepare(Settings* aSettings)
{
    aSettings->setTitle("3D Pointcloud");
    aSettings->setWindowSize(800, 800);
}

void Pointcloud3DApp::setup()
{
    //for keeping the positons and color of the mesh
    vector<vec3> positions;
    vector<Colorf> colors;
    
    
    // Let's make a point cloud that looks like a 3D spiral.
    dlib::rand rnd;
    
    for (float i = -10; i < 10; i += 0.0005)
    {
        // Get a point on a spiral.
        dlib::vector<float> val(sin(i), cos(i), i / 4);
        
        // Now add some random noise to it.
        dlib::vector<float> temp(rnd.get_random_gaussian(),
                                 rnd.get_random_gaussian(),
                                 rnd.get_random_gaussian());
        val += temp / 20;
        
        // Scale it up.
        val *= 200;
        
        // Pick a color based on how far we are along the spiral.
        dlib::rgb_pixel color = dlib::colormap_jet(i, 0, 20);
        
        vec3 pos= fromDlib(val);
        Colorf col = fromDlib(color);
        positions.push_back(pos);
        colors.push_back(col);
    }
    
    //mesh layout
    gl::VboMesh::Layout layout;
    layout.usage(GL_STATIC_DRAW).attrib(geom::POSITION, 3).attrib(geom::COLOR, 3);
    
    // make mesh and buffer attribs
    mPointcloudMesh = gl::VboMesh::create( positions.size(), GL_POINTS, { layout } );
    mPointcloudMesh->bufferAttrib(geom::POSITION, positions.size() * sizeof(vec3), positions.data());
    mPointcloudMesh->bufferAttrib(geom::COLOR, positions.size() * sizeof(Colorf), colors.data());
    
    // Set up the camera.
    mCamera.lookAt( vec3( 0.0f, 0.0f, 1000.0f ), vec3( 0 ) );
    mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.01f, 10000.0f );
    mCamUi = CameraUi( &mCamera, getWindow() );
    
    //point size
    gl::pointSize(1);
}

void Pointcloud3DApp::resize()
{
    mCamera.setPerspective( 60.0f, getWindowAspectRatio(), 0.01f, 10000.0f );
}

void Pointcloud3DApp::mouseDown( MouseEvent event )
{
}

void Pointcloud3DApp::update()
{
}

void Pointcloud3DApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    {
        gl::ScopedViewport scViewport(ivec2(0), getWindowSize());
        gl::ScopedMatrices scMatrices;
        gl::setMatrices(mCamera);
        
        gl::ScopedDepth scDepth(true);
        gl::ScopedGlslProg scpGlslProg(gl::getStockShader(gl::ShaderDef().color()));
        gl::draw( mPointcloudMesh );
    }
}

CINDER_APP( Pointcloud3DApp, RendererGl )
