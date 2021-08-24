#include "format.h"

#include <iomanip>
#include <string>

using std::setfill;
using std::setw;
using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hour;
  long minutes;
  long secs;
  string timeFormat;

  std::ostringstream timeStream;

  hour = seconds / 3600;
  seconds = seconds % 3600;
  minutes = seconds / 60;
  seconds = seconds % 60;
  secs = seconds / 1;

  timeStream << setw(2) << setfill('0') << hour << ":" << setw(2)
             << setfill('0') << minutes << ":" << setw(2) << setfill('0')
             << secs;
  return timeStream.str();
}