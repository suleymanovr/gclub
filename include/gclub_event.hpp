#pragma once

#include "gclub_time.hpp"

namespace gclub {
class event {
private:
  time_hh_mm time;
  uint8_t id;
  std::string body;

public:
  event() {
    id = 0;
    body = {0};
  }

  void set_id(uint8_t val) { id = val; }
  void set_time(const time_hh_mm &from) { time = from; }
  void set_body(std::string nbody) { body.assign(nbody); }

  uint8_t get_id() { return id; }
  time_hh_mm get_time() { return time; }
  std::string get_body() { return body; }
};
}; // namespace gclub
