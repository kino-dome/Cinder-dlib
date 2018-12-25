//
//  Types.h
//  ImageExample
//
//  Created by Hesam Ohadi on 19/10/17.
//
//

#ifndef CinderDlib_Types_h
#define CinderDlib_Types_h

#pragma once

////////////////////////Utils.h from ofx

#include <vector>
#include "cinder/Cinder.h"
#include "cinder/Channel.h"
#include "cinder/Surface.h"

///////////////Defining Cinder Surface as dlib image
/// \sa http://dlib.net/dlib/image_processing/generic_image.h.html
namespace dlib
{
    template <typename T>
    struct image_traits;
    
    // SurfaceT<T>
    template <typename T>
    struct image_traits<ci::SurfaceT<T>>
    {
        typedef T pixel_type;
    };
    
    template <typename T>
    struct image_traits<const ci::SurfaceT<T>>
    {
        typedef T pixel_type;
    };
    
    // main methods for generic image in dlib
    template <typename T>
    inline long num_rows(const ci::SurfaceT<T>& aImage)
    {
        return aImage.getHeight();
    }
    
    template <typename T>
    inline long num_columns(const ci::SurfaceT<T>& aImage)
    {
        return aImage.getWidth();
    }
    
    template <typename T>
    inline void set_image_size(ci::SurfaceT<T>& aImage, long rows, long cols)
    {
        aImage = ci::SurfaceT<T>(cols, rows, aImage.hasAlpha(), aImage.getChannelOrder());
        
    }
    
    template <typename T>
    inline void* image_data(ci::SurfaceT<T>& aImage)
    {
        if (aImage.getDataStore()) // is allocated (?)
            return aImage.getData();
        else
            return nullptr;
    }
    
    template <typename T>
    inline const void* image_data(const ci::SurfaceT<T>& aImage)
    {
        if (aImage.getDataStore())// is allocated (?)
            return aImage.getData();
        else
            return nullptr;
    }
    
    
    template <typename T>
    inline long width_step(const ci::SurfaceT<T>& aImage)
    {
        return aImage.getRowBytes();
    }
    
    template <typename T>
    inline void swap(ci::SurfaceT<T>& aImage1, ci::SurfaceT<T>& aImage2)
    {
        ci::SurfaceT<T> tempImage = std::move(aImage1);
        aImage1 = std::move(aImage2);
        aImage2 = std::move(tempImage);
    }
    
    //ChannelT<T>
    
    template <typename T>
    struct image_traits<ci::ChannelT<T>>
    {
        typedef T pixel_type;
    };
    
    template <typename T>
    struct image_traits<const ci::ChannelT<T>>
    {
        typedef T pixel_type;
    };
    
    template <typename T>
    inline long num_rows(const ci::ChannelT<T>& aChannel)
    {
        return aChannel.getHeight();
    }
    
    template <typename T>
    inline long num_columns(const ci::ChannelT<T>& aChannel)
    {
        return aChannel.getWidth();
    }
    
    template <typename T>
    inline void set_image_size(ci::ChannelT<T>& aChannel, long rows, long cols)
    {
        aChannel = ci::ChannelT<T>(cols, rows);
    }
    
    template <typename T>
    inline void* image_data(ci::ChannelT<T>& aChannel)
    {
        if (aChannel.getDataStore()) // is allocated (?)
            return aChannel.getData();
        else
            return nullptr;
    }
    
    template <typename T>
    inline const void* image_data(const ci::ChannelT<T>& aChannel)
    {
        if (aChannel.getDataStore()) // is allocated (?)
            return aChannel.getData();
        else
            return nullptr;
    }
    
    
    template <typename T>
    inline long width_step(const ci::ChannelT<T>& aChannel)
    {
        return aChannel.getRowBytes();
    }
    
    template <typename T>
    inline void swap(ci::ChannelT<T>& aChannel1, ci::ChannelT<T>& aChannel2)
    {
        ci::ChannelT<T> tempChannel = std::move(aChannel1);
        aChannel1 = std::move(aChannel2);
        aChannel2 = std::move(tempChannel);
    }

}


#endif
