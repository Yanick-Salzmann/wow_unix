#ifndef WOW_UNIX_ADT_CHUNK_H
#define WOW_UNIX_ADT_CHUNK_H

#include <atomic>
#include <memory>

#include "gl/index_buffer.h"
#include "gl/vertex_buffer.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "utils/io.h"
#include "utils/math.h"

namespace wow::io::terrain {
    class adt_chunk {
#pragma pack(push, 1)
        struct map_chunk_flags {
            uint32_t has_mcsh: 1;
            uint32_t impass: 1;
            uint32_t liq_river: 1;
            uint32_t liq_ocean: 1;
            uint32_t liq_magma: 1;
            uint32_t liq_slime: 1;
            uint32_t has_mccv: 1;
            uint32_t unk1: 1;
            uint32_t unk2: 1;
            uint32_t unfixed_alpha_map: 1;
            uint32_t high_res_holes: 1;
            uint32_t unk3: 1;
            uint32_t unk4: 20;
        };

        struct map_chunk_header {
            map_chunk_flags flags{};
            uint32_t index_x{};
            uint32_t index_y{};
            uint32_t num_layers{};
            uint32_t num_doodads{};
            uint32_t ofs_heights{};
            uint32_t ofs_normals{};
            uint32_t ofs_layer{};
            uint32_t ofs_refs{};
            uint32_t ofs_alpha{};
            uint32_t size_alpha{};
            uint32_t ofs_shadow{};
            uint32_t size_shadow{};
            uint32_t area_id{};
            uint32_t num_wmo{};
            uint16_t holes{};
            uint16_t unk1{};
            uint64_t detail_doodads1{};
            uint64_t detail_doodads2{};
            uint64_t no_effect_doodad{};
            uint32_t ofs_snd_emitters{};
            uint32_t num_snd_emitters{};
            uint32_t ofs_liquid{};
            uint32_t size_liquid{};
            glm::vec3 position{};
            uint32_t ofs_mccv{};
            uint32_t ofs_mclv{};
            uint32_t padding{};
        };

        struct adt_vector {
            glm::vec3 position{};
            glm::vec3 normal{};
            glm::vec2 tex_coord{};
            glm::vec2 alpha_coord{};
        };

#pragma pack(pop)

        static gl::index_buffer_ptr _index_buffer;
        gl::vertex_buffer_ptr _vertex_buffer{};

        std::atomic_bool _is_async_loaded = false;
        bool _is_sync_loaded = false;
        bool _sync_load_requested = false;

        utils::bounding_box _bounds{};

        map_chunk_header _header{};

        std::array<adt_vector, 145> _vectors{};

        void load_heights(const utils::binary_reader_ptr &reader);

        void load_normals(const utils::binary_reader_ptr &reader);

        void sync_load();

    public:
        explicit adt_chunk(const utils::binary_reader_ptr &reader);

        [[nodiscard]] std::pair<uint32_t, uint32_t> index() const {
            return {_header.index_x, _header.index_y};
        }

        void on_frame();

        [[nodiscard]] const utils::bounding_box &bounds() const {
            return _bounds;
        }
    };

    using adt_chunk_ptr = std::shared_ptr<adt_chunk>;
}

#endif //WOW_UNIX_ADT_CHUNK_H
