#include "processor.h"
#include "linux_parser.h"


Processor::Processor() : jiffies_(LinuxParser::Jiffies()), active_jiffies_(LinuxParser::ActiveJiffies()) {};
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    float util = (float)active_jiffies_/jiffies_;
    return util;
}