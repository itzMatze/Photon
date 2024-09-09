#pragma once

#include "util/log.hpp"
#include "util/random_generator.hpp"
#include <numeric>
#include <vector>

template<typename Precision = double>
class DiscreteDistribution
{
public:
  DiscreteDistribution()
  {
    probabilities.push_back(0.0);
    probabilities.push_back(1.0);
  }

  template<typename T>
  DiscreteDistribution(const std::vector<T>& weights)
  {
    double total_weight = std::reduce(weights.begin(), weights.end());
    probabilities.push_back(0.0);
    for (T weight : weights)
    {
      probabilities.push_back(probabilities.back() + double(weight) / total_weight);
    }
  }

  uint32_t sample(RandomGenerator& rnd) const
  {
    Precision random_num = rnd.random_float();
    for (uint32_t i = 1; i < probabilities.size(); i++)
    {
      if (random_num < probabilities[i]) return i - 1;
    }
    PH_ASSERT(false, "Failed to sample index!");
    return 0;
  }

  Precision get_probability(uint32_t index) const
  {
    PH_ASSERT(index < probabilities.size() - 1 && index >= 0, "Index outside of probabilities list!");
    return probabilities[index + 1] - probabilities[index];
  }

private:
  std::vector<Precision> probabilities;
};
