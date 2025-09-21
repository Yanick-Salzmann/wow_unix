#include "io.h"

#include <cstring>
#include <stdexcept>

namespace wow::utils {
    binary_reader::binary_reader(std::vector<uint8_t> data) : _data(std::move(data)) {
    }

    void binary_reader::read(void *data, const size_t size) {
        if (_offset + size > _data.size()) {
            throw std::runtime_error("binary reader read out of bounds");
        }

        memcpy(data, _data.data() + _offset, size);
        _offset += size;
    }
}
