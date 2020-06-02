#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
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

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
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
  string key, value;
  float memTot, memFree, memUsed;
  string line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          memTot = stof (value);
        }else if (key == "MemFree") {
          memFree = stof (value);
        }
      }
    }
  }
  memUsed = memTot - memFree;
  return memUsed / memTot;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string up, idle;
  string line;
  long uptime;
  std::ifstream file(kProcDirectory + kUptimeFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    linestream >> up >> idle;
    uptime = stol (up);
  }
  
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string word;
  string line;
  long jiffies = 0;
  // long jiffies2 = 0;
  vector<string> vec;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    while( linestream >> word) {
      vec.push_back(word);
    }
    for(size_t i = 1; i < vec.size(); i++){
      jiffies += stol(vec[i]);
    }
  }
  // jiffies2 = LinuxParser::UpTime() *  sysconf(_SC_CLK_TCK);

  return jiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string word;
  string line;
  string pidstr = to_string(pid);
  long jiffies = 0;
  vector<string> vec;
  vector<int> jif_indx{13, 14, 15, 16};
  std::ifstream file(kProcDirectory + pidstr + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    while( linestream >> word) {
      vec.push_back(word);
    }
    for(int i: jif_indx){
      jiffies += stol(vec[i]);
    }
  }
  return jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string word;
  string line;
  long jiffies = 0;
  vector<string> vec;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    while( linestream >> word) {
      vec.push_back(word);
    }
    for(int i = 1; i < 11; i++){
      if( i != 4 && i != 5) {
        jiffies += stol(vec[i]);
      }
    }
  }
  return jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string word;
  string line;
  long jiffies = 0;
  vector<string> vec;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    while( linestream >> word) {
      vec.push_back(word);
    }
    jiffies += stol(vec[4]);
    jiffies += stold(vec[5]);
  }
  return jiffies;
}

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) {
  long usage = LinuxParser::ActiveJiffies(pid);
  long system = LinuxParser::Jiffies();
  float percentUtil = (float)usage / system;
  return percentUtil;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  vector<int> pids = LinuxParser::Pids();
  int total = pids.size();
  return total;
}

// Read and return vector of pids for running processes 
vector<int> LinuxParser::ActiveProcesses() {
  vector<int> pids = LinuxParser::Pids();
  vector<int> active_pids = {};
  for(int id: pids){
    string key, value;
    string line;
    std::ifstream file(kProcDirectory + to_string(id) + kStatusFilename);
    if (file.is_open()) {
      while (key != "State:"  && std::getline(file, line)) {
        std::istringstream linestream(line);
        int j = 0;
        while (j++ < 1 && linestream >> key >> value) {
          if (key == "State:") {
            if( value == "R") {
              active_pids.push_back(id);
            } else {
              break;
            }
          }
        }
      }
    }
  }
  return active_pids;
}
// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  vector<int> active_pids = LinuxParser::ActiveProcesses();
  int running = active_pids.size();
  
  return running;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream file(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (file.is_open()) {
    std::getline(file, line);
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  float val_converter;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          val_converter = stof(value) / 1000;
          value = to_string(val_converter);
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string user, spacer, test;
  string uid = LinuxParser::Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> spacer >> test) {
        if (test == uid) {
          return user;
        }
      }
    }
  }
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string word;
  string line;
  string pidstr = to_string(pid);
  long starttime = 0;
  vector<string> vec;
  int start_indx{21};
  float tick = sysconf(_SC_CLK_TCK);
  std::ifstream file(kProcDirectory + pidstr + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    while( linestream >> word) {
      vec.push_back(word);
    }
  }
  starttime = stol(vec[start_indx])  / tick;
  return starttime;
}