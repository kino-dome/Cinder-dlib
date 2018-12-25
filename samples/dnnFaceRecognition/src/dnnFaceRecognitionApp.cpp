#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

#include "CinderDlib.h"
#include "Net.h"

#include "dlib/image_io.h"
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/clustering.h"


using namespace ci;
using namespace ci::app;
using namespace std;

// all the dlib conversion are part of the kino namespace. ki===kino
using namespace kino;

//
// https://github.com/davisking/dlib/blob/master/examples/dnn_face_recognition_ex.cpp
//
/*
 This is an example illustrating the use of the deep learning tools from the dlib C++
 Library.  In it, we will show how to do face recognition.  This example uses the
 pretrained dlib_face_recognition_resnet_model_v1 model which is freely available from
 the dlib web site.  This model has a 99.38% accuracy on the standard LFW face
 recognition benchmark, which is comparable to other state-of-the-art methods for face
 recognition as of February 2017.
 
 In this example, we will use dlib to do face clustering.  Included in the examples
 folder is an image, bald_guys.jpg, which contains a bunch of photos of action movie
 stars Vin Diesel, The Rock, Jason Statham, and Bruce Willis.   We will use dlib to
 automatically find their faces in the image and then to automatically determine how
 many people there are (4 in this case) as well as which faces belong to each person.
 
 Finally, this example uses a network with the loss_metric loss.  Therefore, if you want
 to learn how to train your own models, or to get a general introduction to this loss
 layer, you should read the dnn_metric_learning_ex.cpp and
 dnn_metric_learning_on_images_ex.cpp examples.
 */

class dnnFaceRecognitionApp : public App {
public:
    static void prepare(Settings* aSettings);
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void keyDown( KeyEvent event) override;
    void update() override;
    void draw() override;
    
    //fields
    gl::TextureRef                              mImageTex;
    std::map<int, std::vector<gl::TextureRef>>  mFaceChipsMap; //mapped using the cluster_id
};

void dnnFaceRecognitionApp::prepare(Settings* aSettings)
{
    aSettings->setTitle("dnn Face Recognition");
    aSettings->setWindowSize(ivec2(600, 800));
}

