#include "renderer/bucket_rendering.hpp"

BucketRendering::BucketRendering(const SceneFile& scene_file) : bucket_idx(0)
{
  // divide image into buckets that can be rendered concurrently
  const glm::uvec2 bucket_size = glm::uvec2(scene_file.settings.bucket_size, scene_file.settings.bucket_size);
  const glm::uvec2 bucket_count = (scene_file.settings.resolution / bucket_size);
  // add one overflow bucket if the buckets do not fit the resolution
  const glm::uvec2 overflow_bucket_size = glm::uvec2(scene_file.settings.resolution.x % bucket_size.x, scene_file.settings.resolution.y % bucket_size.y);
  for (uint32_t x = 0; x < bucket_count.x; x++)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * glm::uvec2(x, y), bucket_size * glm::uvec2(x + 1, y + 1));
    }
    if (overflow_bucket_size.y > 0) buckets.emplace_back(bucket_size * glm::uvec2(x, bucket_count.y), bucket_size * glm::uvec2(x + 1, bucket_count.y) + glm::uvec2(0, overflow_bucket_size.y));
  }
  if (overflow_bucket_size.x > 0)
  {
    for (uint32_t y = 0; y < bucket_count.y; y++)
    {
      buckets.emplace_back(bucket_size * glm::uvec2(bucket_count.x, y), bucket_size * glm::uvec2(bucket_count.x, y + 1) + glm::uvec2(overflow_bucket_size.x, 0));
    }
  }
}

bool BucketRendering::get_next_bucket(ImageBucket& bucket)
{
  // atomically get next bucket index in each iteration and check whether the index is still valid
  uint32_t next_idx = bucket_idx.fetch_add(1);
  if (next_idx >= buckets.size()) return false;
  bucket = buckets[next_idx];
  return true;
}
