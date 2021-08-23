#include "processor.h"

#include "linux_parser.h"
using ::std::stof;
using std::string;
using std::vector;

float Processor::Utilization() {
  float cpuUtil{0.0};
  vector<string> cpuVals;

  float idle;
  float active;
  float total;
  float totalDelta;
  float idleDelta;

  active = LinuxParser::ActiveJiffies();
  idle = LinuxParser::IdleJiffies();
  total = LinuxParser::Jiffies();

  prevTotal_ = prevActive_ + prevIdle_;
  totalDelta = total - prevTotal_;
  idleDelta = idle - prevIdle_;

  cpuUtil = (totalDelta - idleDelta) / totalDelta;

  prevIdle_ = idle;
  prevActive_ = active;

  return cpuUtil;
}
