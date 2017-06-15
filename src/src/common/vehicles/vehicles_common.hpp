#pragma once

#include "intercept.hpp"

#include <string>


namespace alive {
    namespace common {
        namespace vehicles {


            enum class VehicleType { CAR, TRUCK, MECHANIZED, TANK, ARTILLERY, ANTI_AIR, SHIP, HELICOPTER, PLANE, STATIC_WEAPON };

            VehicleType getVehicleType(const std::string& vehicleClass_);

            bool isArtillery(const std::string& vehicleClass_);
            bool isAntiAir(const std::string& vehicleClass_);


        }
    }
}