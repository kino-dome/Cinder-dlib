//
//  Utils.h
//  ImageExample
//
//  Created by Hesam Ohadi on 19/10/17.
//
//

#ifndef CinderDlib_Utils_h
#define CinderDlib_Utils_h

#pragma once

#include <vector>
#include "cinder/Cinder.h"
#include "cinder/Log.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Area.h"
#include "cinder/ImageIo.h"
#include "cinder/Channel.h"
#include "cinder/Surface.h"
#include "cinder/CinderMath.h"

#include "dlib/image_processing.h"

namespace kino { // start namespace kino
// Helpers for the deduction of ChannelOrder, ColorModel and DataType
template<typename pixel_type>
inline ci::ImageIo::ChannelOrder getDlibChannelOrder()
{
    if (std::is_same<pixel_type, dlib::rgb_pixel>::value)
    {
        return ci::ImageIo::ChannelOrder::RGB;
    }
    else if (std::is_same<pixel_type, dlib::rgb_alpha_pixel>::value)
    {
        return ci::ImageIo::ChannelOrder::RGBA;
    }
    else if (std::is_same<pixel_type, dlib::bgr_pixel>::value)
    {
        return ci::ImageIo::ChannelOrder::BGR;
    }
    else if (dlib::pixel_traits<pixel_type>::num == 1)
    {
        return ci::ImageIo::ChannelOrder::Y;
    }
    else if (dlib::pixel_traits<pixel_type>::num == 2)
    {
        return ci::ImageIo::ChannelOrder::YA;
    }
    else if (std::is_same<pixel_type, dlib::hsi_pixel>::value)
    {
        CI_LOG_W("HSI pixel format not supported, using RGB.");
        return ci::ImageIo::ChannelOrder::RGB;
    }
    else if (std::is_same<pixel_type, dlib::lab_pixel>::value)
    {
        CI_LOG_W("LAB pixel format not supported, using RGB.");
        return ci::ImageIo::ChannelOrder::RGB;
    }
    else
    {
        CI_LOG_W("Unknown pixel format.");
        return ci::ImageIo::ChannelOrder::CUSTOM;
    }
}

template<typename pixel_type>
inline ci::ImageIo::ColorModel getDlibColorModel()
{
    if (dlib::pixel_traits<pixel_type>::num == 1)
    {
        return ci::ImageIo::ColorModel::CM_GRAY;
    }
    else
    {
        return ci::ImageIo::ColorModel::CM_RGB;
    }
}

template<typename pixel_type>
inline ci::ImageIo::DataType getDlibDataType()
{
    if (std::is_same<typename dlib::pixel_traits<pixel_type>::basic_pixel_type, unsigned char>::value)
    {
        return ci::ImageIo::DataType::UINT8;
    }
    else if(std::is_same<typename dlib::pixel_traits<pixel_type>::basic_pixel_type, unsigned short>::value || std::is_same<typename dlib::pixel_traits<pixel_type>::basic_pixel_type, unsigned int>::value)
    {
        return ci::ImageIo::DataType::UINT16;
    }
    else if(std::is_same<typename dlib::pixel_traits<pixel_type>::basic_pixel_type, float>::value || std::is_same<typename dlib::pixel_traits<pixel_type>::basic_pixel_type, unsigned long>::value)
    {
        return ci::ImageIo::DataType::FLOAT32;
    }
    else {
        return ci::ImageIo::DataType::FLOAT16;
    }
}


//Much like the OpenCV block, we need image sources so any cinder type can be initiated from it
//had some problems with value ranges and conversions, see here: http://discourse.libcinder.org/t/imagesource-and-cinder-inter-api-image-conversions-routines/963

template<typename pixel_type>
class ImageSourceDlib : public ci::ImageSource {
public:
    
    typedef typename dlib::pixel_traits<pixel_type>::basic_pixel_type basic_pixel_type;
    
    ImageSourceDlib(const dlib::array2d<pixel_type>& in, float aSourceValueMin = 0.0f, float aSourceValueMax = 255.0f) : ci::ImageSource(), mIsLab(false), mIsHsi(false)
    {
        mWidth = dlib::num_columns(in);
        mHeight = dlib::num_rows(in);
        setColorModel(getDlibColorModel<pixel_type>());
        setChannelOrder(getDlibChannelOrder<pixel_type>());
        setDataType(getDlibDataType<pixel_type>());
        mRowBytes = (int32_t)dlib::width_step(in);
        mData = reinterpret_cast<const basic_pixel_type*>(dlib::image_data(in));
        mSourceValueMin = aSourceValueMin;
        mSourceValueMax = aSourceValueMax;
        if (dlib::pixel_traits<pixel_type>::hsi){
            mIsHsi = true;
        }
        else if (dlib::pixel_traits<pixel_type>::lab){
            mIsLab = true;
        }
    }
    
