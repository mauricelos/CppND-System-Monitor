#include <dirent.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "linux_parser.h"

std::string LinuxParser::OperatingSystem() {
  std::string line, key, value;
  std::ifstream filestream(kOSPath);
  if (!filestream.fail()) {
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

std::string LinuxParser::Kernel() {
  std::string os, version, kernel, line;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (!filestream.fail()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      std::string filename(file->d_name);
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
  float mem_total, mem_free;
  std::string key, value, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (!filestream.fail()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        mem_total = key == "MemTotal" ? std::stof(value) : mem_total;
        mem_free = key == "MemFree" ? std::stof(value) : mem_free;
      }
    }
  }

  return (mem_total - mem_free) / mem_total;
}

long LinuxParser::UpTime() {
  long up_time;
  std::string uptime, line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (!filestream.fail()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    up_time = std::stol(uptime);
  }

  return up_time;
}

long LinuxParser::ActiveJiffies(int pid) {
  long index{0}, active_jiffies{0};
  std::string value, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (!filestream.fail()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (index >= 13 && index <= 16) {
        active_jiffies += std::stol(value);
      }
      ++index;
    }
  }

  return active_jiffies;
}

std::vector<long> LinuxParser::CpuUtilization() {
  std::string value, line;
  std::vector<long> jiffies{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  filestream.ignore(5, ' ');
  if (!filestream.fail()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      jiffies.push_back(std::stol(value));
    }
  }

  return jiffies;
}

int LinuxParser::TotalProcesses() {
  int num_total_processes{0};
  std::string key, value, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (!filestream.fail()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        num_total_processes =
            (key == "processes" ? std::stoi(value) : num_total_processes);
      }
    }
  }

  return num_total_processes;
}

int LinuxParser::RunningProcesses() {
  int num_running_processes{0};
  std::string key, value, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (!filestream.fail()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        num_running_processes =
            (key == "procs_running" ? std::stoi(value) : num_running_processes);
      }
    }
  }

  return num_running_processes;
}

std::string LinuxParser::Command(int pid) {
  std::string command, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (!filestream.fail()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::replace(line.begin(), line.end(), ' ', '*');
      while (linestream >> command) {
        std::replace(command.begin(), command.end(), '*', ' ');
        break;
      }
    }
  }

  return command;
}

std::string LinuxParser::Ram(int pid) {
  std::string key, value, line, memory_usage;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (!filestream.fail()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        memory_usage =
            (key == "VmSize:" ? std::to_string(std::stoi(value) / 1024)
                              : memory_usage);
      }
    }
  }

  return memory_usage;
}

std::string LinuxParser::Uid(int pid) {
  std::string key, value, uid, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (!filestream.fail()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        uid = (key == "Uid:" ? value : uid);
      }
    }
  }

  return uid;
}

std::string LinuxParser::User(int pid) {
  std::string user, password, uid, line;
  std::ifstream filestream(kPasswordPath);
  if (!filestream.fail()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> password >> uid) {
        if (uid == LinuxParser::Uid(pid)) {
          return user;
        }
      }
    }
  }

  return user;
}

long LinuxParser::UpTime(int pid) {
  long index{0}, pid_time{0};
  std::string value, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (!filestream.fail()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (index == 21) {
        pid_time = std::stol(value) / sysconf(_SC_CLK_TCK);
        break;
      }
      ++index;
    }
  }

  return LinuxParser::UpTime() - pid_time;
}
