#include <string>

#include "format.h"

namespace {

std::string time_padding(std::string time_segment, const int padding = 2) {
  if (time_segment.length() < 2) {
    time_segment.insert(time_segment.begin(), padding - time_segment.size(),
                        '0');
  }

  return time_segment;
};

}  // namespace

std::string Format::ElapsedTime(long seconds) {
  return time_padding(std::to_string(seconds / 3600)) + ":" +
         time_padding(std::to_string((seconds / 60) % 60)) + ":" +
         time_padding(std::to_string(seconds % 60));
}