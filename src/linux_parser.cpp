#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iomanip>  // for std::setprecision()
#include <sstream>
#include <string>
#include <vector>

#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  filestream.close();
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  stream.close();
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  float value;
  float memtotal;
  float memfree;
  std::ifstream memoUtilStream(kProcDirectory + kMeminfoFilename);
  if (memoUtilStream.is_open()) {
    while (std::getline(memoUtilStream, line)) {
      std::stringstream lineStream(line);
      lineStream >> key >> value;
      if (key == filterMemTotalString) {
        memtotal = value;
      } else if (key == filterMemFreeString) {
        memfree = value;
      }
    }
  }
  memoUtilStream.close();
  return (memtotal - memfree) / memtotal;
}

long LinuxParser::UpTime() {
  string line;
  string sysTimeValue;
  std::ifstream totalProcsStream(kProcDirectory + kUptimeFilename);
  if (totalProcsStream.is_open()) {
    std::getline(totalProcsStream, line);
    std::stringstream lineStream(line);
    lineStream >> sysTimeValue;
  }
  totalProcsStream.close();
  return stol(sysTimeValue);
}

long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

long LinuxParser::ActiveJiffies() {
  string line;
  string key;
  string value;
  long userVal{0}, niceVal{0}, systemVal{0}, irqVal{0}, softIrqVal{0},
      stealVal{0};
  long active;
  vector<string> activeCpuArgs{};

  int count = 0;

  std::ifstream utilStream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  if (utilStream.is_open()) {
    while (std::getline(utilStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == filterCpu) {
          while (count < 10) {
            activeCpuArgs.push_back(value);
            count++;
            // lineStream >> userVal >> niceVal >> systemVal >> temp >> temp >>
            //     irqVal >> softIrqVal >> stealVal;
          }
        }
      }
      userVal = stol(activeCpuArgs[LinuxParser::CPUStates::kUser_]);
      niceVal = stol(activeCpuArgs[LinuxParser::CPUStates::kNice_]);
      systemVal = stol(activeCpuArgs[LinuxParser::CPUStates::kSystem_]);
      irqVal = stol(activeCpuArgs[LinuxParser::CPUStates::kIRQ_]);
      softIrqVal = stol(activeCpuArgs[LinuxParser::CPUStates::kSoftIRQ_]);
      stealVal = stol(activeCpuArgs[LinuxParser::CPUStates::kSteal_]);
      active = userVal + niceVal + systemVal + irqVal + softIrqVal + stealVal;
    }
  }
  utilStream.close();
  return active;
}

long LinuxParser::IdleJiffies() {
  string line;
  string key;
  string value;
  long idleVal{0}, iowaitVal{0};
  long idle{0};
  vector<string> idleCpuArgs{};
  int count = 0;

  std::ifstream idleStream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  if (idleStream.is_open()) {
    while (std::getline(idleStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == filterCpu) {
          while (count < 10) {
            idleCpuArgs.push_back(value);
            count++;
            // lineStream >> temp >> temp >> temp >> idleVal >> iowaitVal;
          }
        }
      }
      iowaitVal = stol(idleCpuArgs[LinuxParser::CPUStates::kIOwait_]);
      idleVal = stol(idleCpuArgs[LinuxParser::CPUStates::kIdle_]);
      idle = iowaitVal + idleVal;
    }
  }
  idleStream.close();
  return idle;
}

