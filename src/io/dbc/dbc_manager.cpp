#include "dbc_manager.h"

namespace wow::io::dbc {
    void dbc_manager::initialize(const mpq_manager_ptr &mpq_manager) {
        _map_dbc = make_dbc<map_record>(mpq_manager->open("DBFilesClient\\Map.dbc"));
        _loading_screen_dbc = make_dbc<loading_screen_record>(mpq_manager->open("DBFilesClient\\LoadingScreens.dbc"));
    }
}
