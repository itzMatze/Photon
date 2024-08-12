#include "image/image_file_handler.hpp"
#include <ctime>
#include <fstream>
#include <filesystem>
#include <vector>
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

void write_image(const Bitmap& bitmap, std::string image_path, FileType type)
{
  if (type == FileType::png)
  {
    image_path.append(".png");
    std::vector<uint32_t> hex_pixels = bitmap.get_hex_vector();
    stbi_write_png(image_path.c_str(), bitmap.get_resolution().x, bitmap.get_resolution().y, 4, hex_pixels.data(), bitmap.get_resolution().x * 4);
  }
  else if (type == FileType::ppm)
  {
    image_path.append(".ppm");
    std::ofstream file(image_path, std::ios::out | std::ios::binary);
    file << "P3\n" << bitmap.get_resolution().x << " " << bitmap.get_resolution().y << "\n" << 255 << "\n";
    for (uint32_t y = 0; y < bitmap.get_resolution().y; y++)
    {
      for (uint32_t x = 0; x < bitmap.get_resolution().x; x++)
      {
        // extract rgb values from hex representation and print to the file
        uint32_t pixel = bitmap.get(y, x).get_hex_color();
        file << (pixel & 0x000000ff) << " ";
        pixel >>= 8;
        file << (pixel & 0x000000ff) << " ";
        pixel >>= 8;
        file << (pixel & 0x000000ff) << "\t";
      }
      file << "\n";
    }
    file.close();
  }
}

void save_single_image(const Bitmap& bitmap, const std::string& name, FileType type)
{
  std::string image_path("images/tmp/" + name);
  std::filesystem::path path(image_path);
  std::filesystem::path dir = path.parent_path();
  if (!dir.empty() && !std::filesystem::exists(dir)) {
    std::filesystem::create_directories(dir);
  }
  if (!name.empty())
  {
    image_path.append("_");
  }
  // add current time to filename
  std::time_t time = std::time(nullptr);
  std::tm local_time = *std::localtime(&time);
  char time_chars[20];
  std::strftime(time_chars, sizeof(time_chars), "%d-%m-%Y_%H-%M-%S", &local_time);
  std::string time_string(time_chars);
  image_path.append(time_string);
  write_image(bitmap, image_path, type);
}

Bitmap load_image(const unsigned char* pixels, uint32_t width, uint32_t height, uint32_t channels)
{
  std::vector<uint32_t> pixel_vector;
  for (uint32_t i = 0; i < width * height * channels; i += channels)
  {
    // if alpha channel is not there, set it to max value
    uint32_t color = (channels == 4) ? pixels[i + 3] : 0xff;
    color <<= 8;
    color |= uint32_t(pixels[i + 2]);
    color <<= 8;
    color |= uint32_t(pixels[i + 1]);
    color <<= 8;
    color |= uint32_t(pixels[i]);
    pixel_vector.emplace_back(color);
  }
  return Bitmap(pixel_vector, width, height);
}

Bitmap load_image(const std::string& path)
{
  int width, height, channels;
  unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 0);
  assert(channels == 3 || channels == 4);
  Bitmap bitmap = load_image(pixels, width, height, channels);
  stbi_image_free(pixels);
  return bitmap;
}

ImageSeries::ImageSeries(const std::string& directory, FileType type) : dir("images/tmp/" + directory), type(type)
{
  if (!std::filesystem::exists(dir)) std::filesystem::create_directory(dir);
}

void ImageSeries::save_image(const Bitmap& bitmap, uint32_t frame_idx)
{
  std::filesystem::path path(dir / std::to_string(frame_idx));
  write_image(bitmap, path.string(), type);
}

