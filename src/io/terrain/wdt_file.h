#ifndef WOW_UNIX_WDT_FILE_H
#define WOW_UNIX_WDT_FILE_H
#include "utils/io.h"

#include <memory>

namespace wow::io::terrain {
#pragma pack(push, 1)
    struct wdt_header {
        uint32_t flags;
        uint32_t unk1[7];
    };
#pragma pack(pop)

    class wdt_file {
        wdt_header _header{};

    public:
        explicit wdt_file(const utils::binary_reader_ptr &reader);

        bool has_large_alpha() const {
            return (_header.flags & 0x84) != 0;
        }
    };

    using wdt_file_ptr = std::shared_ptr<wdt_file>;

    inline wdt_file_ptr make_wdt(const utils::binary_reader_ptr &reader) {
        return std::make_shared<wdt_file>(reader);
    }
}

#endif //WOW_UNIX_WDT_FILE_H
