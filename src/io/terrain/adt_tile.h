#ifndef WOW_UNIX_ADT_TILE_H
#define WOW_UNIX_ADT_TILE_H
#include <memory>

#include "utils/io.h"

namespace wow::io::terrain {
    class adt_tile {
    public:
        adt_tile(uint32_t x, uint32_t y, utils::binary_reader &reader) {
        }
    };

    using adt_tile_ptr = std::shared_ptr<adt_tile>;
}

#endif //WOW_UNIX_ADT_TILE_H