void dnnFaceRecognitionApp::setup()
{
    // Set up a timing mechanism for benchmarking.
    auto start = std::chrono::system_clock::now();
    
    // The first thing we are going to do is load all our models.  First, since we need to
    // find faces in the image we will need a face detector:
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    // We will also use a face landmarking model to align faces to a standard pose:  (see face_landmark_detection_ex.cpp for an introduction)
    dlib::shape_predictor sp;
    dlib::deserialize( getAssetPath("shape_predictor_5_face_landmarks.dat").string() ) >> sp;
    // And finally we load the DNN responsible for face recognition.
    anet_type net;
    dlib::deserialize( getAssetPath("dlib_face_recognition_resnet_model_v1.dat").string() ) >> net;
    
    dlib::matrix<dlib::rgb_pixel> img;
    dlib::load_image(img, getAssetPath("bald_guys.jpg").string());
    // convert image to cinder from dlib
    mImageTex = gl::Texture::create(fromDlib(img));
    
    // Run the face detector on the image of our action heroes, and for each face extract a
    // copy that has been normalized to 150x150 pixels in size and appropriately rotated
    // and centered.
    std::vector<matrix<rgb_pixel>> faces;
    for (auto face : detector(img))
    {
        auto shape = sp(img, face);
        dlib::matrix<dlib::rgb_pixel> face_chip;
        dlib::extract_image_chip(img, dlib::get_face_chip_details(shape,150,0.25), face_chip);
        faces.push_back(move(face_chip));
    }
    
    if (faces.size() == 0)
    {
        cout << "No faces found in image!" << endl;
    }
    
    // This call asks the DNN to convert each face image in faces into a 128D vector.
    // In this 128D vector space, images from the same person will be close to each other
    // but vectors from different people will be far apart.  So we can use these vectors to
    // identify if a pair of images are from the same person or from different people.
    std::vector<dlib::matrix<float,0,1>> face_descriptors = net(faces);
    
    // In particular, one simple thing we can do is face clustering.  This next bit of code
    // creates a graph of connected faces and then uses the Chinese whispers graph clustering
    // algorithm to identify how many people there are and which faces belong to whom.
    std::vector<dlib::sample_pair> edges;
    for (size_t i = 0; i < face_descriptors.size(); ++i)
    {
        for (size_t j = i; j < face_descriptors.size(); ++j)
        {
            // Faces are connected in the graph if they are close enough.  Here we check if
            // the distance between two face descriptors is less than 0.6, which is the
            // decision threshold the network was trained to use.  Although you can
            // certainly use any other threshold you find useful.
            if (dlib::length(face_descriptors[i]-face_descriptors[j]) < 0.6)
                edges.push_back(dlib::sample_pair(i,j));
        }
    }
    std::vector<unsigned long> labels;
    const auto num_clusters = dlib::chinese_whispers(edges, labels);
    // This will correctly indicate that there are 4 people in the image.
    console() << "number of people found in the image: "<< num_clusters << endl;
    
    
    // Now let's display the face clustering results on the screen.  You will see that it
    // correctly grouped all the faces.
    for (size_t cluster_id = 0; cluster_id < num_clusters; ++cluster_id)
    {
        std::vector<gl::TextureRef> temp;
        for (size_t j = 0; j < labels.size(); ++j)
        {
            if (cluster_id == labels[j])
                temp.push_back(gl::Texture::create(fromDlib(faces[j])));
        }
        mFaceChipsMap[cluster_id] = std::move(temp);

    }
    
    
    
    
    // Finally, let's print one of the face descriptors to the screen.
    console() << "face descriptor for one face: " << dlib::trans(face_descriptors[0]) << endl;
    
    // It should also be noted that face recognition accuracy can be improved if jittering
    // is used when creating face descriptors.  In particular, to get 99.38% on the LFW
    // benchmark you need to use the jitter_image() routine to compute the descriptors,
    // like so:
    dlib::matrix<float,0,1> face_descriptor = dlib::mean(dlib::mat(net(jitter_image(faces[0]))));
    console() << "jittered face descriptor for one face: " << dlib::trans(face_descriptor) << endl;
    // If you use the model without jittering, as we did when clustering the bald guys, it
    // gets an accuracy of 99.13% on the LFW benchmark.  So jittering makes the whole
    // procedure a little more accurate but makes face descriptor calculation slower.
    
    // Finish benchmarking.
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    
    console() << "Finished computation at " << std::ctime(&end_time);
    console() << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;
}

void dnnFaceRecognitionApp::mouseDown( MouseEvent event )
{
}

void dnnFaceRecognitionApp::update()
{
}

void dnnFaceRecognitionApp::keyDown( KeyEvent event )
{
}

void dnnFaceRecognitionApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    //draw main picture
        auto imageBounds = mImageTex->getBounds();
        Rectf destRect = Rectf(0, 0, getWindowWidth(), imageBounds.getHeight() / (imageBounds.getWidth()/(float)getWindowWidth()));
        gl::draw(mImageTex, imageBounds, destRect);
    
    
    //display clusters
    {
        const gl::ScopedModelMatrix scModelMatrix;
        //start from the bottom of the main picture plus some padding
        gl::translate(0, destRect.getHeight() + 20.0f);
        
        for (auto& iter: mFaceChipsMap){
            int clusterId = iter.first;
            auto& faces = iter.second; //all the faces for each cluster
            {
                const gl::ScopedModelMatrix scModelMatrix2;
                gl::drawString("Cluster No." + toString(clusterId), vec2(12, 35));
                // skip the space occupied the text
                gl::translate(100, 0);
                for (auto& faceTex : faces){
                    gl::draw(faceTex, Rectf(0.0f, 0.0f, 75.0f, 75.0f));
                    // skip the space occupied by each image, originally (150x150) but scaled down to 75x75
                    gl::translate(75.0f, 0);
                }
            }
            gl::translate(vec2(0, 75.0f));
        }
    }
}

CINDER_APP( dnnFaceRecognitionApp, RendererGl, &dnnFaceRecognitionApp::prepare )
