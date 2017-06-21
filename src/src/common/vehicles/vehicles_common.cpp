#include "vehicles_common.hpp"
#include "vehicles_types.hpp"

#include "intercept.hpp"

#include <vector>

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

            std::vector<TurretMagazine> getVehicleTurretMagazines(const std::string& vehicleClass_) {
                sqf::config_entry vehicleConfig = sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> vehicleClass_;

                std::vector<TurretMagazine> result;

                // add driver magazines

                types::auto_array<game_value> gameValueMagazines = sqf::get_array(vehicleConfig >> "magazines").to_array();
                std::vector<std::string> magazines{ gameValueMagazines.begin(),gameValueMagazines.end() };

                for (auto& mag : magazines) result.push_back({ mag ,{ -1 } });

                // add turret magazines

                __internal::getVehicleTurretMagsRecurse(result, vehicleConfig >> "Turrets");

                return result;
            }

            std::vector< std::vector<int> > getVehicleTurretPaths(const std::string& vehicleClass_) {
                std::vector< std::vector<int> > paths;

                std::vector<TurretMagazine> turretMags = getVehicleTurretMagazines(vehicleClass_);

                for (auto& turretMag : turretMags)
                    paths.push_back(turretMag.turretPath);

                return paths;
            }

            VehiclePositions getVehiclePositions(const std::string& vehicleClass_) {
                sqf::config_entry vehicleConfig = sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> vehicleClass_;

                VehiclePositions positions;

                std::vector<int> positionsCount{ 0,0,0,0,0 };

                __internal::_getVehiclePositionsCount(positionsCount, vehicleConfig >> "Turrets");

                if (static_cast<int>(sqf::get_number(vehicleConfig >> "hasDriver")) == 1)
                    positions.Driver = { true };

                if (positionsCount[1] == 1)
                    positions.Gunner = { true };

                if (positionsCount[2] == 1)
                    positions.Commander = { true };

                std::vector< std::vector<int> > turretPaths = getVehicleTurretPaths(vehicleClass_);

                for (auto& turretPath : turretPaths)
                    if (turretPath[0] != -1) positions.Turrets.push_back({ true, turretPath });

                int cargoCount = static_cast<int>(sqf::get_number(vehicleConfig >> "transportSoldier"));
                for (int i = 0; i < cargoCount; i++)
                    positions.Cargo.push_back({ true });

                return positions;
            }


            namespace __internal {

                void getVehicleTurretMagsRecurse(std::vector<TurretMagazine>& result_, sqf::config_entry& turretConfig_, std::vector<int> turretPath_) {
                    int count = static_cast<int>(sqf::count(turretConfig_));

                    sqf::config_entry turretEntry;

                    for (int i = 0; i < count; i++) {
                        turretEntry = sqf::select(turretConfig_, static_cast<float>(i));

                        if (sqf::is_class(turretEntry)) {
                            // construct new path
                            std::vector<int> path{ turretPath_.begin(),turretPath_.end() };
                            path.push_back(i);

                            // add magazines

                            types::auto_array<game_value> gameValueMagazines = sqf::get_array(turretEntry >> "magazines").to_array();
                            std::vector<std::string> magazines{ gameValueMagazines.begin(),gameValueMagazines.end() };

                            for (auto& mag : magazines) result_.push_back({ mag , path });

                            // add magazines for sub turrets

                            sqf::config_entry subTurrets = turretConfig_ >> "Turrets";

                            if (sqf::is_class(subTurrets))
                                getVehicleTurretMagsRecurse(result_, subTurrets, path);
                        }
                    }
                }

                void _getVehiclePositionsCount(std::vector<int>& result_, intercept::sqf::config_entry& turretConfig_, std::vector<int> turretPath_) {
                    int turretCount = static_cast<int>(sqf::count(turretConfig_));

                    sqf::config_entry turret;

                    bool primaryGunner = false;
                    bool primaryObserver = false;
                    bool copilot = false;
                    bool isPersonTurret = false;

                    for (int i = 0; i < turretCount; i++) {
                        turret = sqf::select(turretConfig_, static_cast<float>(i));

                        if (sqf::is_class(turret)) {
                            // construct new path
                            std::vector<int> path{ turretPath_.begin(),turretPath_.end() };
                            path.push_back(i);

                            primaryGunner = false;
                            primaryObserver = false;
                            copilot = false;
                            isPersonTurret = false;

                            if (sqf::get_number(turret >> "primaryGunner") == 1) {
                                primaryGunner = true;
                                result_[1] = 1;
                            }

                            if (sqf::get_number(turret >> "primaryObserver") == 1) {
                                primaryObserver = true;
                                result_[2] = 1;
                            }

                            if (sqf::get_number(turret >> "isCopilot") == 1) {
                                copilot = true;
                                result_[3]++;
                            }

                            if (sqf::get_number(turret >> "isPersonTurret") == 1)
                                isPersonTurret = true;

                            if (!primaryGunner && !primaryObserver && !copilot) {
                                if (!isPersonTurret)
                                    result_[3]++;
                                else
                                    result_[4]++;
                            }

                            //  add sub positions

                            sqf::config_entry subTurrets = turretConfig_ >> "Turrets";

                            if (sqf::is_class(subTurrets))
                                _getVehiclePositionsCount(result_, subTurrets, path);
                        }
                    }
                }

            }


        }
    }
}