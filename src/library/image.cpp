/*
 * DIPlib 3.0
 * This file contains definitions for the Image class and related functions.
 *
 * (c)2014-2016, Cris Luengo.
 * Based on original DIPlib code: (c)1995-2014, Delft University of Technology.
 */

#include <iostream>
#include <algorithm>

#include "diplib.h"

namespace dip {

//
bool Image::CompareProperties(
      Image const& src,
      Option::CmpProps cmpProps,
      Option::ThrowException throwException
) const {
   if( cmpProps == Option::CmpProps_DataType ) {
      if( dataType_ != src.dataType_ ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, "Data type doesn't match" );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_Dimensionality ) {
      if( sizes_.size() != src.sizes_.size() ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, "Dimensionality doesn't match" );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_Sizes ) {
      if( sizes_ != src.sizes_ ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, E::SIZES_DONT_MATCH );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_Strides ) {
      if( strides_ != src.strides_ ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, "Strides don't match" );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_TensorShape ) {
      if( tensor_ != src.tensor_ ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, "Tensor shape doesn't match" );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_TensorElements ) {
      if( tensor_.Elements() != src.tensor_.Elements() ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, E::NTENSORELEM_DONT_MATCH );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_TensorStride ) {
      if( tensorStride_ != src.tensorStride_ ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, "Tensor stride doesn't match" );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_ColorSpace ) {
      if( colorSpace_ != src.colorSpace_ ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, "Color space doesn't match" );
         return false;
      }
   }
   if( cmpProps == Option::CmpProps_PixelSize ) {
      if( pixelSize_ != src.pixelSize_ ) {
         dip_ThrowIf( throwException == Option::ThrowException::doThrow, "Pixel sizes don't match" );
         return false;
      }
   }
   return true;
}

//
bool Image::CheckProperties(
      dip::uint ndims,
      dip::DataType::Classes dts,
      Option::ThrowException throwException
) const {
   bool result = sizes_.size() == ndims;
   if( !result && ( throwException == Option::ThrowException::doThrow ) ) {
      dip_Throw( E::DIMENSIONALITY_NOT_SUPPORTED );
   }
   result &= dts == dataType_;
   if( !result && ( throwException == Option::ThrowException::doThrow ) ) {
      dip_Throw( E::DATA_TYPE_NOT_SUPPORTED );
   }
   return result;
}

bool Image::CheckProperties(
      UnsignedArray const& sizes,
      dip::DataType::Classes dts,
      Option::ThrowException throwException
) const {
   bool result = sizes_ == sizes;
   if( !result && ( throwException == Option::ThrowException::doThrow ) ) {
      dip_Throw( E::SIZES_DONT_MATCH );
   }
   result &= dts == dataType_;
   if( !result && ( throwException == Option::ThrowException::doThrow ) ) {
      dip_Throw( E::DATA_TYPE_NOT_SUPPORTED );
   }
   return result;
}

bool Image::CheckProperties(
      UnsignedArray const& sizes,
      dip::uint tensorElements,
      dip::DataType::Classes dts,
      Option::ThrowException throwException
) const {
   bool result = sizes_ == sizes;
   if( !result && ( throwException == Option::ThrowException::doThrow ) ) {
      dip_Throw( E::SIZES_DONT_MATCH );
   }
   result &= tensor_.Elements() == tensorElements;
   if( !result && ( throwException == Option::ThrowException::doThrow ) ) {
      dip_Throw( E::NTENSORELEM_DONT_MATCH );
   }
   result &= dts == dataType_;
   if( !result && ( throwException == Option::ThrowException::doThrow ) ) {
      dip_Throw( E::DATA_TYPE_NOT_SUPPORTED );
   }
   return result;
}


//
std::ostream& operator<<(
      std::ostream& os,
      Image const& img
) {
   // Shape and other main propertiees
   if( img.TensorElements() == 1 ) {
      os << "Scalar image, ";
   } else {
      os << img.TensorRows() << "x" << img.TensorColumns() << "-tensor image, ";
   }
   os << img.Dimensionality() << "-D, " << img.DataType().Name();
   if( img.IsColor() ) {
      os << ", color image: " << img.ColorSpace();
   }
   os << std::endl;

   // Image size
   os << "   sizes: ";
   dip::UnsignedArray const& sizes = img.Sizes();
   for( dip::uint ii = 0; ii < sizes.size(); ++ii ) {
      os << ( ii > 0 ? ", " : "" ) << sizes[ ii ];
   }
   os << std::endl;

   // Pixel size
   if( img.HasPixelSize() ) {
      os << "   pixel size: ";
      dip::PixelSize const& ps = img.PixelSize();
      for( dip::uint ii = 0; ii < sizes.size(); ++ii ) {
         os << ( ii > 0 ? " x " : "" ) << ps[ ii ];
      }
      os << std::endl;
   }

   // Strides
   os << "   strides: ";
   dip::IntegerArray const& strides = img.Strides();
   for( dip::uint ii = 0; ii < strides.size(); ++ii ) {
      os << ( ii > 0 ? ", " : "" ) << strides[ ii ];
   }
   os << std::endl;

   os << "   tensor stride: " << img.TensorStride() << std::endl;

   // Data segment
   if( img.IsForged() ) {
      os << "   data pointer:   " << img.Data() << " (shared among " << img.ShareCount() << " images)" << std::endl;

      os << "   origin pointer: " << img.Origin() << std::endl;

      if( img.HasContiguousData() ) {
         if( img.HasNormalStrides() ) {
            os << "   strides are normal" << std::endl;
         } else {
            os << "   data are contiguous but strides are not normal" << std::endl;
         }
      }

      dip::uint stride;
      void* origin;
      img.GetSimpleStrideAndOrigin( stride, origin );
      if( origin ) {
         os << "   simple stride: " << stride << std::endl;
      } else {
         os << "   strides are not simple" << std::endl;
      }

   } else {
      os << "   not forged" << std::endl;
   }
   return os;
}

} // namespace dip
