#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <string>

class Processor {
 public:
  float Utilization();

 private:
  float prevActive_{0.0};
  float prevIdle_{0.0};
  float prevTotal_{0.0};
};

#endif