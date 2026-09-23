#include "PerfTimer.h"

std::unordered_map<std::string, ScopedProfilee::Timing> ScopedProfilee::timings;
std::chrono::high_resolution_clock::time_point ScopedProfilee::startReport;
std::vector<double> ScopedProfilee::activeProfiles;