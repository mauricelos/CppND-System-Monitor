#include <unistd.h>
#include <string>

#include "linux_parser.h"
#include "process.h"

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() { return pid_; }

double Process::CpuUtilization() const {
  return (LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) /
         static_cast<double>(Process::UpTime());
}

std::string Process::Command() { return LinuxParser::Command(pid_); }

std::string Process::Ram() { return LinuxParser::Ram(pid_); }

std::string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(const Process& a) const {
  return this->CpuUtilization() > a.CpuUtilization();
}
