#pragma once

#include "common\include.hpp"

#include <vector>


namespace alive {
    namespace sys_profile {


        enum class ProfileType { INFANTRY , VEHICLE };
        enum class ProfileVehicleType { INFANTRY, MOTORIZED, MECHANIZED, ARMORED, ARTILLERY, ANTI_AIR, NAVAL, HELICOPTER, PLANE, STATIC_WEAPON };

        inline ProfileVehicleType vehicleTypeToProfileType(common::vehicles::VehicleType vehicleType_) {
            switch (vehicleType_) {
                case common::vehicles::VehicleType::CAR: {
                    return ProfileVehicleType::MOTORIZED;
                }
                case common::vehicles::VehicleType::TRUCK: {
                    return ProfileVehicleType::MOTORIZED;
                }
                case common::vehicles::VehicleType::MECHANIZED: {
                    return ProfileVehicleType::MECHANIZED;
                }
                case common::vehicles::VehicleType::TANK: {
                    return ProfileVehicleType::ARMORED;
                }
                case common::vehicles::VehicleType::ARTILLERY: {
                    return ProfileVehicleType::ARTILLERY;
                }
                case common::vehicles::VehicleType::ANTI_AIR: {
                    return ProfileVehicleType::ANTI_AIR;
                }
                case common::vehicles::VehicleType::SHIP: {
                    return ProfileVehicleType::NAVAL;
                }
                case common::vehicles::VehicleType::HELICOPTER: {
                    return ProfileVehicleType::HELICOPTER;
                }
                case common::vehicles::VehicleType::PLANE: {
                    return ProfileVehicleType::PLANE;
                }
                case common::vehicles::VehicleType::STATIC_WEAPON:
                {
                    return ProfileVehicleType::STATIC_WEAPON;
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