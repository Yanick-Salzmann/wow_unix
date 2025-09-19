#ifndef WOW_UNIX_DBC_FILE_H
#define WOW_UNIX_DBC_FILE_H

#include <map>
#include <cstdint>
#include "io/mpq_file.h"
#include "spdlog/spdlog.h"
#include "dbc_structs.h"

#include <boost/pfr.hpp>

#pragma pack(push, 1)

struct dbc_header {
    uint32_t magic;
    uint32_t record_count;
    uint32_t field_count;
    uint32_t record_size;
    uint32_t string_block_size;
};

#pragma pack(pop)

namespace wow::io::dbc {
    template<typename T>
    class dbc_file {
        std::map<int32_t, T> _record_map{};
        std::map<int32_t, std::string> _string_table{};

        dbc_header _header{};

        void load_header(const mpq_file_ptr &file) {
            file->seek(0);
            _header = file->read<dbc_header>();
            if (_header.magic != 0x43424457) {
                SPDLOG_ERROR("Invalid DBC file: 0x{:X} != 0x{:X} ", _header.magic, 0x43424443);
                throw std::runtime_error("Invalid DBC file");
            }

            file->seek(sizeof(dbc_header) + _header.record_count * _header.record_size);
            std::vector<char> string_data(_header.string_block_size);
            file->read(string_data);

            int32_t offset = 0;
            while (offset < string_data.size()) {
                std::string str{string_data.data() + offset};
                _string_table[offset] = str;
                offset += static_cast<int32_t>(str.size() + 1);
            }
        }

        void load_data(const mpq_file_ptr &file) {
            for (int32_t i = 0; i < _header.record_count; ++i) {
                file->seek(sizeof(dbc_header) + i * _header.record_size);

                T record{};
                boost::pfr::for_each_field(record, [&]<typename F>(F &field) {
                    typedef std::remove_cv_t<F> field_type;

                    if constexpr (std::is_same_v<field_type, std::string>) {
                        field = _string_table[file->read<int32_t>()];
                    } else if constexpr (std::is_same_v<field_type, loc_string>) {
                        field = loc_string{};
                        int32_t values[17]{};
                        file->read(values);
                        for (int idx = 0; idx < 16; ++idx) {
                            if (values[idx] != 0) {
                                field.text = _string_table[values[idx]];
                                break;
                            }
                        }
                    } else if constexpr (std::is_same_v<field_type, bool>) {
                        field = file->read<uint8_t>() != 0;
                    } else {
                        field = file->read<field_type>();
                    }
                });

                auto id = boost::pfr::get<0>(record);
                _record_map[id] = record;
            }

            _string_table.clear();
        }

    public:
        explicit dbc_file(const mpq_file_ptr &file) {
            load_header(file);
            load_data(file);
        }

        uint32_t record_count() const {
            return _header.record_count;
        }

        bool has_record(int32_t id) const {
            return _record_map.contains(id);
        }

        T record(int32_t id) const {
            return _record_map.at(id);
        }

        std::map<int32_t, T>::const_iterator begin() const {
            return _record_map.cbegin();
        }

        std::map<int32_t, T>::const_iterator end() const {
            return _record_map.cend();
        }
    };

    template<typename T>
    using dbc_file_ptr = std::shared_ptr<dbc_file<T> >;

    template<typename T>
    dbc_file_ptr<T> make_dbc(mpq_file_ptr file) {
        return std::make_shared<dbc_file<T> >(file);
    }
}


#endif //WOW_UNIX_DBC_FILE_H
