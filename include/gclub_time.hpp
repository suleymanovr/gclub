#pragma once

#include <cstdint>
#include <string>

namespace gclub {
class time_hh_mm {
private:
  uint8_t hh;
  uint8_t mm;

public:
  time_hh_mm(uint8_t hh_val = 0, uint8_t mm_val = 0)
      : hh(hh_val % 24), mm(mm_val % 60) {}

  void copy_time_to(time_hh_mm &copy_to) {
    copy_to.hh = hh;
    copy_to.mm = mm;
  }

  void copy_time_from(const time_hh_mm &copy_from) {
    hh = copy_from.hh;
    mm = copy_from.mm;
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
