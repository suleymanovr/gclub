#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <new>
#include <queue>
#include <unordered_map>

#include "gclub_time.hpp"
#include "gclub_client.hpp"
#include "gclub_desktop.hpp"

namespace gclub {
class admin {
private:
  uint32_t max_dskps;
  uint32_t nofvacant;
  uint32_t cost_per_h;
  desktop *dskps_table;
  time_hh_mm opens_at;
  time_hh_mm closes_at;

  std::unordered_map<std::string, client> client_list;
  std::queue<client> client_queue;

public:
  admin(uint32_t set_max_dskps, uint32_t cost, time_hh_mm opens,
        time_hh_mm closes)
      : max_dskps(set_max_dskps), cost_per_h(cost), opens_at(opens),
        closes_at(closes) {
    dskps_table = new desktop[max_dskps + 1];

    for (uint32_t i = 1; i <= max_dskps; i++) {
      dskps_table[i].set_id(i);
    }
    nofvacant = max_dskps;
  };

  ~admin() { delete[] dskps_table; }

  bool dskp_status(uint32_t id) { return dskps_table[id].is_busy(); }

  void set_dskp_busy(uint32_t id) {
    dskps_table[id].set_busy();
    --nofvacant;
  }

  void set_dskp_vacant(uint32_t id) {
    dskps_table[id].set_not_busy();
    ++nofvacant;
  }

  uint32_t get_nofvacant() { return nofvacant; }

  desktop *get_dskp(uint32_t id) {
    return id <= max_dskps ? &dskps_table[id] : nullptr;
  }

  void record_dskp(uint32_t id, time_hh_mm &curr_time,
                   time_hh_mm &client_session_start) {
    time_hh_mm delta(curr_time - client_session_start);

    desktop *dskp_ptr = get_dskp(id);
    dskp_ptr->append_total_busy_tm(delta.get_hh(), delta.get_mm());
    dskp_ptr->append_daily_rev(delta.get_time_in_hh() * cost_per_h);
  }

  void report_desktops() {
    desktop *ptr;
    time_hh_mm total;
    for (uint32_t id = 1; id <= max_dskps; id++) {
      ptr = get_dskp(id);
      total = ptr->get_total_busy_tm();
      std::cout << ptr->get_id() << ' ' << ptr->get_daily_rev() << ' '
                << total.get_string_time() << std::endl;
    }
  }

  time_hh_mm get_open_hrs() { return opens_at; }
  time_hh_mm get_close_hrs() { return closes_at; }

  bool client_list_isempty() { return client_list.empty(); }

  bool client_inside(const std::string &name) {
    return client_list.find(name) != client_list.end();
  }

  void append_client_list(const std::string &name, const client &new_client) {
    client_list[name] = new_client;
  }

  bool find_client(client &cl) {
    auto memb = client_list.find(cl.get_name());
    if (memb == client_list.end()) {
      return 0;
    }
    cl = memb->second;
    return 1;
  }

  void delete_client(std::string &client_name) {
    client_list.erase(client_name);
  }

  bool update_client_info(client &cl) {
    auto memb = client_list.find(cl.get_name());
    if (memb == client_list.end()) {
      return 0;
    }
    memb->second = cl;
    return 1;
  }

  bool add_to_queue(client &cl) {
    if (client_queue.size() == max_dskps) {
      return 0;
    }
    client_queue.push(cl);
    return 1;
  }

  bool pull_from_queue(client &cl) {
    if (client_queue.empty()) {
      return 0;
    }
    cl = client_queue.front();
    client_queue.pop();
    return 1;
  }

  void move_client_list_to(std::map<std::string, client> &ordered_list) {
    for (auto mv_client : client_list) {
      ordered_list.insert(mv_client);
    }
    client_list.clear();
  }
};
}; // namespace gclub
