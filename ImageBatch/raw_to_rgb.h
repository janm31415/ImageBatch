#pragma once


#include "jtk/image.h"

#include <string>

namespace jtk
  {

  enum EBayerMatrixType
    {
    BM_NONE = 0,
    BM_GRBG = 1,
    BM_GBRG = 2,
    BM_RGGB = 3,
    BM_BGGR = 4
    };

  EBayerMatrixType get_bayer_matrix_type(const std::string& bayer);  
  image<uint64_t> bilinear(const image<uint16_t>& raw, EBayerMatrixType iBayer);      
  void scale_image(image<uint64_t>& im, double scale);  
  image<uint32_t> clamp_to_rgb(const image<uint64_t>& im);
  }