#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id) : pid_(id) {
    cpuUtil_ = LinuxParser::CpuUtilization(id);
    command_ = LinuxParser::CpuUtilization(id);
};
// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() {  
    cpuUtil_ = LinuxParser::CpuUtilization(pid_);
    ram_ = Ram();


    return cpuUtil_;
 }

// Return the command that generated this process
string Process::Command() {
    command_ = LinuxParser::Command(pid_);
    return command_;
}

// Return this process's memory utilization
string Process::Ram() {
    ram_ = LinuxParser::Ram(pid_);
    return ram_;
}

// Return the user (name) that generated this process
string Process::User() {
    user_ = LinuxParser::User(pid_);
    return user_;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    uptime_ = LinuxParser::UpTime(pid_);
    return uptime_;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return cpuUtil_ > a.cpuUtil_; }