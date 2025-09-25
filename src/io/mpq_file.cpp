#include "mpq_file.h"

#include <stdexcept>

#include "spdlog/fmt/fmt.h"

namespace wow::io {
    mpq_file::mpq_file(const HANDLE file) : _file(file) {
        DWORD size_high = 0;
        uint64_t size = SFileGetFileSize(_file, &size_high);
        size |= static_cast<uint64_t>(size_high) << 32;
        _buffer.resize(size);

        SFileSetFilePointer(_file, 0, nullptr, FILE_BEGIN);
        SFileReadFile(_file, _buffer.data(), size, nullptr, nullptr);
        SFileCloseFile(_file);
    }

    void mpq_file::read(size_t size, void *buffer) {
        if (_offset + size > _buffer.size()) {
            throw std::runtime_error(fmt::format("mpq file read out of bounds: {} + {} > {}", _offset, size,
                                                 _buffer.size()));
        }

        memcpy(buffer, _buffer.data() + _offset, size);
        _offset += size;
    }

    std::string mpq_file::read_text() {
        return std::string{reinterpret_cast<char *>(_buffer.data()), _buffer.size()};
    }

    utils::binary_reader_ptr mpq_file::to_binary_reader() {
        return std::make_shared<utils::binary_reader>(_buffer);
    }
}
