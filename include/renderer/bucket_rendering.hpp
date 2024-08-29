#pragma once

#include <atomic>
#include <vector>

#include "scene/scene_file_handler.hpp"
#include "util/vec2.hpp"

struct ImageBucket
{
  glm::uvec2 min;
  glm::uvec2 max;
};

class BucketRendering
{
public:
  BucketRendering(const SceneFile& scene_file);
  bool get_next_bucket(ImageBucket& bucket);

private:
  std::atomic<uint32_t> bucket_idx;
  std::vector<ImageBucket> buckets;
};
