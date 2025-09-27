#ifndef WOW_UNIX_CONSTANTS_H
#define WOW_UNIX_CONSTANTS_H

namespace wow::utils {
    static constexpr float TILE_SIZE = 533.0f + 1.0f / 3.0f;
    static constexpr float CHUNK_SIZE = TILE_SIZE / 16.0f;
    static constexpr float VERTEX_SIZE = CHUNK_SIZE / 8.0f;

    static constexpr float MAP_MID_POINT = TILE_SIZE * 32.0f;
}

#endif //WOW_UNIX_CONSTANTS_H