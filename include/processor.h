#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long prev_idle_time_{0}, prev_total_time_{0};
};

#endif
