#include <ostream>

#include "gclub_model.hpp"
#include "t_file_parser.hpp"

int main(int argc, char *argv[]) {
  struct gclub::input_params test_args;

  if (parse_test_file(argv[1], test_args)) {
    return 1;
  }

  gclub::admin admin{test_args.max_desktops, test_args.cost_per_h,
                     test_args.opens_at, test_args.closes_at};
  gclub::event new_event;
  gclub::time_hh_mm time;

  std::cout << test_args.opens_at.string() << std::endl;
  while (!test_args.event_list.empty()) {
    new_event = test_args.event_list.back();
    gclub::handle_event(new_event, admin);
    test_args.event_list.pop_back();
  }
  gclub::end_of_the_day(admin);

  return 0;
}
