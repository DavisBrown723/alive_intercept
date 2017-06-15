#pragma once

#include "common\include.hpp"

#include <vector>


namespace alive {
    namespace sys_profile {


        enum class ProfileType { INFANTRY, MOTORIZED, MECHANIZED, ARMORED, ANTI_AIR, ARTILLERY, HELICOPTER, PLANE, SHIP, STATIC_WEAPON };

        inline ProfileType vehicleTypeToProfileType(common::vehicles::VehicleType vehicleType_) {
            switch (vehicleType_) {
                case common::vehicles::VehicleType::CAR: {
                    return ProfileType::MOTORIZED;
                }
                case common::vehicles::VehicleType::TRUCK: {
                    return ProfileType::MOTORIZED;
                }
                case common::vehicles::VehicleType::MECHANIZED: {
                    return ProfileType::MECHANIZED;
                }
                case common::vehicles::VehicleType::TANK: {
                    return ProfileType::ARMORED;
                }
                case common::vehicles::VehicleType::ARTILLERY: {
                    return ProfileType::ARTILLERY;
                }
                case common::vehicles::VehicleType::ANTI_AIR: {
                    return ProfileType::ANTI_AIR;
                }
                case common::vehicles::VehicleType::SHIP: {
                    return ProfileType::SHIP;
                }
                case common::vehicles::VehicleType::HELICOPTER: {
                    return ProfileType::HELICOPTER;
                }
                case common::vehicles::VehicleType::PLANE: {
                    return ProfileType::PLANE;
                }
                case common::vehicles::VehicleType::STATIC_WEAPON:
                {
                    return ProfileType::STATIC_WEAPON;
                }
            }
        }

        struct HitPoint {
            std::string name;
            float damage;
        };

        struct TurretMagazine {
            std::string name;
            std::vector<int> turretPath;
            int ammoCount = -1;
        };


    }
}