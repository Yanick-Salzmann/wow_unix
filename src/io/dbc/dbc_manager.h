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
        void initialize(const mpq_manager_ptr &mpq_manager);
    };

    typedef std::shared_ptr<dbc_manager> dbc_manager_ptr;
}

#endif //WOW_UNIX_DBC_MANAGER_H
