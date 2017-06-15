#include "vehicles_common.hpp"

#include "intercept.hpp"

using namespace intercept;


namespace alive {
    namespace common {
        namespace vehicles {


            VehicleType getVehicleType(const std::string& vehicleClass_) {
                if (sqf::is_kind_of(vehicleClass_, "Car"))
                    return VehicleType::CAR;
                else if (sqf::is_kind_of(vehicleClass_, "Truck"))
                    return VehicleType::TRUCK;
                else if (sqf::is_kind_of(vehicleClass_, "Armored"))
                    return VehicleType::MECHANIZED;
                else if (sqf::is_kind_of(vehicleClass_, "Tank")) {
                    if (isArtillery(vehicleClass_))
                        return VehicleType::ARTILLERY;

                    if (isAntiAir(vehicleClass_))
                        return VehicleType::ANTI_AIR;

                    return VehicleType::TANK;
                } else if (sqf::is_kind_of(vehicleClass_, "Ship"))
                    return VehicleType::SHIP;
                else if ((sqf::is_kind_of(vehicleClass_, "Helicopter")))
                    return VehicleType::HELICOPTER;
                else if (sqf::is_kind_of(vehicleClass_, "Plane"))
                    return VehicleType::PLANE;
                else if ((sqf::is_kind_of(vehicleClass_, "StaticWeapon")))
                    return VehicleType::STATIC_WEAPON;

                return VehicleType::CAR;
            }

            bool isArtillery(const std::string& vehicleClass_) {
                return sqf::is_kind_of(vehicleClass_, "LandVehicle") &&
                    (sqf::get_number(sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> vehicleClass_ >> "Turrets" >> "MainTurret" >> "maxElev") > 65) &&
                    (sqf::get_number(sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> vehicleClass_ >> "artilleryScanner") > 0);
            }

            bool isAntiAir(const std::string& vehicleClass_) {
                return sqf::is_kind_of(vehicleClass_, "LandVehicle") &&
                    (sqf::get_number(sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> vehicleClass_ >> "Turrets" >> "MainTurret" >> "maxElev") > 65) &&
                    (sqf::get_number(sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> vehicleClass_ >> "artilleryScanner") == 0);
            }


        }
    }
}