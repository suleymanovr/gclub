#include "gclub_model.hpp"

using namespace std;

inline void gclub::print_event(const string &time, const unsigned &id,
                               const string &body) {
  cout << time << ' ' << id << ' ' << body << endl;
}

inline void gclub::split_body(const string &body, string &name,
                              uint32_t &dskp_id) {
  string::size_type pos = 0;

  pos = body.find(" ");
  name.assign(body.substr(0, pos));
  dskp_id = stoul(body.substr(pos + 1), nullptr, 10);
}

void gclub::ev12_client_landed(time_hh_mm &curr_time, client &land_client,
                               uint32_t deskp_id, admin &adm) {
  land_client.assign_dskp(deskp_id);
  land_client.set_session_start(curr_time);
  land_client.set_status(client_status::LANDED);
  adm.set_dskp_busy(deskp_id);
  adm.update_client_info(land_client);

  string body{land_client.get_name()};
  body.append(" ");
  body.append(to_string(deskp_id));
  print_event(curr_time.string(), 12, body);
}

void gclub::ev11_client_leave(const string &time, const string &client_name) {
  print_event(time, 11, client_name);
}

void gclub::ev13_error(const string &time, const string &error) {
  cout << time << ' ' << "13 " << error << endl;
}

void gclub::ev4_client_leave(event &ev, admin &adm) {
  time_hh_mm curr_time = ev.get_time();
  string client_name{ev.get_body()};
  string str_time{curr_time.string()};
  client client;
  uint32_t dskp_id;

  client.set_name(client_name);
  if (!adm.find_client(client)) {
    print_event(str_time, 4, client_name);
    ev13_error(str_time, "ClientUnknown");
    return;
  }
  if (client.get_status() == client_status::WAITING) {
    print_event(str_time, 4, client_name);
    ev13_error(str_time, "YouShallNotPass!");
    return;
  }

  if (!(dskp_id = client.get_assigned_dskp_id())) {
    print_event(str_time, 4, client_name);
    adm.delete_client(client_name);
    return;
  }
  time_hh_mm session_start = client.get_session_start();
  adm.record_dskp(dskp_id, curr_time, session_start);
  adm.set_dskp_vacant(dskp_id);
  adm.delete_client(client_name);
  print_event(str_time, 4, client_name);

  if (adm.pull_from_queue(client)) {
    ev12_client_landed(curr_time, client, dskp_id, adm);
    return;
  }
}

void gclub::ev3_client_wait(event &ev, admin &adm) {
  time_hh_mm curr_time = ev.get_time();
  string client_name{ev.get_body()};
  string str_time{curr_time.string()};
  client client_wait;

  client_wait.set_name(client_name);
  if (!adm.find_client(client_wait)) {
    print_event(str_time, 3, client_name);
    ev13_error(str_time, "ClientUnknown");
    return;
  }
  if (client_wait.get_assigned_dskp_id()) {
    print_event(str_time, 3, client_name);
    ev13_error(str_time, "ICanWaitNoLonger!");
    return;
  }
  if (client_wait.get_status() == client_status::WAITING) {
    print_event(str_time, 3, client_name);
    ev13_error(str_time, "YouShallNotPass!");
    return;
  }
  if (adm.get_nofvacant()) {
    print_event(str_time, 3, client_name);
    ev13_error(str_time, "ICanWaitNoLonger!");
    return;
  }

  client_wait.set_status(client_status::WAITING);
  adm.update_client_info(client_wait);
  if (adm.add_to_queue(client_wait)) {
    print_event(str_time, 3, client_name);
    return;
  }
  adm.delete_client(client_name);
  print_event(str_time, 3, client_name);
  ev11_client_leave(str_time, client_name);
}

void gclub::ev2_client_landed(event &ev, admin &adm) {
  time_hh_mm curr_time = ev.get_time();
  string body{ev.get_body()};
  string str_time{curr_time.string()};
  string client_name;
  client land_client;
  uint32_t new_deskp_id, old_deskp_id;

  split_body(body, client_name, new_deskp_id);
  land_client.set_name(client_name);
  if (!adm.find_client(land_client)) {
    print_event(str_time, 2, body);
    ev13_error(str_time, "ClientUnknown");
    return;
  }
  if (land_client.get_status() == client_status::WAITING) {
    print_event(str_time, 2, body);
    ev13_error(str_time, "YouShellNotPass");
    return;
  }

  if (adm.dskp_status(new_deskp_id)) {
    print_event(str_time, 2, body);
    ev13_error(str_time, "PlaceIsBusy");
    return;
  }
  if (land_client.get_status() == client_status::LANDED) {
    old_deskp_id = land_client.get_assigned_dskp_id();
    adm.set_dskp_vacant(old_deskp_id);
    time_hh_mm session_start = land_client.get_session_start();
    adm.record_dskp(old_deskp_id, curr_time, session_start);
  } else {
    land_client.set_status(client_status::LANDED);
  }

  land_client.assign_dskp(new_deskp_id);
  land_client.set_session_start(curr_time);
  adm.set_dskp_busy(new_deskp_id);
  adm.update_client_info(land_client);

  print_event(str_time, 2, body);
}

