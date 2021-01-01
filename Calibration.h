#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <mbed_assert.h>

struct Point {
  float mean{0};
  float stddev{0};
};

class CalibrationData {
public:
  void add_sample(uint16_t value) {
    count_ += 1;
    float v = value;
    auto delta = v - mean_;
    mean_ += delta / count_;
    auto delta2 = v - mean_;
    m2_ += delta * delta2;
  }

  Point finalize() {
    MBED_ASSERT(count_ > 1);
    auto stddev = std::sqrt(m2_ / (count_ - 1));

    MBED_ASSERT(mean_ >= 0);
    MBED_ASSERT(mean_ <= std::numeric_limits<uint16_t>::max());

    return Point{mean_, stddev};
  }

  int count() {
    return count_;
  }

private:
  int count_{0};
  float mean_{0};
  float m2_{0};
};
