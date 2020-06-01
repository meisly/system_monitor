#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    float util = LinuxParser::ActiveJiffies() / LinuxParser::Jiffies();
    return util;
}