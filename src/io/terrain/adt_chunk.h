#ifndef WOW_UNIX_ADT_CHUNK_H
#define WOW_UNIX_ADT_CHUNK_H

#include <cstdint>
#include <memory>

#include "utils/io.h"

namespace wow::io::terrain {
    class adt_chunk {
    public:
        adt_chunk(uint32_t x, uint32_t y, utils::binary_reader_ptr reader);
    };

    using adt_chunk_ptr = std::shared_ptr<adt_chunk>;
}

#endif //WOW_UNIX_ADT_CHUNK_H
