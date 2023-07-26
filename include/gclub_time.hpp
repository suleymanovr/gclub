#pragma once

#include <cstdint>
#include <string>

namespace gclub {
class time_hh_mm {
private:
  uint8_t hh;
  uint8_t mm;

public:
  explicit time_hh_mm(uint8_t hh_val = 0, uint8_t mm_val = 0)
      : hh(hh_val % 24), mm(mm_val % 60) {}

  time_hh_mm(const time_hh_mm &val) {
    hh = val.hh;
    mm = val.mm;
  }
  time_hh_mm &operator=(const time_hh_mm &val) {
    hh = val.hh;
    mm = val.mm;
    return *this;
  }

  void set_time(uint8_t hh_val, uint8_t mm_val) {
    hh = hh_val % 24;
    mm = mm_val % 60;
  }
  void append_time(uint8_t hh_val, uint8_t mm_val) {
    hh = (hh + hh_val) % 24;
    mm = (mm + mm_val) % 60;
  }

  uint16_t get_time_in_mm() { return hh * 60 + mm; }
  uint8_t get_time_in_hh() { return mm > 0 ? hh + 1 : hh; }
  uint8_t get_hh() { return hh; }
  uint8_t get_mm() { return mm; }

  time_hh_mm get_delta(const time_hh_mm &start);
  std::string get_string_time();
  void set_from_string(std::string time);
};
}; // namespace gclub
