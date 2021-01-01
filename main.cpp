#include <mbed.h>
#include "Calibration.h"

constexpr uint32_t timer_flag = 1;
constexpr uint32_t print_ticks = 100;

Ticker ticker;
EventFlags event_flags;

AnalogIn adj_io{PA_0};
AnalogIn envel_io{PA_1};
std::array<DigitalOut, 10> leds{PB_12, PB_13, PB_14, PB_15, PA_8,
                                PB_2,  PB_0,  PA_7,  PB_11, PB_10};

void tick() {
  event_flags.set(timer_flag);
}

int main(void) {
  event_flags.clear();
  ticker.attach(&tick, 10ms);

  leds[0] = 1;

  for (;;) {
    CalibrationData c;
    uint16_t cnt[66] = {0};
    uint16_t env_max = std::numeric_limits<uint16_t>::min();
    uint16_t env_min = std::numeric_limits<uint16_t>::max();
    uint32_t env_sum = 0;

    for (uint32_t ticks = 0; ticks < print_ticks; ticks++) {
      event_flags.wait_all(timer_flag);

      auto envel = envel_io.read_u16();

      env_max = std::max(envel, env_max);
      env_min = std::min(envel, env_min);
      env_sum += envel;

      int i = envel / 1000;
      cnt[i] += 1;

      c.add_sample(envel);
    }

    auto adj = adj_io.read_u16();
    auto env_avg = env_sum / print_ticks;
    auto p = c.finalize();

    float fi, ff;
    ff = std::modf(p.mean, &fi);
    uint16_t mi = fi;
    uint16_t mf = ff * 1000;
    ff = std::modf(p.stddev, &fi);
    uint16_t di = fi;
    uint16_t df = ff * 1000;

    printf("adj %d\n", adj);
    printf("mean %d.%03d stddev %d.%03d\n", mi, mf, di, df);
    printf("envel min %d max %d avg %lu\n", env_min, env_max, env_avg);

    char buf[512];
    int offset = snprintf(buf, 512, "cnt ");
    for (int i = 0; i < 66; i++) {
      offset += snprintf(buf + offset, 512 - offset, "%d ", cnt[i]);
    }
    printf("%s\n", buf);

    size_t pos;

    if (env_avg > adj) {
      pos = leds.size();
    } else if (adj == 0) {
      pos = 0;
    } else {
      pos = 10 * env_avg / adj;
    }

    for (size_t i = 0; i < leds.size(); i++) {
      // leds[i] = i <= pos ? 1 : 0;
    }
  }
}
