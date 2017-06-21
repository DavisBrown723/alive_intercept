#pragma once

#include "intercept.hpp"

#include "vehicles_types.hpp"

#include <string>
#include <vector>


namespace alive {
    namespace common {
        namespace vehicles {


            enum class VehicleType { CAR, TRUCK, MECHANIZED, TANK, ARTILLERY, ANTI_AIR, SHIP, HELICOPTER, PLANE, STATIC_WEAPON };

            VehicleType getVehicleType(const std::string& vehicleClass_);

            bool isArtillery(const std::string& vehicleClass_);
            bool isAntiAir(const std::string& vehicleClass_);

            std::vector<TurretMagazine> getVehicleTurretMagazines(const std::string& vehicleClass_);

            std::vector< std::vector<int> > getVehicleTurretPaths(const std::string& vehicleClass_);

            VehiclePositions getVehiclePositions(const std::string& vehicleClass_);


            namespace __internal {

                void getVehicleTurretMagsRecurse(std::vector<TurretMagazine>& result_, intercept::sqf::config_entry& turretConfig_, std::vector<int> turretPath_ = {});

                void _getVehiclePositionsCount(std::vector<int>& result_, intercept::sqf::config_entry& turretConfig_, std::vector<int> turretPath_ = {});

            }


        }
    }
}