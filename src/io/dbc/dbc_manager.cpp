#include "dbc_manager.h"

namespace wow::io::dbc {
    void dbc_manager::initialize(const mpq_manager_ptr &mpq_manager,
                                 const std::function<void(int, const std::string &)> &callback) {
        callback(95, "Loading Map.dbc");
        _map_dbc = make_dbc<map_record>(mpq_manager->open("DBFilesClient\\Map.dbc"));
        callback(95, "Loading LoadingScreens.dbc");
        _loading_screen_dbc = make_dbc<loading_screen_record>(mpq_manager->open("DBFilesClient\\LoadingScreens.dbc"));
        callback(95, "Loading AreaPoi.dbc");
        _area_poi_dbc = make_dbc<area_poi_record>(mpq_manager->open("DBFilesClient\\AreaPoi.dbc"));
        callback(95, "Loading AreaTable.dbc");
        _area_table_dbc = make_dbc<area_table_record>(mpq_manager->open("DBFilesClient\\AreaTable.dbc"));
    }
}
