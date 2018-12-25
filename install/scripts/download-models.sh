#!/bin/sh
cd ../../assets
mkdir -p models
mkdir -p models/mnist

echo "Downloading shape_predictor_68_face_landmarks.dat.bz2"
curl -L -o models/shape_predictor_68_face_landmarks.dat.bz2 --progress-bar https://github.com/davisking/dlib-models/raw/master/shape_predictor_68_face_landmarks.dat.bz2

echo "Extracting shape_predictor_68_face_landmarks.dat"
bzip2 -d models/shape_predictor_68_face_landmarks.dat.bz2

echo "Downloading shape_predictor_5_face_landmarks.dat.bz2"
curl -L -o models/shape_predictor_5_face_landmarks.dat.bz2 --progress-bar https://github.com/davisking/dlib-models/raw/master/shape_predictor_5_face_landmarks.dat.bz2

echo "Extracting shape_predictor_5_face_landmarks.dat"
bzip2 -d models/shape_predictor_5_face_landmarks.dat.bz2

echo "Downloading mmod_human_face_detector.dat.bz2"
curl -L -o models/mmod_human_face_detector.dat.bz2 --progress-bar https://github.com/davisking/dlib-models/raw/master/mmod_human_face_detector.dat.bz2

echo "Extracting mmod_human_face_detector.dat"
bzip2 -d models/mmod_human_face_detector.dat.bz2

echo "Downloading dlib_face_recognition_resnet_model_v1.dat.bz2"
curl -L -o models/dlib_face_recognition_resnet_model_v1.dat.bz2 --progress-bar https://github.com/davisking/dlib-models/raw/master/dlib_face_recognition_resnet_model_v1.dat.bz2

echo "Extracting dlib_face_recognition_resnet_model_v1.dat"
bzip2 -d models/dlib_face_recognition_resnet_model_v1.dat.bz2

echo "Downloading resnet34_1000_imagenet_classifier.dnn.bz2"
curl -L -o models/resnet34_1000_imagenet_classifier.dnn.bz2 --progress-bar http://dlib.net/files/resnet34_1000_imagenet_classifier.dnn.bz2

echo "Extracting resnet34_1000_imagenet_classifier.dnn"
bzip2 -d models/resnet34_1000_imagenet_classifier.dnn.bz2

# MNIST
echo "Downloading MNIST Dataset"
curl -L -o models/mnist/train-images-idx3-ubyte.gz --progress-bar  http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz
curl -L -o models/mnist/train-labels-idx1-ubyte.gz --progress-bar  http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz
curl -L -o models/mnist/t10k-images-idx3-ubyte.gz --progress-bar  http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz
curl -L -o models/mnist/t10k-labels-idx1-ubyte.gz --progress-bar  http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz

echo "Extracting MNIST Dataset"
gunzip -d models/mnist/train-images-idx3-ubyte.gz
gunzip -d models/mnist/train-labels-idx1-ubyte.gz
gunzip -d models/mnist/t10k-images-idx3-ubyte.gz
gunzip -d models/mnist/t10k-labels-idx1-ubyte.gz

echo "Done"

