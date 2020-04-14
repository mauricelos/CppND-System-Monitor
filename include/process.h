#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();
  std::string User();
  std::string Command();
  double CpuUtilization() const;
  std::string Ram();
  long int UpTime() const;
  bool operator<(const Process& a) const;

 private:
  int pid_{0};
};

#endif