void gclub::ev1_client_came(event &ev, admin &adm) {
  time_hh_mm start = adm.get_open_hrs();
  time_hh_mm fin = adm.get_close_hrs();
  time_hh_mm curr_time = ev.get_time();
  string str_time{curr_time.string()};
  string client_name{ev.get_body()};
  client new_client;

  if (curr_time.get_hh() <= start.get_hh() &&
          curr_time.get_mm() < start.get_mm() ||
      curr_time.get_hh() >= fin.get_hh() && curr_time.get_mm() > fin.get_mm()) {
    print_event(str_time, 1, client_name);
    ev13_error(str_time, "NotOpenYet");
    return;
  }

  if (adm.client_inside(client_name)) {
    print_event(str_time, 1, client_name);
    ev13_error(str_time, "YouShallNotPass");
    return;
  } else {
    new_client.set_name(client_name);
    new_client.set_status(client_status::NOT_LANDED);
    adm.append_client_list(client_name, new_client);
  }
  print_event(str_time, 1, client_name);
}

void gclub::handle_event(event &new_event, admin &adm) {
  switch (new_event.get_id()) {
  case 1:
    ev1_client_came(new_event, adm);
    break;
  case 2:
    ev2_client_landed(new_event, adm);
    break;
  case 3:
    ev3_client_wait(new_event, adm);
    break;
  case 4:
    ev4_client_leave(new_event, adm);
    break;
  }
}

void gclub::end_of_the_day(admin &adm) {
  map<string, client> ordered_list;
  time_hh_mm curr_time = adm.get_close_hrs();
  time_hh_mm session_start;
  string str_time{curr_time.string()};
  uint32_t dskp_id;

  if (!adm.client_list_isempty()) {
    adm.move_client_list_to(ordered_list);

    for (auto client : ordered_list) {
      ev11_client_leave(str_time, client.first);
      if (!(dskp_id = client.second.get_assigned_dskp_id())) {
        continue;
      } else {
        session_start = client.second.get_session_start();
        adm.record_dskp(dskp_id, curr_time, session_start);
        adm.set_dskp_vacant(dskp_id);
      }
    }
  }
  cout << str_time << endl;
  adm.report_desktops();
}

/// time_hh_mm class methods
std::string gclub::time_hh_mm::string() const {
  std::string time;
  if (hh < 10) {
    time.assign("0");
    time.append(std::to_string(hh));
  } else {
    time.assign(std::to_string(hh));
  }
  if (mm < 10) {
    time.append(":0");
    time.append(std::to_string(mm));
  } else {
    time.append(":");
    time.append(std::to_string(mm));
  }
  return time;
}

void gclub::time_hh_mm::set(const std::string time) {
  if (time[0] == '0') {
    hh = std::stoul(time.substr(1, 1), nullptr, 10);
  } else {
    hh = std::stoul(time.substr(0, 2), nullptr, 10);
  }
  if (time[3] == '0') {
    mm = std::stoul(time.substr(4, 1), nullptr, 10);
  } else {
    mm = std::stoul(time.substr(3, 2), nullptr, 10);
  }
}

gclub::time_hh_mm gclub::time_hh_mm::operator-(const time_hh_mm start) {
  time_hh_mm delta{};
  bool mless = false;

  if (mm < start.mm) {
    delta.mm = 60 - (start.mm - mm);
    mless = true;
  } else if (mm > start.mm) {
    delta.mm = mm - start.mm;
  } else {
    delta.mm = 0;
  }

  if (hh < start.hh) {
    delta.hh = 24 - (start.hh - hh);
  } else if (hh > start.hh) {
    delta.hh = hh - start.hh;
  } else {
    delta.hh = 0;
  }

  if (mless && delta.hh == 0) {
    delta.hh = 23;
  } else if (mless) {
    delta.hh -= 1;
  }

  return delta;
}
/// end time_hh_mm class methods
