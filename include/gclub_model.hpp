#pragma once

#include <list>

#include "gclub_admin.hpp"
#include "gclub_client.hpp"
#include "gclub_desktop.hpp"
#include "gclub_event.hpp"
#include "gclub_time.hpp"

namespace gclub {
struct input_params {
  uint32_t max_desktops;
  uint32_t cost_per_h;
  time_hh_mm opens_at;
  time_hh_mm closes_at;
  std::list<event> event_list;

  input_params() {
	max_desktops = 0;
	cost_per_h 	 = 0;
  };
};

inline void print_event(const std::string &time, const unsigned &id,
                        const std::string &body);
inline void split_body(const std::string &body, std::string &name,
                       uint32_t &deskp);

void end_of_the_day(admin &adm);
/// OUTPUT events
void ev13_error(const std::string &time, const std::string &error);
void ev12_client_landed(time_hh_mm &time, client &cl_land, uint32_t dskp_id,
                        admin &adm);
void ev11_client_leave(const std::string &time, const std::string &client_name);

/// INPUT events
void ev4_client_leave(event &ev_leave, admin &adm);
void ev3_client_wait(event &ev_wait, admin &adm);
void ev2_client_landed(event &ev_land, admin &adm);
void ev1_client_came(event &ev_came, admin &adm);
void handle_event(event &new_event, admin &adm);
}; // namespace gclub
