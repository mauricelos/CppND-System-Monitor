#include <numeric>

#include "linux_parser.h"
#include "processor.h"

float Processor::Utilization() {
  auto cpu_util_list = LinuxParser::CpuUtilization();
  const auto idle_time =
      cpu_util_list[LinuxParser::kIdle_] + cpu_util_list[LinuxParser::kIOwait_];
  const auto total_time =
      std::accumulate(cpu_util_list.begin(), cpu_util_list.end(), 0);
  const float idle_time_delta = idle_time - prev_idle_time_;
  const float total_time_delta = total_time - prev_total_time_;

  prev_idle_time_ = idle_time;
  prev_total_time_ = total_time;

  return 1.0 - idle_time_delta / total_time_delta;
}
