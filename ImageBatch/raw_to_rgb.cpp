#include "raw_to_rgb.h"

#include "jtk/concurrency.h"

namespace jtk
  {

  template <class T>
  T minimum(T a, T b)
    {
    return a < b ? a : b;
    }

  template <class T>
  T minimum(T a, T b, T c)
    {
    return minimum(minimum(a, b), c);
    }

  template <class T>
  T minimum(T a, T b, T c, T d)
    {
    return minimum(minimum(a, b, c), d);
    }

  template <class T>
  T maximum(T a, T b)
    {
    return a < b ? b : a;
    }

  template <class T>
  T maximum(T a, T b, T c)
    {
    return maximum(maximum(a, b), c);
    }

  template <class T>
  T maximum(T a, T b, T c, T d)
    {
    return maximum(maximum(a, b, c), d);
    }

  template <class T>
  T median(T a, T b, T c)
    {
    if (a > b)
      {
      if (c > a) return a;
      else if (c < b) return b;
      return c;
      }
    else
      {
      if (c < a) return a;
      else if (c > b) return b;
      return c;
      }
    }

  template <class T>
  T sqr(T a)
    {
    return a * a;
    }

  const char* BayerMatrixToString(const EBayerMatrixType& bm)
    {
    static const char* str[5] =
      {
      "None", "GRBG", "GBRG", "RGGB", "BGGR"
      };
    return str[bm];
    }

  static EBayerMatrixType g_BayerMatrixLeftOrRight[5] =
    // BM_NONE    BM_GRBG    BM_GBRG   BM_RGGB   BM_BGGR
    // +-+-+      +-+-+      +-+-+     +-+-+     +-+-+
    // |L|L|      |G|R|      |G|B|     |R|G|     |B|G|
    // |L|L|      |B|G|      |R|G|     |G|B|     |G|R|
    // +-+-+      +-+-+      +-+-+     +-+-+     +-+-+
    { BM_NONE, BM_RGGB, BM_BGGR, BM_GRBG, BM_GBRG };

  EBayerMatrixType GetBayerMatrixLeft(EBayerMatrixType iBayer) { return g_BayerMatrixLeftOrRight[iBayer]; }
  EBayerMatrixType GetBayerMatrixRight(EBayerMatrixType iBayer) { return g_BayerMatrixLeftOrRight[iBayer]; }

  static EBayerMatrixType g_BayerMatrixUpOrDown[5] =
    // BM_NONE    BM_GRBG    BM_GBRG   BM_RGGB   BM_BGGR
    // +-+-+      +-+-+      +-+-+     +-+-+     +-+-+
    // |L|L|      |G|R|      |G|B|     |R|G|     |B|G|
    // |L|L|      |B|G|      |R|G|     |G|B|     |G|R|
    // +-+-+      +-+-+      +-+-+     +-+-+     +-+-+
    { BM_NONE, BM_BGGR, BM_RGGB, BM_GBRG, BM_GRBG };

  EBayerMatrixType GetBayerMatrixUp(EBayerMatrixType iBayer) { return g_BayerMatrixUpOrDown[iBayer]; }
  EBayerMatrixType GetBayerMatrixDown(EBayerMatrixType iBayer) { return g_BayerMatrixUpOrDown[iBayer]; }

  EBayerMatrixType get_bayer_matrix_type(const std::string& bayer)
    {
    if (bayer == "BGGR")
      return BM_BGGR;
    else if (bayer == "RGGB")
      return BM_RGGB;
    else if (bayer == "GBRG")
      return BM_GBRG;
    else if (bayer == "GRBG")
      return BM_GRBG;
    return BM_NONE;
    }

  template <class T>
  image<T> expand(const image<T>& im, size_t border_size, T border_color)
    {
    image<T> out(im.width() + 2 * (int32_t)(border_size), im.height() + 2 * (int32_t)(border_size), false);

    T* d = out.data();
    T* d_end = out.data() + out.stride() * border_size;
    for (; d != d_end; ++d)
      *d = border_color;
    d = out.end() - out.stride() * border_size;
    d_end = out.end();
    for (; d != d_end; ++d)
      *d = border_color;

    jtk::parallel_for((int32_t)(border_size), (int32_t)out.height() - (int32_t)(border_size), [&](int32_t r)
      {
      T* d = out.data() + r * out.stride();
      T* d_end = d + border_size;
      for (; d != d_end; ++d)
        *d = border_color;
      d_end = out.data() + (r + 1) * out.stride() - border_size - (out.stride() - out.width());
      const T* o = im.data() + (r - border_size) * im.stride();
      for (; d != d_end; ++d, ++o)
        *d = *o;
      d_end = out.data() + (r + 1) * out.stride();
      for (; d != d_end; ++d)
        *d = border_color;
      });
    return out;
    }

  image<uint64_t> bilinear(const image<uint16_t>& raw, EBayerMatrixType iBayer)
    {
    image<uint16_t> im = expand<uint16_t>(raw, 1, 0);
    int w = raw.width();
    int h = raw.height();
    int w1 = im.stride();
    image<uint64_t> out(w, h, false);

    jtk::parallel_for(int(0), h, [&](int y)
      {
      uint16_t* p_src = im.data() + w1 + 1 + y * w1;
      uint64_t* p_dst = out.data() + y * out.stride();
      EBayerMatrixType Bayer = iBayer;
      if (y % 2 == 1)
        Bayer = GetBayerMatrixDown(iBayer);
      uint16_t r1, r2, r3, r4, g1, g2, g3, g4, b1, b2, b3, b4;
      uint16_t r, g, b;

      for (int x = 0; x < w; ++x, ++p_src, ++p_dst)
        {
        switch (Bayer)
          {
          case BM_BGGR:
            // R1  G1  R2
            // G2 [B ] G3
            // R3  G4  R4
            r1 = p_src[-w1 - 1];  g1 = p_src[-w1]; r2 = p_src[-w1 + 1];
            g2 = p_src[-1];                   g3 = p_src[+1];
            r3 = p_src[+w1 - 1];  g4 = p_src[+w1]; r4 = p_src[+w1 + 1];

            r = (r1 + r2 + r3 + r4) / 4;
            g = (g1 + g2 + g3 + g4) / 4;
            b = p_src[0];
            break;

          case BM_GBRG:
            // G   R1  G
            // B1 [G ] B2
            // G   R2  G
            r1 = p_src[-w1];
            b1 = p_src[-1];                b2 = p_src[+1];
            r2 = p_src[+w1];

            r = (r1 + r2) / 2;
            g = p_src[0];
            b = (b1 + b2) / 2;
            break;

          case BM_GRBG:
            // G   B1  G
            // R1 [G ] R2
            // G   B2  G
            b1 = p_src[-w1];
            r1 = p_src[-1];                  r2 = p_src[+1];
            b2 = p_src[+w1];

            r = (r1 + r2) / 2;
            g = p_src[0];
            b = (b1 + b2) / 2;
            break;

          default: // BM_RGGB:
            // B1  G1  B2
            // G2 [R ] G3
            // B3  G4  B4
            b1 = p_src[-w1 - 1];  g1 = p_src[-w1];  b2 = p_src[-w1 + 1];
            g2 = p_src[-1];                    g3 = p_src[+1];
            b3 = p_src[+w1 - 1];  g4 = p_src[+w1];  b4 = p_src[+w1 + 1];

            r = p_src[0];
            g = (g1 + g2 + g3 + g4) / 4;
            b = (b1 + b2 + b3 + b4) / 4;
            break;
          }
        Bayer = GetBayerMatrixRight(Bayer);
        uint64_t col = 0xffff000000000000 | (uint64_t)b << 32 | (uint64_t)g << 16 | (uint64_t)r;        
        *p_dst = col;
        }
      });
    return out;
    }


  void scale_image(image<uint64_t>& im, double scale)
    {
    for (auto& clr : im)
      {
      const uint64_t red = (uint64_t)((clr&0xffff)*scale) & 0xffff;
      const uint64_t green = (uint64_t)(((clr>>16) & 0xffff) * scale) & 0xffff;
      const uint64_t blue = (uint64_t)(((clr>>32) & 0xffff) * scale) & 0xffff;
      clr = 0xffff000000000000 | (blue << 32) | (green << 16) | red;
      }
    }

  image<uint32_t> clamp_to_rgb(const image<uint64_t>& im)
    {
    image<uint32_t> out(im.width(), im.height());
    for (uint32_t y = 0; y < im.height(); ++y)
      {
      uint32_t* a = out.data() + y*out.stride();
      const uint64_t* b = im.data() + y*im.stride();
      const uint64_t* const b_end = b + im.width();
      for (; b != b_end; ++b, ++a)
        {
        const uint64_t col = *b;
        uint64_t red = col & 0xff;
        uint64_t green = (col >> 16) & 0xff;
        uint64_t blue = (col >> 32) & 0xff;
        *a = 0xff000000 | ((uint32_t)blue << 16) | ((uint32_t)green << 8) | (uint32_t)red;
        }
      }
    return out;
    }

  }