    ImageSourceDlib(const dlib::matrix<pixel_type>& in, float aSourceValueMin = 0.0f, float aSourceValueMax = 255.0f) : ci::ImageSource(), mIsLab(false), mIsHsi(false)
    {
        mWidth = dlib::num_columns(in);
        mHeight = dlib::num_rows(in);
        setColorModel(getDlibColorModel<pixel_type>());
        setChannelOrder(getDlibChannelOrder<pixel_type>());
        setDataType(getDlibDataType<pixel_type>());
        mRowBytes = (int32_t)dlib::width_step(in);
        mData = reinterpret_cast<const basic_pixel_type*>(dlib::image_data(in));
        mSourceValueMin = aSourceValueMin;
        mSourceValueMax = aSourceValueMax;
        if (dlib::pixel_traits<pixel_type>::hsi){
            mIsHsi = true;
        }
        else if (dlib::pixel_traits<pixel_type>::lab){
            mIsLab = true;
        }
    }
    
    void load( ci::ImageTargetRef target ) {
        // get a pointer to the ImageSource function appropriate for handling our data configuration
        ci::ImageSource::RowFunc func = setupRowFunc( target );
        const size_t numChannels = ci::ImageIo::channelOrderNumChannels(mChannelOrder);
        
        if(getDataType() == ci::ImageIo::DataType::UINT8){
            if (mIsHsi){
                std::vector<uint8_t> data(mWidth * mHeight * numChannels, 0);
                for(int i=0; i< data.size(); i += 3){
                    dlib::assign_pixel_helpers::COLOUR col;
                    dlib::assign_pixel_helpers::HSL hsl;
                    hsl.h = *mData/255.0*360;
                    hsl.s = *(mData+1)/255.0;
                    hsl.l = *(mData+2)/255.0;
                    col = dlib::assign_pixel_helpers::HSL2RGB(hsl);
                    data[i] = static_cast<uint8_t>(col.r*255.0 + 0.5);
                    data[i+1] = static_cast<uint8_t>(col.g*255.0 + 0.5);
                    data[i+2] = static_cast<uint8_t>(col.b*255.0 + 0.5);
                    mData+=3;
                }
                
                for( int32_t row = 0; row < mHeight; ++row ) {
                    std::vector<uint8_t> rowData(data.begin() + mWidth * numChannels * row, data.begin() + mWidth * numChannels * (row+1));
                    ((*this).*func)( target, row, rowData.data() );
                }
            }
            else if(mIsLab) {
                std::vector<uint8_t> data(mWidth * mHeight * numChannels, 0);
                for(int i=0; i< data.size(); i += 3){
                    dlib::assign_pixel_helpers::COLOUR col;
                    dlib::assign_pixel_helpers::Lab lab;
                    lab.l = (*mData/255.0)*100;
                    lab.a = (*(mData+1)-128.0);
                    lab.b = (*(mData+2)-128.0);
                    col = dlib::assign_pixel_helpers::Lab2RGB(lab);
                    data[i] = static_cast<uint8_t>(col.r*255.0 + 0.5);
                    data[i+1] = static_cast<uint8_t>(col.g*255.0 + 0.5);
                    data[i+2] = static_cast<uint8_t>(col.b*255.0 + 0.5);
                    mData+=3;
                }
                
                for( int32_t row = 0; row < mHeight; ++row ) {
                    std::vector<uint8_t> rowData(data.begin() + mWidth * numChannels * row, data.begin() + mWidth * numChannels * (row+1));
                    ((*this).*func)( target, row, rowData.data() );
                }
            }
            else {
                for( int32_t row = 0; row < mHeight; ++row ) {
                    ((*this).*func)( target, row, mData );
                    mData += mRowBytes;
                }
            }
        }
        // float, map pixel values to (0,1)
        else if (getDataType() == ci::ImageIo::DataType::FLOAT32) {
            std::vector<float> data(mWidth * mHeight * numChannels, 0);
            for(auto& val: data){
                val = ci::lmap<float>(*mData, mSourceValueMin, mSourceValueMax, 0.0f, 1.0f);
                mData++;
            }
            
            for( int32_t row = 0; row < mHeight; ++row ) {
                std::vector<float> rowData(data.begin() + mWidth * numChannels * row, data.begin() + mWidth * numChannels * (row+1));
                ((*this).*func)( target, row, rowData.data() );
            }
        }
        else if (getDataType() == ci::ImageIo::DataType::UINT16 ||
                 getDataType() == ci::ImageIo::DataType::FLOAT16) {
            std::vector<uint16_t> data(mWidth * mHeight * numChannels, 0);
            for(auto& val: data){
                val = ci::lmap<uint16_t>(*mData, mSourceValueMin, mSourceValueMax, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
                mData++;
            }
            
            for( int32_t row = 0; row < mHeight; ++row ) {
                std::vector<uint16_t> rowData(data.begin() + mWidth * numChannels * row, data.begin() + mWidth * numChannels * (row+1));
                ((*this).*func)( target, row, rowData.data() );
            }
        }
    }
    
    const basic_pixel_type*		mData;
    float                       mSourceValueMin, mSourceValueMax;
    int32_t                     mRowBytes;
    bool                        mIsLab, mIsHsi;
};

/////////////////////// fromDlib Utils

template <typename pixel_type>
inline ci::ImageSourceRef fromDlib(const dlib::array2d<pixel_type>& in )
{
    return ci::ImageSourceRef( new ImageSourceDlib<pixel_type>(in));
}

template <typename pixel_type>
inline ci::ImageSourceRef fromDlib(const dlib::matrix<pixel_type>& in)
{
    return ci::ImageSourceRef( new ImageSourceDlib<pixel_type>(in));
}

template<typename E>
inline ci::ImageSourceRef fromDlib(const dlib::matrix_exp<E>& in)
{
    // Extract the underlying type from stored in the dlib::matrix_exp.
    typedef typename dlib::matrix_traits<E>::type pixel_type;
    
    // For dlib::matrix_exp, we must assign the expression to a concrete type
    // in order to resolve the expression. We cannot use an `auto` here.
    //
    // We can now instantiate our dlib::matrix.
    dlib::matrix<pixel_type> m = in;
    
    return ci::ImageSourceRef( new ImageSourceDlib<pixel_type>(m));
}

template<typename T>
inline const glm::tvec4<T>& fromDlib(const dlib::vector<T, 4>& v)
{
    return *reinterpret_cast<const glm::tvec4<T>*>(&v);
}

template<typename T>
inline const glm::tvec3<T>& fromDlib(const dlib::vector<T, 3>& v)
{
    return *reinterpret_cast<const glm::tvec3<T>*>(&v);
}

template<typename T>
inline const glm::tvec2<T>& fromDlib(const dlib::vector<T, 2>& v)
{
    return *reinterpret_cast<const glm::tvec2<T>*>(&v);
}

template<typename T>
inline const glm::tvec1<T>& fromDlib(const dlib::vector<T, 1>& v)
{
    return *reinterpret_cast<const glm::tvec1<T>*>(&v);
}

inline ci::Area fromDlib(const dlib::rectangle& r)
{
    return ci::Area(r.left(), r.top(), r.right(), r.bottom());
}
    
inline ci::Area fromDlib(const dlib::mmod_rect& r)
{
    return fromDlib(r.rect);
}

inline ci::Colorf fromDlib(const dlib::rgb_pixel& v)
{
    return ci::Colorf(v.red / 255.0f , v.green / 255.0f, v.blue / 255.0f);
}


inline ci::ColorAf fromDlib(const dlib::rgb_alpha_pixel& v)
{
    return ci::ColorAf(v.red / 255.0f, v.green / 255.0f, v.blue / 255.0f, v.alpha / 255.0f);
}

////// toDlib Utils

template<typename T>
inline const dlib::vector<T, 4>& toDlib(const glm::tvec4<T>& v)
{
    return *reinterpret_cast<const dlib::vector<T, 4>*>(&v);
}

template<typename T>
inline const dlib::vector<T, 3>& toDlib(const glm::tvec3<T>& v)
{
    return *reinterpret_cast<const dlib::vector<T, 3>*>(&v);
}


template<typename T>
inline const dlib::vector<T, 2>& toDlib(const glm::tvec2<T>& v)
{
    return *reinterpret_cast<const dlib::vector<T, 2>*>(&v);
}

template<typename T>
inline const dlib::vector<T, 1>& toDlib(const glm::tvec1<T>& v)
{
    return *reinterpret_cast<const dlib::vector<T, 1>*>(&v);
}

inline dlib::rectangle toDlib(const ci::Area& a)
{
    return dlib::rectangle(a.getX1(), a.getY1(), a.getX2(), a.getY2());
}

template<typename T>
inline dlib::rectangle toDlib(const ci::RectT<T>& r)
{
    return dlib::rectangle(r.getX1(), r.getY1(), r.getX2(), r.getY2());
}

inline dlib::rgb_pixel toDlib(const ci::Color8u& c)
{
    return dlib::rgb_pixel(c.r*255, c.g*255, c.b*255);
}

inline dlib::rgb_alpha_pixel toDlib(const ci::ColorA8u& c)
{
    return dlib::rgb_alpha_pixel(c.r * 255, c.g * 255, c.b * 255, c.a * 255);
}

//////////////////// Helper utils

inline void scale(dlib::rectangle& in, double scaler)
{
    in.left() *= scaler;
    in.top() *= scaler;
    in.right() *= scaler;
    in.bottom() *= scaler;
}

inline void scale(dlib::full_object_detection& in, double scaler)
{
    for (unsigned int i = 0; i < in.num_parts(); ++i)
    {
        // Must be done individually.
        in.part(i).x() *= scaler;
        in.part(i).y() *= scaler;
    }
    
    scale(in.get_rect(), scaler);
    
}

inline void scale(dlib::mmod_rect& in, double scaler)
{
    scale(in.rect, scaler);
}

}// end namespace kino::
namespace ki = kino;

#endif
