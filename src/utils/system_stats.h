#pragma once

#include <cstdint>

namespace wow::utils {
    struct system_stats {
        int32_t cpu_usage; // percentage
        int64_t memory_usage; // bytes
        int64_t total_memory; // bytes
        int32_t gpu_usage; // percentage
    };

    system_stats get_system_stats();
}