vector<string> LinuxParser::CpuUtilization() {
  string line;
  string key;
  string value;
  vector<string> cpuValues{};
  float userVal, niceVal, systemVal, irqVal, softIrqVal, stealVal;
  float idleVal, iowaitVal;
  float idle, active;
  int count = 0;

  std::ifstream utilStream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  if (utilStream.is_open()) {
    while (std::getline(utilStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == filterCpu) {
          while (count < 11) {
            cpuValues.push_back(value);
            count++;
          }
        }
      }

      userVal = stof(cpuValues[CPUStates::kUser_]);
      niceVal = stof(cpuValues[CPUStates::kNice_]);
      systemVal = stof(cpuValues[CPUStates::kSystem_]);
      irqVal = stof(cpuValues[CPUStates::kIRQ_]);
      softIrqVal = stof(cpuValues[CPUStates::kSoftIRQ_]);
      stealVal = stof(cpuValues[CPUStates::kSteal_]);
      idleVal = stof(cpuValues[CPUStates::kIdle_]);
      iowaitVal = stof(cpuValues[CPUStates::kIOwait_]);

      active = userVal + niceVal + systemVal + irqVal + softIrqVal + stealVal;
      idle = idleVal + iowaitVal;

      cpuValues.push_back(to_string(active));
      cpuValues.push_back(to_string(idle));
    }
  }
  utilStream.close();
  return cpuValues;
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value{0};
  std::ifstream totalProcsStream(kProcDirectory + kStatFilename);
  if (totalProcsStream.is_open()) {
    while (std::getline(totalProcsStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == filterProcesses) {
          return value;
        }
      }
    }
  }
  totalProcsStream.close();
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value{0};
  std::ifstream runningProcsStream(kProcDirectory + kStatFilename);
  if (runningProcsStream.is_open()) {
    while (std::getline(runningProcsStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == filterRunningProcesses) {
          return value;
        }
      }
    }
  }
  runningProcsStream.close();
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  string pCommand;
  std::ifstream commandStream(kProcDirectory + to_string(pid) +
                              kCmdlineFilename);
  if (commandStream.is_open()) {
    while (std::getline(commandStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> pCommand) {
        return pCommand;
      }
    }
  }
  commandStream.close();
  return string();
}

string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string pRam;
  std::ifstream ramStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (ramStream.is_open()) {
    while (std::getline(ramStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> pRam) {
        if (key == filterProcMem) {  // using "VmRSS" instead of "VmSize"(memory
                                // usage) for exact physical memory being used as
                                // a part of Physical RAM
          return to_string(stol(pRam) / 1000);
        }
      }
    }
  }
  ramStream.close();
  return string();
}

string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream uIdStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (uIdStream.is_open()) {
    while (std::getline(uIdStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == filterUID) {
          return value;
        }
      }
    }
  }
  uIdStream.close();
  return string();
}

string LinuxParser::User(int pid) {
  string line;
  string pUserName;
  string temp;
  string uId;
  std::ifstream userStream(kPasswordPath);
  if (userStream.is_open()) {
    while (std::getline(userStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream lineStream(line);

      while (lineStream >> pUserName >> temp >> uId) {
        if (Uid(pid) == uId) {
          return pUserName;
        }
      }
    }
  }
  userStream.close();
  return string();
}

long LinuxParser::UpTime(int pid) {
  string line;
  vector<string> pUpTime{};
  long startTime{0};
  long sysTime = LinuxParser::UpTime();
  string value;
  std::ifstream pUpTimeStream(kProcDirectory + to_string(pid) + kStatFilename);
  if (pUpTimeStream.is_open()) {
    while (std::getline(pUpTimeStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> value) {
        pUpTime.push_back(value);
      }
      startTime = stol(pUpTime[21]);
    }
  }
  pUpTimeStream.close();
  return sysTime - (startTime / sysconf(_SC_CLK_TCK));
}

float LinuxParser::CpuUtil(int pid) {
  string line;
  string value;
  float total{0.0};
  float seconds;
  float cpuUsage{0.0};

  float hertz = sysconf(_SC_CLK_TCK);

  vector<string> sysUptimeVect{};

  std::ifstream pCpuSystemStream(LinuxParser::kProcDirectory + to_string(pid) +
                                 LinuxParser::kStatFilename);

  if (pCpuSystemStream.is_open()) {
    while (std::getline(pCpuSystemStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> value) {
        sysUptimeVect.push_back(value);
      }
    }
    total = stof(sysUptimeVect[ProcUTIL::Cstime]) +
            stof(sysUptimeVect[ProcUTIL::Cutime]) +
            stof(sysUptimeVect[ProcUTIL::Stime_]) +
            stof(sysUptimeVect[ProcUTIL::Utime_]);
    seconds = LinuxParser::UpTime(pid);
    cpuUsage = ((total / hertz) / seconds);
  }
  pCpuSystemStream.close();
  return cpuUsage;
}

// long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }
