#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "renderer/color.hpp"
#include "util/vec2.hpp"

enum class FileType
{
  ppm,
  png
};

void save_single_image(const std::vector<Color>& pixels, const std::string& name, const cm::Vec2u resolution, FileType type = FileType::png);
void load_image(const std::string& path, std::vector<uint32_t>& bitmap, cm::Vec2u& resolution);

class ImageSeries
{
public:
  ImageSeries(const std::string& directory, const cm::Vec2u resolution, FileType type = FileType::png);
  void save_image(const std::vector<Color>& pixels, uint32_t frame_idx);
private:
  const std::filesystem::path dir;
  const cm::Vec2u resolution;
  const FileType type;
};

