#include <iostream>

#include "CmdList.h"
#include "raw_to_rgb.h"

#define JTK_IMAGE_IMPLEMENTATION
#include "jtk/image.h"

#define JTK_FILE_UTILS_IMPLEMENTATION
#include "jtk/file_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"



void copy_directory_structure(const std::string& /*dir_from*/, const std::string& /*dir_to*/)
  {
  std::cout << "NOT IMPLEMENTED";
  exit(-1);
  }

int32_t str_to_int(const std::string& str)
  {
  std::stringstream sstr;
  sstr << str;
  int32_t v;
  sstr >> v;
  return v;
  }

jtk::image<uint32_t> read_image(const std::string& filename)
  {
  int w, h, nr_of_channels;
  unsigned char* im = stbi_load(filename.c_str(), &w, &h, &nr_of_channels, 4);
  if (im)
    {
    jtk::image<uint32_t> im32 = jtk::span_to_image(w, h, w, (const uint32_t*)im);
    stbi_image_free(im);
    return im32;
    }   
  return jtk::image<uint32_t>();
  }

bool write_to_file(const jtk::image<uint32_t>& texture, const std::string& filename)
  {
  std::string ext = jtk::get_extension(filename);
  if (ext.empty())
    return false;
  std::transform(ext.begin(), ext.end(), ext.begin(), [](char ch) {return (char)::tolower(ch); });
  if (ext == "png")
    {
    if (!stbi_write_png(filename.c_str(), texture.width(), texture.height(), 4, (void*)texture.data(), texture.stride() * 4))
      return false;
    }
  else if (ext == "jpg" || ext == "jpeg")
    {
    if (!stbi_write_jpg(filename.c_str(), texture.width(), texture.height(), 4, (void*)texture.data(), 80))
      return false;
    }
  else if (ext == "bmp")
    {
    if (!stbi_write_bmp(filename.c_str(), texture.width(), texture.height(), 4, (void*)texture.data()))
      return false;
    }
  else if (ext == "tga")
    {
    if (!stbi_write_tga(filename.c_str(), texture.width(), texture.height(), 4, (void*)texture.data()))
      return false;
    }
  else
    return false;
  return true;
  }



bool load_image(jtk::image<uint32_t>& im, const std::string& filename, std::string bayer, int bits_per_pixel)
  {
  auto ext = jtk::get_extension(filename);
  if (ext == "jpg")
    {
    im = read_image(filename);
    return true;
    }
  else if (ext == "png")
    {
    im = read_image(filename);
    return true;
    }
  else if (ext == "bmp")
    {
    im = read_image(filename);
    return true;
    }
  else if (ext == "pgm")
    {
    jtk::image<uint16_t> raw;
    if (!jtk::load_pgm(raw, filename))
      return false;
    jtk::EBayerMatrixType bayer_type = jtk::BM_NONE;
    if (bayer == "GRBG")
      bayer_type = jtk::BM_GRBG;
    else if (bayer == "GBRG")
      bayer_type = jtk::BM_GBRG;
    else if (bayer == "RGGB")
      bayer_type = jtk::BM_RGGB;
    else if (bayer == "BGGR")
      bayer_type = jtk::BM_BGGR;
    auto im_clr = jtk::bilinear(raw, bayer_type);
    double scale = 255.0 / (double(1 << bits_per_pixel) - 1.0);
    jtk::scale_image(im_clr, scale);
    im = jtk::clamp_to_rgb(im_clr);      
    return true;
    }
  else if (ext == "ppm")
    {
    return false;
    }
  return false;
  }

bool save_image(const jtk::image<uint32_t>& im, const std::string& filename)
  {
  auto ext = jtk::get_extension(filename);
  if (ext == "jpg")
    {
    printf("saving to %s\n", filename.c_str());
    write_to_file(im, filename);    
    return true;
    }
  else if (ext == "pgm16")
    {
    printf("saving to %s\n", filename.c_str());
    printf("not implemented");
    return false;
    }
  else if (ext == "png")
    {
    printf("saving to %s\n", filename.c_str());
    write_to_file(im, filename);
    return true;
    }
  else if (ext == "bmp")
    {
    printf("saving to %s\n", filename.c_str());
    write_to_file(im, filename);
    return true;
    }
  else if (ext == "ppm")
    {
    printf("saving to %s\n", filename.c_str());
    printf("not implemented");
    return false;
    }
  else if (ext == "pgm")
    {
    printf("saving to %s\n", filename.c_str());
    return jtk::write_pgm(jtk::image_to_gray(im), filename);
    }
  else
    printf("Don't know the fileformat of %s", filename.c_str());
  return false;
  }

bool load_image(jtk::image<uint8_t>& im, const std::string& filename)
  {
  auto ext = jtk::get_extension(filename);
  if (ext == "pgm")
    {
    return jtk::load_pgm(im, filename);   
    }
  return false;
  }

bool save_image(const jtk::image<uint8_t>& im, const std::string& filename)
  {
  auto ext = jtk::get_extension(filename);
  if (ext == "pgm")
    {
    printf("saving to %s\n", filename.c_str());
    return jtk::write_pgm(im, filename);
    }
  else if (ext == "pgm16")
    {
    printf("saving to %s\n", filename.c_str());
    printf("not implemented");
    return false;
    }
  else if (ext == "jpg")
    {
    printf("saving to %s\n", filename.c_str());
    return write_to_file(jtk::three_gray_to_uint32_t(im,im,im), filename);    
    }
  else if (ext == "png")
    {
    printf("saving to %s\n", filename.c_str());
    return write_to_file(jtk::three_gray_to_uint32_t(im, im, im), filename);
    }
  else if (ext == "bmp")
    {
    printf("saving to %s\n", filename.c_str());
    return write_to_file(jtk::three_gray_to_uint32_t(im, im, im), filename);
    }
  else if (ext == "ppm")
    {
    printf("saving to %s\n", filename.c_str());
    printf("not implemented");
    return false;
    }
  else
    printf("Don't know the fileformat of %s", filename.c_str());
  return false;
  }

