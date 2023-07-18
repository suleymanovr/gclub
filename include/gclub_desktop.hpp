#pragma once

#include <cstdint>

#include "gclub_desktop.hpp"
#include "gclub_time.hpp"

namespace gclub {
class desktop {
private:
  uint32_t id;
  uint32_t daily_revenue;
  bool busy;
  time_hh_mm total_busy_tm;

public:
  desktop() {
    id = 0;
    daily_revenue = 0;
    busy = 0;
  };
  desktop &operator=(desktop &);

  void set_id(uint32_t val) { id = val; }
  uint32_t get_id() { return id; }

  void append_daily_rev(uint32_t val) { daily_revenue += val; }
  uint32_t get_daily_rev() { return daily_revenue; }

  void set_busy() { busy = 1; }
  void set_not_busy() { busy = 0; }
  bool is_busy() { return busy; }

  time_hh_mm get_total_busy_tm() { return total_busy_tm; }
  std::string get_string_total_busy_tm() {
    return total_busy_tm.get_string_time();
  }

  void append_total_busy_tm(uint8_t hh_val, uint8_t mm_val) {
    total_busy_tm.append_time(hh_val, mm_val);
  }
};
}; // namespace gclub
