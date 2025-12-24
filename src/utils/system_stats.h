#ifndef WOW_UNIX_SYSTEM_STATS_H
#define WOW_UNIX_SYSTEM_STATS_H

#include <cstdint>

namespace wow::utils {
    struct system_stats {
        int32_t cpu_usage; // percentage
        int64_t memory_usage; // bytes
        int64_t total_memory; // bytes
        int32_t gpu_usage; // percentage
        int32_t cpu_frequency_mhz; 
        int64_t gpu_memory_used; // bytes
        int64_t gpu_memory_total; // bytes
    };

    system_stats get_system_stats();
}

#endif // WOW_UNIX_SYSTEM_STATS_H