#ifndef WOW_UNIX_DBC_MANAGER_H
#define WOW_UNIX_DBC_MANAGER_H

#include "dbc_structs.h"
#include "dbc_file.h"
#include "io/mpq_manager.h"

namespace wow::io::dbc {
    class dbc_manager {
        dbc_file_ptr<map_record> _map_dbc{};
        dbc_file_ptr<loading_screen_record> _loading_screen_dbc{};
        dbc_file_ptr<area_poi_record> _area_poi_dbc{};
        dbc_file_ptr<area_table_record> _area_table_dbc{};
        dbc_file_ptr<light_record> _light_dbc{};
        dbc_file_ptr<light_params_record> _light_params_dbc{};
        dbc_file_ptr<light_skybox_record> _light_skybox_dbc{};

    public:
        void initialize(const mpq_manager_ptr &mpq_manager,
                        const std::function<void(int, const std::string &)> &callback);

        [[nodiscard]] const dbc_file_ptr<map_record> &map_dbc() const {
            return _map_dbc;
        }

        [[nodiscard]] const dbc_file_ptr<loading_screen_record> &loading_screen_dbc() const {
            return _loading_screen_dbc;
        }

        [[nodiscard]] const dbc_file_ptr<area_poi_record> &area_poi_dbc() const {
            return _area_poi_dbc;
        }

        [[nodiscard]] const dbc_file_ptr<area_table_record> &area_table_dbc() const {
            return _area_table_dbc;
        }

        [[nodiscard]] const dbc_file_ptr<light_record> &light_dbc() const {
            return _light_dbc;
        }

        [[nodiscard]] const dbc_file_ptr<light_params_record> &light_params_dbc() const {
            return _light_params_dbc;
        }

        [[nodiscard]] const dbc_file_ptr<light_skybox_record> &light_skybox_dbc() const {
            return _light_skybox_dbc;
        }
    };

    using dbc_manager_ptr = std::shared_ptr<dbc_manager>;
}

#endif //WOW_UNIX_DBC_MANAGER_H
