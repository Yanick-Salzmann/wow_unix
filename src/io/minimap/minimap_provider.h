#ifndef WOW_UNIX_MINIMAP_PROVIDER_H
#define WOW_UNIX_MINIMAP_PROVIDER_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <mutex>

#include "minimap_loader_pool.h"
#include "io/blp/blp_file.h"
#include "io/dbc/dbc_manager.h"
#include "utils/io.h"

namespace wow::io::minimap {
    class minimap_provider {
        dbc::dbc_manager_ptr _dbc_manager{};
        mpq_manager_ptr _mpq_manager{};
        minimap_loader_pool _loader_pool{};

        std::map<std::string, std::string> _md5_translate{};

        uint32_t _current_map_id = 0xFFFFFFFF;
        std::string _base_path{};

        std::mutex _cache_lock{};
        std::unordered_map<uint64_t, std::vector<uint8_t> > _cache{};

        bool open_from_cache(uint64_t cache_key, std::vector<uint8_t> &out_image);

        void add_to_cache(uint64_t cache_key, const std::vector<uint8_t> &image_data);

        blp::blp_file_ptr open_tile(uint32_t x, uint32_t y) const;

    public:
        minimap_provider(
            dbc::dbc_manager_ptr dbc_manager,
            mpq_manager_ptr mpq_manager
        );

        void switch_to_map(uint32_t map_id);

        void read_image(std::vector<uint8_t> &image_data, uint32_t map_id, uint32_t zoom_level, int32_t tx, int32_t ty);
    };

    typedef std::shared_ptr<minimap_provider> minimap_provider_ptr;
}

#endif //WOW_UNIX_MINIMAP_PROVIDER_H
