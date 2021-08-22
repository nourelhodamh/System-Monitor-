#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();                             
  std::string User();                    
  std::string Command();                  
  float CpuUtilization();                  
  std::string Ram();                     
  long int UpTime();                      
  bool operator<(Process const& a) const;  

 private:
  int pid_{0};
  float cpuUtilization_{0};
  std::string command_{0};
  std::string ram_{};
  std::string user_{};
  long int upTime_{};


};

#endif