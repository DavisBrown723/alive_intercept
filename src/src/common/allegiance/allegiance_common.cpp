#include "allegiance_common.hpp"

#include "common\rv.hpp"

#include <string>

using namespace intercept;


namespace alive {
    namespace common {
        namespace allegiance {


            types::side numberToSide(int num_) {
                switch (num_) {
                    case 0:
                        return RV::get().sides.East;
                    case 1:
                        return RV::get().sides.West;
                    case 2:
                        return RV::get().sides.Guer;
                    case 3:
                        return RV::get().sides.Civ;
                    default:
                        return RV::get().sides.East;
                }
            }

            int sideToNumber(types::side side_) {
                if (side_ == RV::get().sides.East)
                    return 0;
                else if (side_ == RV::get().sides.West)
                    return 1;
                else if (side_ == RV::get().sides.Guer)
                    return 2;
                else if (side_ == RV::get().sides.Civ)
                    return 3;

                return 0;
            }


            types::config getFactionConfig(const std::string& faction_) {
                intercept::types::config path = sqf::config_entry(sqf::mission_config_file()) >> "CfgFactionClasses" >> faction_;

                if (sqf::is_class(path))
                    return path;

                path = sqf::config_entry(sqf::config_file()) >> "CfgFactionClasses" >> faction_;

                if (sqf::is_class(path))
                    return path;

                return sqf::config_null();
            }

            types::side getFactionSide(const std::string& faction_) {
                types::config factionConfigPath = getFactionConfig(faction_);

                if (factionConfigPath != sqf::config_null())
                    return numberToSide(sqf::get_number(sqf::config_entry(factionConfigPath) >> "side"));
                else
                    return RV::get().sides.East;
            }

            std::string getSideColor(const intercept::types::side& side_) {
                if (side_ == RV::get().sides.East)
                    return "ColorOPFOR";
                else if (side_ == RV::get().sides.West)
                    return "ColorBLUFOR";
                else if (side_ == RV::get().sides.Guer)
                    return "ColorIndependent";
                else if (side_ == RV::get().sides.Civ)
                    return "ColorCiv";

                return "ColorOPFOR";
            }


        }
    }
}