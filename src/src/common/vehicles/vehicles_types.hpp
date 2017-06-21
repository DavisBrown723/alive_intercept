#pragma once

#include <string>
#include <vector>


namespace alive {
    namespace sys_profile {
        class ProfileUnit;
        class ProfileVehicle;
    }

    namespace common {
        namespace vehicles {

            struct TurretMagazine {
                std::string name;
                std::vector<int> turretPath;
                int ammoCount = -1;
            };

            struct VehicleSeat {
                VehicleSeat() : isSeat(false), unit(nullptr) {}
                VehicleSeat(bool isSeat_) : isSeat(isSeat_), unit(nullptr) {}

                bool isSeat = false;
                sys_profile::ProfileUnit* unit;
            };

            struct VehicleSeatTurret : public VehicleSeat {
                VehicleSeatTurret() : VehicleSeat() {}
                VehicleSeatTurret(bool isSeat_, std::vector<int> path_) : VehicleSeat(isSeat), path(path_) {}

                std::vector<int> path;
            };

            struct VehiclePositions {
                VehicleSeat Driver;
                VehicleSeat Commander;
                VehicleSeat Gunner;
                std::vector<VehicleSeatTurret> Turrets;
                std::vector<VehicleSeat> Cargo;
            };
            

        }
    }
}