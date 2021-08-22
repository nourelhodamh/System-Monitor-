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

  cpuVals = LinuxParser::CpuUtilization();

  // if (prevTotal_ != 0) {
    active = stof(cpuVals[0]);
    idle = stof(cpuVals[1]);

    prevTotal_ = prevActive_ + prevIdle_;
    total = active + idle;

    totalDelta = total - prevTotal_;
    idleDelta = idle - prevIdle_;

    cpuUtil = (totalDelta - idleDelta) / totalDelta;
  // }

  prevActive_ = stof(cpuVals[0]);
  prevIdle_ = stof(cpuVals[1]);

  return 0;
}
