#include <cstdint>
#include <fstream>
#include <ostream>
#include <regex>

#include "t_file_parser.hpp"

using namespace std;

regex time_pat{R"((0\d|1\d|2[0-3]):[0-5]\d)"};
regex event_id_pat{R"(\d|\d\d)"};
regex body_pat{R"(^\w+$|^\w+\s\d+)"};
regex postv_int_pat{R"(^[1-9]+$|[1-9]\d+$)"};

/**
 * @brief   checks IO state from ifstream
 * @returns 0 on success, 1 if reached EOF, -1 if IO error occured.
 */
inline int check_io_state(ifstream &file) {
  if (!file.good()) {
    if (file.eof()) {
      /* cout << "Reached EOF!\n"; */
      return 1;
    } else if (file.bad()) {
      cout << "IO state: badbit\n";
    } else if (file.fail()) {
      cout << "IO state: failbit\n";
    }
    return -1;
  }
  return 0;
}

inline void perror_in_line(string &line, uint64_t lnum, const char *err) {
  cout << "Error (line " << lnum << "): " << '"' << line << '"' << ". " << err
       << endl;
}

/**
 * @brief   get line from file
 * @returns 0 on success, 1 if reached EOF, -1 if IO error occured.
 */
inline int get_line(ifstream &file, string &line) {
  int ret = 1;
  getline(file, line);
  if ((ret = check_io_state(file))) {
    return ret;
  }
  return 0;
}

/**
 * @brief assigns substring from ln to wrd, begining from pos0 until
 * delimeter met. If delimeter == 0 assigns whole ln from pos0.
 * @returns position of next word right after specified delimeter.
 * If delimeter == '\n' or not specified returns 0 after successfull assignment.
 * If delimeter not found returns string::npos.
 */
inline string::size_type get_word(const string &ln, string &wrd,
                                  string::size_type pos0,
                                  const char delimeter) {
  if (delimeter) {
    string::size_type posn{0}, n{0};
    if ((posn = ln.find_first_of(delimeter, pos0)) == string::npos) {
      return string::npos;
    }
    n = posn - pos0;
    wrd.assign(ln.substr(pos0, n));
    return delimeter == '\n' ? 0 : posn + 1;
  }
  wrd.assign(ln.substr(pos0));
  return 0;
}

/**
 * @brief   parses event line according to event pattern
 * @returns true if line successfully parsed, false on wrong input.
 */
bool parse_event_line(string &line, uint64_t lnum, gclub::event &new_event,
                      gclub::time_hh_mm &prev_event_time, uint32_t max_dskps) {
  gclub::time_hh_mm time;
  uint8_t event_id{0};
  string body, word;

  unsigned long convres{0};
  size_t nconv{0};
  string::size_type pos0{0};

  smatch match_time;
  if ((pos0 = get_word(line, word, pos0, ' ')) == string::npos) {
    perror_in_line(line, lnum, "Wrong input.");
    return false;
  }
  if (!std::regex_match(word, match_time, time_pat)) {
    perror_in_line(line, lnum, "Wrong time format!");
    return false;
  }

  time.set_from_string(word);
  if (time.get_time_in_mm() <= prev_event_time.get_time_in_mm()) {
    perror_in_line(line, lnum,
                   "Wrong time. Must be greater than previous event time!");
    return false;
  }

  if ((pos0 = get_word(line, word, pos0, ' ')) == string::npos) {
    perror_in_line(line, lnum, "Wrong input.");
    return false;
  }
  smatch match_event_id;
  if (!std::regex_match(word, match_event_id, event_id_pat)) {
    perror_in_line(line, lnum, "Wrong event ID format!");
    return false;
  }

  convres = stoul(word, &nconv, 10);
  if (word.size() > nconv) {
    perror_in_line(line, lnum, "Wrong event ID!");
    return false;
  }
  if (convres >= 1 && convres <= 4 || convres >= 11 && convres <= 13) {
    event_id = convres;
  } else {
    perror_in_line(line, lnum, "Wrong event ID!");
    return false;
  }

  if ((pos0 = get_word(line, word, pos0, 0)) == string::npos) {
    perror_in_line(line, lnum, "Wrong input.");
    return false;
  }

  int desktop_id{0};
  string client_id;

  smatch body_match;
  if (!std::regex_match(word, body_match, body_pat)) {
    perror_in_line(line, lnum, "Wrong body!");
    return false;
  }
  if ((pos0 = word.find(" ")) != string::npos) {
    string dskp_id = word.substr(pos0 + 1);
    if (event_id == 2 || event_id == 12) {
      smatch positive_int;
      if (!std::regex_match(dskp_id, positive_int, postv_int_pat)) {
        perror_in_line(line, lnum, "Wrong input.");
        return false;
      }
      desktop_id = stoi(dskp_id, &nconv, 10);
      if (desktop_id <= 0 || desktop_id > max_dskps) {
        perror_in_line(line, lnum, "Wrong desktop number!");
        return false;
      }
    } else {
      perror_in_line(line, lnum,
                     "Specified event ID does not require desktop ID!");
      return false;
    }
  }
  new_event.set_id(event_id);
  new_event.set_time(time);
  new_event.set_body(word);
  return true;
}

