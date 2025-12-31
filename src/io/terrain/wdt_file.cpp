#include "wdt_file.h"

#include <unordered_map>

#include "spdlog/spdlog.h"

namespace wow::io::terrain {
    wdt_file::wdt_file(const utils::binary_reader_ptr &reader) {
        std::unordered_map<uint32_t, utils::binary_reader_ptr> chunks{};

        while (!reader->eof()) {
            const auto signature = reader->read<uint32_t>();
            const auto size = reader->read<uint32_t>();
            std::vector<uint8_t> data(size);
            reader->read(data.data(), size);
            chunks[signature] = utils::make_binary_reader(data);
        }

        if (!chunks.contains('MPHD')) {
            SPDLOG_ERROR("WDT file missing MPHD chunk");
            throw std::runtime_error("WDT file missing MPHD chunk");
        }

        const auto mphd = chunks['MPHD'];
        if (mphd->size() != sizeof(wdt_header)) {
            SPDLOG_ERROR("WDT file has invalid MPHD chunk size");
            throw std::runtime_error("WDT file has invalid MPHD chunk size");
        }

        _header = mphd->read<wdt_header>();
    }
}
