#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
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
      while (lineStream >> key >> value) {
        if (key == "MemTotal:") {
          memtotal = value;
        } else if (key == "MemFree:") {
          memfree = value;
        }
      }
    }
  }

  return (memtotal - memfree) / memtotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long sysUptimeValue;
  std::ifstream totalProcsStream(kProcDirectory + kUptimeFilename);
  if (totalProcsStream.is_open()) {
    while (std::getline(totalProcsStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> sysUptimeValue) {
        return sysUptimeValue;
      }
    }
  }
  return sysUptimeValue;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string key;
  string value;
  vector<string> cpuValues{};
  float userVal, niceVal, systemVal, irqVal, softIrqVal, stealVal;
  float idleVal, iowaitVal;
  float idle, active;

  std::ifstream utilStream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  if (utilStream.is_open()) {
    while (std::getline(utilStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == "cpu") {
          userVal = value[CPUStates::kUser_];
          niceVal = value[CPUStates::kNice_];
          systemVal = value[CPUStates::kSystem_];
          irqVal = value[CPUStates::kIRQ_];
          softIrqVal = value[CPUStates::kSoftIRQ_];
          stealVal = value[CPUStates::kSteal_];

          idleVal = value[CPUStates::kIdle_];
          iowaitVal = value[CPUStates::kIOwait_];
        }
      }
      active = userVal + niceVal + systemVal + irqVal + softIrqVal + stealVal;
      idle = idleVal + iowaitVal;

      cpuValues.push_back(to_string(active));
      cpuValues.push_back(to_string(idle));
    }
  }
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
        if (key == "processes") {
          return value;
        }
      }
    }
  }

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
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
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
  return string();
}

string LinuxParser::Ram(int pid) {
  string line;
  string key;
  long pRam;
  std::ifstream ramStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (ramStream.is_open()) {
    while (std::getline(ramStream, line)) {
      std::stringstream lineStream(line);
      while (lineStream >> key >> pRam) {
        if (key == "VmSize:") {
          return to_string(pRam / 1000);
        }
      }
    }
  }

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
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }

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

  return string();
}

long LinuxParser::UpTime(int pid) {
  string line;
  vector<string> pUpTime{};
  int count = 0;
  long processTime;
  std::ifstream pUpTimeStream(kProcDirectory + to_string(pid) + kStatFilename);
  if (pUpTimeStream.is_open()) {
    while (std::getline(pUpTimeStream, line)) {
      std::stringstream lineStream(line);
      while (count <= 21) {
        pUpTime.push_back(line);
        count++;
      }
      processTime = stol(pUpTime[21]);
    }
  }
  return (UpTime() - processTime) / sysconf(_SC_CLK_TCK);
}
float LinuxParser::CpuUtilization(int pid) {
  string line;
  float total{0};
  float seconds;
  float cpuUsage;

  vector<string> sysUptimeVect{};

  std::ifstream pCpuSystemStream(LinuxParser::kProcDirectory + to_string(pid) +
                                 LinuxParser::kStatFilename);
  if (pCpuSystemStream.is_open()) {
    while (std::getline(pCpuSystemStream, line)) {
      std::stringstream lineStream(line);
      sysUptimeVect.push_back(line);
      total = line[13] + line[14] + line[15] + line[16] + line[21];
      seconds = LinuxParser::UpTime(pid) - (line[22] / sysconf(_SC_CLK_TCK));

      cpuUsage = 100 * ((total / sysconf(_SC_CLK_TCK)) / seconds);
    }
  }

  return cpuUsage;
}
