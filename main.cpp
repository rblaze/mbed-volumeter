#include "mbed.h"

constexpr uint32_t timer_flag = 1;

Ticker ticker;
EventFlags event_flags;

AnalogIn base_io{PA_0};
AnalogIn envel_io{PA_6};
std::array<DigitalOut, 10> leds{PB_12, PB_13, PB_14, PB_15, PA_8,
                                PB_2,  PB_0,  PA_7,  PB_11, PB_10};

void tick() {
  event_flags.set(timer_flag);
}

int main(void) {
  event_flags.clear();
  ticker.attach(&tick, 200ms);

  for (;;) {
    auto base = base_io.read_u16();
    auto envel = envel_io.read_u16();

    printf("%d %d\n", base, envel);
    size_t pos;

    if (envel > base) {
      pos = leds.size();
    } else if (base == 0) {
      pos = 0;
    } else {
      pos = 10 * envel / base;
    }

    for (size_t i = 0; i < leds.size(); i++) {
      leds[i] = i <= pos ? 1 : 0;
    }

    event_flags.wait_all(timer_flag);
  }
}
