#include "format.h"

#include <string>

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds [[maybe_unused]]) {
  int hour;
  int minutes;
  int secs;
  string timeFormat;

  hour = seconds / 3600;
  seconds = seconds % 3600;
  minutes = seconds / 60;
  seconds = seconds % 60;
  secs = seconds / 1;
  timeFormat =
      to_string(hour) + ":" + to_string(minutes) + ":" + to_string(secs);

  return timeFormat;
}