void operate_files_in_folder(std::vector<std::string>::iterator& args, const std::vector<std::string>::iterator& end,
  const std::function<bool(jtk::image<uint32_t>&, const jtk::image<uint32_t>&, std::vector<std::string>::iterator&, const std::vector<std::string>::iterator&)>& op_on_rgb,
  const std::function<bool(jtk::image<uint8_t>&, const jtk::image<uint8_t>&, std::vector<std::string>::iterator&, const std::vector<std::string>::iterator&)>& op_on_gray)
  {
  std::vector<std::string> operation_args;
  std::string in, out;
  std::string input_ext, output_ext, bayer("BGGR");
  bool subfolders = false;
  bool rename = false;
  int32_t rename_digits = 4;
  int bits_per_pixel = 12;
  if (args == end)
    {
    std::cerr << "Arguments are missing!\n Type 'ImageBatch -?' for instructions" << std::endl;
    return;
    }
  in = *args++;
  if (args == end)
    {
    std::cerr << "Arguments are missing!\n Type 'ImageBatch -?' for instructions" << std::endl;
    return;
    }
  out = *args++;

  while (args != end)
    {
    if (*args == "-s")
      subfolders = true;
    else if (args->substr(0, 3) == "-i:")
      {
      input_ext = args->substr(3);
      }
    else if (args->substr(0, 3) == "-o:")
      {
      output_ext = args->substr(3);
      }
    else if (args->substr(0, 3) == "-b:")
      {
      bayer = args->substr(3);
      }
    else if (args->substr(0, 3) == "-r:")
      {
      rename = true;
      rename_digits = str_to_int(args->substr(3));
      }
    else if (args->substr(0, 5) == "-bpp:")
      {
      bits_per_pixel = str_to_int(args->substr(5));
      }
    else
      operation_args.push_back(*args);
    ++args;
    }
  if (!jtk::is_directory(in))
    {
    std::cerr << in << " is not a folder" << std::endl;
    return;
    }
  if (!jtk::is_directory(out))
    {
    std::cerr << out << " is not a folder" << std::endl;
    return;
    }
  if (subfolders)
    copy_directory_structure(in, out);
  auto files = jtk::get_files_from_directory(in, subfolders);
  int index = 0;
  for (auto f : files)
    {
    if (!input_ext.empty())
      {
      if (jtk::get_extension(f) != input_ext)
        continue;
      }
    jtk::image<uint32_t> im;
    jtk::image<uint8_t> img;

    std::string p;
    if (rename)
      {
      char buf[1024];
      std::stringstream sprintf_arg;
      sprintf_arg << "%s/%0" << rename_digits << "d." << jtk::get_extension(f);
      sprintf(buf, sprintf_arg.str().c_str(), out.c_str(), index);
      p = std::string(buf);
      }
    else
      p = out + f.substr(in.length());
    if (!output_ext.empty())
      {
      p = jtk::remove_extension(p);
      p.append("." + output_ext);
      }
    if (load_image(im, f, bayer, bits_per_pixel))
      {
      jtk::image<uint32_t> im_out;
      printf("working on file %s\n", f.c_str());
      auto it = operation_args.begin();
      auto it_end = operation_args.end();
      if (!op_on_rgb(im_out, im, it, it_end))
        {
        std::cerr << "Something went wrong!\nType 'ImageBatch -?' for instructions" << std::endl;
        return;
        }
      save_image(im_out, p);
      }
    else if (load_image(img, f))
      {
      jtk::image<uint8_t> im_out;
      printf("working on file %s\n", f.c_str());
      auto it = operation_args.begin();
      auto it_end = operation_args.end();
      if (!op_on_gray(im_out, img, it, it_end))
        {
        std::cerr << "Something went wrong!\n Type 'ImageBatch -?' for instructions" << std::endl;
        return;
        }
      save_image(im_out, p);
      }
    ++index;
    }
  }

template <class T>
bool copy(jtk::image<T>& out, const jtk::image<T>& im, std::vector<std::string>::iterator& args, const std::vector<std::string>::iterator& end)
  {
  args; end;
  out = im;
  return true;
  }

void copy_on_files(std::vector<std::string>::iterator& args, const std::vector<std::string>::iterator& end)
  {
  operate_files_in_folder(args, end, copy<uint32_t>, copy<uint8_t>);
  }

int main(int argc, char* argv[])
  {
  if (argc < 2)
    {
    std::cout << "Type 'ImageBatch -?' for instructions" << std::endl;
    exit(0);
    }

  std::vector<std::string> args;
  for (int i = 0; i < argc; ++i)
    args.push_back(argv[i]);

  CmdList cmd("ImageBatch",
    "    -?                             shows this help text",
    "    -s                             include subfolders\n"
    "    -i:<jpg|png|bmp|pgm|ppm>       only open files of given file format\n"
    "    -o:<jpg|png|bmp|pgm|pgm16|ppm> save files to target file format\n"
    "    -r:int                         rename files to 0000.xxx 0001.xxx ...\n"
    "                                   where the number of digits is given by int\n"
    "    -b:<BGGR|RGGB|GRBG|GBRG>       set bayer pattern for raw images\n"
    "    -bpp:int                       set the bits per pixels for raw images");

  cmd.RegCmd("copy",
    "    -copy in out                 copies the files",
    copy_on_files);

  cmd.RunCommands(++args.begin(), args.end());
  return 0;
  }