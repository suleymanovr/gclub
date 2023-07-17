#pragma once

#include <cstdint>

#include "gclub_model.hpp"

/**
 * @brief   parses event line accordingly to event pattern
 * @returns true if line successfully parsed, false on wrong input.
 */
bool parse_event_line(std::string &line, uint64_t lnum, gclub::event &new_event,
                      gclub::time_hh_mm &prev_event_time, uint32_t max_dskps);

/**
 * @brief reads file line by line from ifstream.
 * @returns on success return 0. If EOF met before parse_line() returns 1. If
 * file IO error occures returns -1. If input does not match specified format
 * returns 2.
 */
int parse_test_file(char *fname, struct gclub::input_params &test_data);
