#pragma once
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "Common/defs.h"
#include "Common/NonCopyable.h"
#include "Common/NonMovable.h"

namespace Utils {
    
    /* ---------------- domain-specific categories ---------------- */
    enum class ProfileCategory : uint8_t
    {
        Platform = 0,      // window + input
        Scene,             // building or updating world data
        Film,              // CPU buffer writes / tonemap
        Transfer,          // staging-buffer copies
        Vulkan,            // command-buffer / queue submits
        Misc               // anything else
    };
    
    /* --------- one record = one timing event (nanoseconds) ------ */
    struct ProfileSample { long long duration; };
    
    class Profiler : private NonCopyable, private NonMovable
    {
    public:
        Profiler(std::string_view section,
                 ProfileCategory category = ProfileCategory::Misc);
        ~Profiler();
    
        static void LogSummary();  /* call once on exit */
    
    private:
        using Clock      = std::chrono::high_resolution_clock;
        using TimePoint  = std::chrono::time_point<Clock>;
    
        ProfileCategory m_category;
        std::string     m_name;
        TimePoint       m_start;
    
        static std::unordered_map<
                  ProfileCategory,
                  std::unordered_map<std::string, std::vector<ProfileSample>>
               > s_records;
        static std::mutex s_mutex;
    
        static void logRecord(ProfileCategory cat,
                              const std::string& name,
                              long long duration);
    
        static std::string categoryToString(ProfileCategory);
        static std::string formatDuration(long long ns);
    };

}

/* -------------------- compile-time helpers ------------------ */
#define CAT_PRIV(a,b) a##b
#define CAT(a,b) CAT_PRIV(a,b)
#define UNIQUE_VAR(prefix) CAT(prefix,__LINE__)

#ifdef UTILS_RunProfile
    #define PROFILE_FUNCTION(cat) \
        Utils::Profiler UNIQUE_VAR(_prof_){ __FUNCTION__, Utils::ProfileCategory::cat }

    #define PROFILE_SCOPE(cat, name) \
        Utils::Profiler UNIQUE_VAR(_prof_){ name, Utils::ProfileCategory::cat }

    /* shorthands for Misc category */
    #define PROFILE_FUNCTION_AUTO() PROFILE_FUNCTION(Misc)
    #define PROFILE_SCOPE_AUTO(name) PROFILE_SCOPE(Misc, name)
#else
    #define PROFILE_FUNCTION(cat)   ((void)0)
    #define PROFILE_SCOPE(cat,name) ((void)0)
    #define PROFILE_FUNCTION_AUTO() ((void)0)
    #define PROFILE_SCOPE_AUTO(x)   ((void)0)
#endif
