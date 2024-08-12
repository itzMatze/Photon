#pragma once

#include <string>
#include <filesystem>
#include "image/bitmap.hpp"
#include "util/vec2.hpp"

enum class FileType
{
  ppm,
  png
};

void save_single_image(const Bitmap& bitmap, const std::string& name, FileType type = FileType::png);
Bitmap load_image(const unsigned char* pixels, uint32_t width, uint32_t height, uint32_t channels);
Bitmap load_image(const std::string& path);

class ImageSeries
{
public:
  ImageSeries(const std::string& directory, FileType type = FileType::png);
  void save_image(const Bitmap& bitmap, uint32_t frame_idx);
private:
  const std::filesystem::path dir;
  const FileType type;
};

