#pragma once
//Recreated
#include "Core/Debug/Log.h"




#include <chrono>
#include <string>
#include <unordered_map>
#include <iostream>

class ScopedProfilee
{
public:
    static std::mutex profilemutex;
    ScopedProfilee(const char* name)
        : m_name(name),
        m_start(std::chrono::high_resolution_clock::now())
    {
        std::lock_guard lg(profilemutex);
        activeProfiles.push_back(0);

    }

    ~ScopedProfilee()
    {
        std::lock_guard lg(profilemutex);
        auto end = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(
            end - m_start
        ).count();

        double exclusive = ms - activeProfiles.back();

        auto& total = timings[m_name];
        total.inclusiveTime += ms;
        total.exclusiveTime += exclusive;
        total.calls++;

        if (ms > 10.0)
        {
            LOGW("[PROFILE] %s took %.2f ms\n", m_name.c_str(), ms);
        }

        activeProfiles.pop_back();

        if (!activeProfiles.empty())
        {
            activeProfiles.back() += ms;//this is fine since remember, ms is inclusive time
        }
    }

    struct Timing
    {
        double inclusiveTime = 0;
        double exclusiveTime = 0;
        uint64_t calls = 0;
    };

    static void beginreport()
    {
        std::lock_guard lg(profilemutex);
        for (auto& t : timings)
        {
            t.second.calls = 0;
            t.second.inclusiveTime = 0;
            t.second.exclusiveTime = 0;
        }
        startReport = std::chrono::high_resolution_clock::now();
    }

    static void report()
    {
        std::lock_guard lg(profilemutex);
        std::vector<std::pair<std::string,Timing>> sorted;

        auto endReport = std::chrono::high_resolution_clock::now();

        double ms = std::chrono::duration<double, std::milli>(
            endReport - startReport
        ).count();


        for (auto& t : timings)
        {
            sorted.push_back(t);
            t.second.calls = 0;
            t.second.inclusiveTime = 0;
            t.second.exclusiveTime = 0;
        }

        std::sort(sorted.begin(), sorted.end(), [](std::pair<std::string, Timing>& a, std::pair<std::string, Timing>& b)
            {
                return a.second.exclusiveTime > b.second.exclusiveTime;
            });

        for (auto& t : sorted)
        {
            printf("[PROFILE] Task %s had %i calls and took %f/%.2f seconds inclusive, %f/%.2f seconds exclusive\n",t.first.c_str(), (int)t.second.calls, t.second.inclusiveTime / 1000.0, ms / 1000.0, t.second.exclusiveTime / 1000.0, ms / 1000.0);
        }
    }
    
    static std::chrono::high_resolution_clock::time_point startReport;
    static std::unordered_map<std::string, Timing> timings;
    static std::vector<double> activeProfiles;

private:
    std::string m_name;
    std::chrono::high_resolution_clock::time_point m_start;
};


#define ScopedProfile(name) ScopedProfilee profiler___(name)