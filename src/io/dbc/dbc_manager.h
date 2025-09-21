#ifndef WOW_UNIX_DBC_MANAGER_H
#define WOW_UNIX_DBC_MANAGER_H

#include "dbc_structs.h"
#include "dbc_file.h"
#include "io/mpq_manager.h"

namespace wow::io::dbc {
    class dbc_manager {
        dbc_file_ptr<map_record> _map_dbc{};
        dbc_file_ptr<loading_screen_record> _loading_screen_dbc{};

    public:
        void initialize(const mpq_manager_ptr &mpq_manager,
                        const std::function<void(int, const std::string &)> &callback);

        const dbc_file_ptr<map_record> &map_dbc() const {
            return _map_dbc;
        }

        const dbc_file_ptr<loading_screen_record> &loading_screen_dbc() const {
            return _loading_screen_dbc;
        }
    };

    typedef std::shared_ptr<dbc_manager> dbc_manager_ptr;
}

#endif //WOW_UNIX_DBC_MANAGER_H
