#pragma once

#include <cstdint>

#include "gclub_time.hpp"

namespace gclub {

enum client_status { NOT_LANDED, LANDED, WAITING };

class client {
private:
  std::string name;
  uint32_t assigned_dskp_id;
  int status;
  time_hh_mm session_start;

public:
  client() {
    assigned_dskp_id = 0;
    status = client_status::NOT_LANDED;
  };

  std::string get_name() { return name; }
  void set_name(std::string &val) { name.assign(val); }

  int get_status() { return status; }
  void set_status(int new_status) { status = new_status; }

  uint32_t get_assigned_dskp_id() { return assigned_dskp_id; }
  void assign_dskp(uint32_t id) { assigned_dskp_id = id; }

  time_hh_mm get_session_start() { return session_start; }
  std::string get_string_session_start() {
    return session_start.string();
  }
  void set_session_start(time_hh_mm &time) { session_start = time; }
};
}; // namespace gclub
