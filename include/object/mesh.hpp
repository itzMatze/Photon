#pragma once

#include <cassert>
#include <cstdint>

class Mesh
{
public:
  Mesh() = default;
  Mesh(int32_t material_idx, uint32_t idx_offset, uint32_t idx_count) : material_id(material_idx), triangle_index_offset(idx_offset / 3), triangle_index_count(idx_count / 3)
  {
    assert(idx_offset % 3 == 0);
    assert(idx_count % 3 == 0);
  }

  int32_t material_id;
  uint32_t triangle_index_offset;
  uint32_t triangle_index_count;
};