/**
 * @brief reads file line by line from ifstream.
 * @returns on success return 0. If EOF met before parse_line() returns 1. If
 * file IO error occures returns -1. If input does not match specified format
 * returns 2.
 */
int parse_test_file(char *fname, struct gclub::input_params &test_data) {
  string line, word;
  uint64_t lnum{1};
  std::size_t nconv{0};
  int ret{1};

  ifstream file(fname);
  if (file.is_open()) {
    /// Get max desktops
    if ((ret = get_line(file, line))) {
      perror_in_line(line, lnum, "Unable to get a word.");
      return ret;
    }
    smatch positive_int;
    if (!std::regex_match(line, positive_int, postv_int_pat)) {
      perror_in_line(line, lnum, "Wrong input.");
      return 2;
    }
    long res = stol(line, &nconv, 10);
    if (line.size() > nconv) {
      perror_in_line(line, lnum, "Wrong input.");
      return 2;
    }
    if (res > 0) {
      test_data.max_desktops = res;
    } else {
      perror_in_line(line, lnum, "Wrong number of desktops.");
      return 2;
    }

    /// Get working hours
    lnum++;
    line.clear();
    smatch time_match;
    const char delim_pat[]{' ', 0};

    if ((ret = get_line(file, line))) {
      perror_in_line(line, lnum, "Unable to get a word.");
      return ret;
    }

    string::size_type pos0{0};
    for (unsigned i = 0; i < sizeof(delim_pat); i++) {
      if ((pos0 = get_word(line, word, pos0, delim_pat[i])) == string::npos) {
        perror_in_line(line, lnum, "Unable to get a word.");
        return 2;
      }
      if (!std::regex_match(word, time_match, time_pat)) {
        perror_in_line(line, lnum, "Wrong time format.");
        return 2;
      } else {
        if (i == 0) {
          test_data.opens_at.set_from_string(word);
        } else {
          test_data.closes_at.set_from_string(word);
        }
      }
    }
    /* cout << test_data.opens_at.get_string_time() << " " */
    /*      << test_data.closes_at.get_string_time() << endl; */

    /// Get cost per hour
    lnum++;
    line.clear();
    if ((ret = get_line(file, line))) {
      perror_in_line(line, lnum, "Unable to get a word.");
      return 2;
    }
    if (!std::regex_match(line, positive_int, postv_int_pat)) {
      perror_in_line(line, lnum, "Wrong input.");
      return 2;
    }
    res = stol(line, &nconv, 10);
    if (line.size() > nconv) {
      perror_in_line(line, lnum, "Wrong input.");
    }
    if (res > 0) {
      test_data.cost_per_h = res;
    } else {
      perror_in_line(line, lnum, "Wrong cost per hour value.");
      return 2;
    }
    /* cout << test_data.cost_per_h << endl; */

    lnum++;
    gclub::event new_event;
    gclub::time_hh_mm reftime;
    int pres = 0;
    line.clear();
    while ((pres = get_line(file, line)) == 0) {
      if (!parse_event_line(line, lnum, new_event, reftime,
                            test_data.max_desktops)) {
        if (test_data.event_list.empty()) {
          return 2;
        } else {
          test_data.event_list.clear();
          return 2;
        }
      }
      reftime.copy_time_from(new_event.get_time());
      test_data.event_list.push_front(new_event);
      lnum++;
    }
    if (pres == 1) {
      return 0;
    } else if (pres == -1) {
      return 1;
    }
  } else {
    cout << "Unable to open %s" << fname << endl;
    return 1;
  }

  return 0;
}

/// For debugging
void read_desktop(gclub::desktop *tbl) {
  string total_busy_tm = tbl->get_string_total_busy_tm();
  cout << "Desktop ID: " << tbl->get_id() << endl;
  cout << "Status: " << (tbl->is_busy() ? "Busy" : "Vacant") << endl;
  cout << "Daily revenue: " << tbl->get_daily_rev() << endl;
  cout << "Total busy time: " << total_busy_tm << endl;
}

void read_client(gclub::client &cl) {
  string session_start = cl.get_string_session_start();
  cout << "Client: " << cl.get_name() << endl;
  cout << "Session started at: " << session_start << endl;
  cout << "Assigned desktop: " << cl.get_assigned_dskp_id() << endl;
}
