#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    processes_.clear();  // clear vector to fill it up with new values 
    vector<int> pids = LinuxParser::Pids();
    for(int pid : pids)
        processes_.emplace_back(pid);  // faster then push_back
    std::sort(processes_.begin(), processes_.end()); // using default operator <
    return processes_; 
}

//Return the system's kernel identifier (string)
std::string System::Kernel() {
    return LinuxParser::Kernel();
}

//Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization();
}

//Return the operating system name
std::string System::OperatingSystem() { 
    return LinuxParser::OperatingSystem();
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() {
    uptime_ = LinuxParser::UpTime();
    return uptime_;

}