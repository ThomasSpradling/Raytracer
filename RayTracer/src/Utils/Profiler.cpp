#include "Utils/Profiler.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Utils {
    
    /* ---------- static storage ---------- */
    std::unordered_map<
        ProfileCategory,
        std::unordered_map<std::string, std::vector<ProfileSample>>
    > Profiler::s_records;
    
    std::mutex Profiler::s_mutex;
    
    /* ---------- ctor / dtor measure duration ---------- */
    Profiler::Profiler(std::string_view name, ProfileCategory cat)
        : m_category{cat}, m_name{name},
          m_start{Clock::now()}
    {}
    
    Profiler::~Profiler()
    {
        const auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            Clock::now() - m_start).count();
        logRecord(m_category, m_name, dur);
    }
    
    /* ---------- static helpers ---------- */
    void Profiler::logRecord(ProfileCategory cat,
                             const std::string& name,
                             long long ns)
    {
        std::lock_guard lock(s_mutex);
        s_records[cat][name].push_back({ ns });
    
    #ifdef UTILS_ProfileVerbose
        Logger::Log(Logger::Level::Profile,
                    "({}) {}",
                    name,
                    formatDuration(ns));
    #endif
    }
    
    void Profiler::LogSummary()
    {
        std::lock_guard lock(s_mutex);
        std::ostringstream out;
    
        out << "\n----- Profiler Summary -----\n";
    
        for (auto& [cat, sections] : s_records)
        {
            out << "\n[" << categoryToString(cat) << "]\n";
            out << std::string(70, '-') << '\n';
    
            for (auto& [name, vec] : sections)
            {
                long long total = 0;
                for (auto& s : vec) total += s.duration;
    
                long long avg = total / static_cast<long long>(vec.size());
    
                out << std::setw(36) << std::left << name
                    << "  runs: " << std::setw(6) << vec.size()
                    << "  avg: "  << std::setw(12) << formatDuration(avg)
                    << "  total: " << formatDuration(total)
                    << '\n';
            }
        }
       std::cout << out.str();
    }
    
    /* ---------- formatting + category names ---------- */
    auto Profiler::formatDuration(long long nanoseconds) -> std::string {
      std::ostringstream oss;
      if (nanoseconds < 10000) {
        oss << nanoseconds << " ns";
      } else if (long long microseconds = nanoseconds / 1000; microseconds < 10000) {
        oss << microseconds << " µs";
      } else if (long long milliseconds = microseconds / 1000; milliseconds < 10000) {
        oss << milliseconds << " ms";
      } else {
        long seconds = milliseconds / 1000;
        oss << seconds << " sec";
      }
      return oss.str();
    }
    
    std::string Profiler::categoryToString(ProfileCategory c)
    {
        switch (c)
        {
            case ProfileCategory::Platform:  return "Platform";
            case ProfileCategory::Scene:     return "Scene";
            case ProfileCategory::Film:      return "Film";
            case ProfileCategory::Transfer:  return "Transfer";
            case ProfileCategory::Vulkan:    return "Vulkan";
            default:                         return "Misc";
        }
    }

}
