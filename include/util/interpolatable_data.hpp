#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

template<typename T>
concept Interpolatable = requires(const T& a, const T& b, float weight)
{
  { interpolate(a, b, weight) } -> std::same_as<T>;
};

// stores a list of interpolatable elements to interpolate between two lists of elements properly
template<Interpolatable T>
class InterpolatableData
{
public:
  InterpolatableData() = default;
  InterpolatableData(const std::vector<T>& data, const std::vector<uint32_t>& ids) : data(data), ids(ids)
  {
    assert(this->data.size() == this->ids.size());
    // make sure the ids are unique
#if !defined(NDEBUG)
    for (uint32_t i = 1; i < ids.size(); i++)
    {
      assert(ids[i - 1] < ids[i]);
    }
#endif
    next_id = ids.size() > 0 ? ids.back() + 1 : 0;
  }

  uint32_t add_new_data(const T& datum)
  {
    // assign ascending indices to objects
    data.emplace_back(datum);
    ids.emplace_back(next_id++);
    return ids.back();
  }

  T& get_element(uint32_t id)
  {
    uint32_t idx = std::min(id, uint32_t(ids.size() - 1));
    while (id < ids[idx]) idx--;
    while (id > ids[idx]) idx++;
    assert(idx < ids.size() && id == ids[idx]);
    return data[idx];
  }

  void remove_element(uint32_t id)
  {
    uint32_t idx = std::min(id, uint32_t(ids.size() - 1));
    while (id < ids[idx]) idx--;
    while (id > ids[idx]) idx++;
    assert(idx < ids.size() && id == ids[idx]);
    data.erase(data.begin() + idx);
    ids.erase(ids.begin() + idx);
  }

  const std::vector<T>& get_data() const
  {
    return data;
  }

  const std::vector<uint32_t>& get_ids() const
  {
    return ids;
  }

private:
  std::vector<T> data;
  std::vector<uint32_t> ids;
  uint32_t next_id;
};

template<typename T>
InterpolatableData<T> interpolate(const InterpolatableData<T>& a, const InterpolatableData<T>& b, float weight)
{
  std::vector<T> result_data;
  std::vector<uint32_t> result_ids;
  const std::vector<T>& data_a = a.get_data();
  const std::vector<uint32_t>& ids_a = a.get_ids();
  const std::vector<T>& data_b = b.get_data();
  const std::vector<uint32_t>& ids_b = b.get_ids();

  assert(data_a.size() == ids_a.size() && data_b.size() == ids_b.size());
  uint32_t i = 0, j = 0;
  while (i < data_a.size() && j < data_b.size())
  {
    if (ids_a[i] == ids_b[j])
    {
      result_data.emplace_back(interpolate(data_a[i], data_b[j], weight));
      result_ids.emplace_back(ids_a[i]);
      i++;
      j++;
    }
    else if (ids_a[i] < ids_b[j])
    {
      // add data from previous keyframe that are not present in the next keyframe
      result_data.emplace_back(data_a[i]);
      result_ids.emplace_back(ids_a[i]);
      i++;
    }
    // additional data in the next keyframe will be added when that keyframe is reached
    else if (ids_a[i] > ids_b[j]) j++;
  }
  // add remaining objects
  while (i < data_a.size())
  {
    result_data.emplace_back(data_a[i]);
    result_ids.emplace_back(ids_a[i]);
    i++;
  }
  return InterpolatableData(result_data, result_ids);
}

