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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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
  }
  uptime = stol (up);
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string word;
  string line;
  long jiffies = 0;
  long jiffies2 = 0;
  vector<string> vec;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);
    while( linestream >> word) {
      vec.push_back(word);
    }
    for(int i = 1; i < vec.size(); i++){
      jiffies += stol(vec[i]);
    }
  }
  jiffies2 = LinuxParser::UpTime() *  sysconf(_SC_CLK_TCK);

  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the number of active jiffies for the system
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

// TODO: Read and return the number of idle jiffies for the system
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
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  vector<int> pids = LinuxParser::Pids();
  int total = pids.size();
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  vector<int> pids = LinuxParser::Pids();
  int running = 0;
  for(int id: pids){
    string key, value;
    string line;
    std::ifstream file(kProcDirectory + to_string(id) + kStatusFilename);
    if (file.is_open()) {
      while (std::getline(file, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == "State" && value == "R") {
            running +=1 ;
          }
        }
      }
    }
  }
  return running;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }