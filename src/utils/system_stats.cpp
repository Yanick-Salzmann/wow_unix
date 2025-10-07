#include "system_stats.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <memory>
#include <array>

#include "string_utils.h"

/**
 * This code is mainly AI generated and reviewed manually
 */
namespace wow::utils {
    enum class gpu_type {
        UNKNOWN,
        NVIDIA,
        AMD,
        INTEL
    };

    static uint64_t prev_total = 0;
    static uint64_t prev_idle = 0;
    static auto cached_gpu_type = gpu_type::UNKNOWN;
    static bool gpu_type_detected = false;

    gpu_type detect_gpu_type() {
        if (gpu_type_detected) {
            return cached_gpu_type;
        }

        std::array<char, 128> buffer;

        const std::unique_ptr<FILE, decltype(&pclose)> pipe(
            popen("lspci | grep -i vga", "r"),
            pclose
        );

        if (pipe) {
            std::string result;
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            result = to_lower(result);

            if (result.find("nvidia") != std::string::npos) {
                cached_gpu_type = gpu_type::NVIDIA;
            } else if (result.find("amd") != std::string::npos || result.find("ati") != std::string::npos) {
                cached_gpu_type = gpu_type::AMD;
            } else if (result.find("intel") != std::string::npos) {
                cached_gpu_type = gpu_type::INTEL;
            }
        }

        gpu_type_detected = true;
        return cached_gpu_type;
    }

    static int32_t get_nvidia_gpu_usage() {
        const std::unique_ptr<FILE, decltype(&pclose)> pipe(
            popen("nvidia-smi --query-gpu=utilization.gpu --format=csv,noheader,nounits", "r"),
            pclose
        );

        if (pipe) {
            std::array<char, 128> buffer;
            std::string result;

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            return std::stoi(result);
        }

        return 0;
    }

    static int32_t get_amd_gpu_usage() {
        if (std::ifstream gpu_file("/sys/class/drm/card0/device/gpu_busy_percent");
            gpu_file.is_open()) {
            int32_t usage;
            gpu_file >> usage;
            return usage;
        }

        const std::unique_ptr<FILE, decltype(&pclose)> pipe(
            popen("radeontop -d - -l 1 2>/dev/null | grep -oP 'gpu \\K[0-9]+'", "r"),
            pclose
        );

        if (pipe) {
            std::array<char, 128> buffer;
            std::string result;

            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            return std::stoi(result);
        }
        return 0;
    }

    static auto get_intel_gpu_usage() -> int32_t {
        const std::unique_ptr<FILE, decltype(&pclose)> pipe(
            popen("intel_gpu_top -J -s 100 2>/dev/null | grep -oP '\"Render/3D/0\": \\{\"busy\": \\K[0-9.]+'", "r"),
            pclose
        );

        if (pipe) {
            std::array<char, 128> buffer;
            std::string result;
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }

            return static_cast<int32_t>(std::stof(result));
        }

        return 0;
    }

    system_stats get_system_stats() {
        system_stats stats{};

        if (std::ifstream stat_file("/proc/stat");
            stat_file.is_open()) {
            std::string line;
            std::getline(stat_file, line);

            std::istringstream iss(line);
            std::string cpu;
            iss >> cpu;

            uint64_t user{}, nice{}, system{}, idle{}, iowait{}, irq{}, softirq{}, steal{}, guest{}, guest_nice{};
            iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

            const auto total = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
            const auto idle_time = idle + iowait;

            if (prev_total > 0) {
                const auto total_diff = total - prev_total;
                const auto idle_diff = idle_time - prev_idle;
                if (total_diff > 0) {
                    stats.cpu_usage = static_cast<int32_t>((100 * (total_diff - idle_diff)) / total_diff);
                }
            }

            prev_total = total;
            prev_idle = idle_time;
        }

        if (std::ifstream mem_file("/proc/meminfo");
            mem_file.is_open()) {
            std::string line;
            uint64_t total_mem = 0, available_mem = 0;

            while (std::getline(mem_file, line)) {
                std::istringstream iss(line);
                std::string key;
                uint64_t value;
                std::string unit;
                iss >> key >> value >> unit;

                if (key == "MemTotal:") {
                    total_mem = value * 1024;
                } else if (key == "MemAvailable:") {
                    available_mem = value * 1024;
                }
            }

            stats.memory_usage = total_mem - available_mem;
            stats.total_memory = total_mem;
        }

        switch (detect_gpu_type()) {
            case gpu_type::NVIDIA:
                stats.gpu_usage = get_nvidia_gpu_usage();
                break;
            case gpu_type::AMD:
                stats.gpu_usage = get_amd_gpu_usage();
                break;
            case gpu_type::INTEL:
                stats.gpu_usage = get_intel_gpu_usage();
                break;
            default:
                stats.gpu_usage = 0;
                break;
        }

        return stats;
    }
}
