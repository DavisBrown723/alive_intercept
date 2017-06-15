#include "profile_vehicle.hpp"

#include "profile_system.hpp"
#include "profile_handler.hpp"
#include "common\include.hpp"

#include <string>
#include <vector>

using namespace intercept;


namespace alive {
    namespace sys_profile {


        ProfileVehicle::ProfileVehicle()
            :
            _vehicleClass("B_MRAP_01_F"),
            _dir(0),
            _fuel(1.f),
            _engineOn(false)
        {

        }

        ProfileVehicle::ProfileVehicle(
            const types::side& side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const std::string& vehicleClass_
        )
            :
            Profile(side_, faction_, pos_),
            _vehicleClass(vehicleClass_),
            _dir(0),
            _fuel(1.f),
            _engineOn(false)
        {
            _vehicleType = common::vehicles::getVehicleType(_vehicleClass);

            _calculateSpeed();
            _initializeHitpoints();
            _initializeMagazines();
        }

        ProfileVehicle::ProfileVehicle(
            const types::side& side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const types::config& _vehicleConfig
        )
            :
            Profile(side_, faction_, pos_),
            _vehicleClass(sqf::get_text(sqf::config_entry(_vehicleConfig)))
        {
            _vehicleType = common::vehicles::getVehicleType(_vehicleClass);

            _calculateSpeed();
            _initializeHitpoints();
            _initializeMagazines();
        }

        ProfileVehicle::~ProfileVehicle() {
            enableDebug(false);
        }

        ProfileVehicle* ProfileVehicle::Create(
            const types::side& side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const std::string& vehicleClass_
        ) {
            ProfileVehicle* profile = new ProfileVehicle(side_, faction_, pos_, vehicleClass_);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }

        ProfileVehicle* ProfileVehicle::Create(
            const types::side& side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const types::config& _vehicleConfig
        ) {
            ProfileVehicle* profile = new ProfileVehicle(side_, faction_, pos_, _vehicleConfig);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }

        void ProfileVehicle::spawn() {
            if (_active)
                return;

            _vehicleObject = sqf::create_vehicle(_vehicleClass, _pos, {}, 0.f, "CAN_COLLIDE");

            // set vehicle info

            sqf::set_dir(_vehicleObject, static_cast<float>(_dir));
            sqf::set_fuel(_vehicleObject, _fuel);
            sqf::engine_on(_vehicleObject, _engineOn);

            for (auto& hitpoint : _hitpoints) {
                sqf::set_hit_point_damage(_vehicleObject, hitpoint.name, hitpoint.damage);
            }

            // remove magazines before setting from stored data

            for (auto& mag : sqf::magazines_all_turrets(_vehicleObject)) sqf::remove_magazine_turret(_vehicleObject, mag.name, mag.turret_path);

            // if ammoCount is -1 (default value)
            // addMagazineTurret will add full ammo (submit intercept PR)

            for (auto& turretMag : _magazines)
                sqf::add_magazine_turret(_vehicleObject, turretMag.name, turretMag.turretPath, turretMag.ammoCount);

            // spawn complete

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 1.f);

            _active = true;
        }

        void ProfileVehicle::despawn() {
            if (!_active)
                return;

            // update vehicle info

            _dir = static_cast<int>(sqf::get_dir(_vehicleObject));
            _fuel = sqf::fuel(_vehicleObject);
            _engineOn = sqf::is_engine_on(_vehicleObject);

            for (auto& hitpoint : _hitpoints) {
                hitpoint.damage = sqf::get_hit_point_damage(_vehicleObject, hitpoint.name);
            }

            _magazines.clear();
            for (auto& turretMag : sqf::magazines_all_turrets(_vehicleObject))
                if (turretMag.count > 0) _magazines.push_back({ turretMag.name, turretMag.turret_path, turretMag.count });

            sqf::delete_vehicle(_vehicleObject);

            // despawn complete

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 0.3f);

            _active = false;
        }

        void ProfileVehicle::update(const float dt_) {

        }


        // protected


        void ProfileVehicle::_calculateSpeed() {
            _speed = static_cast<int>(
                sqf::get_number(
                    sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> _vehicleClass >> "maxSpeed"
                ) * 0.20
            );
        }

        void ProfileVehicle::_createDebugMarker() {
            _debugMarker = sqf::create_marker(_id + "_debug", _pos);

            sqf::set_marker_pos(_debugMarker, _pos);
            sqf::set_marker_size(_debugMarker, types::vector2(0.6f, 0.6f));

            if (!_active)
                sqf::set_marker_alpha(_debugMarker, 0.3f);

            std::string markerType;
            switch (_vehicleType) {
                case common::vehicles::VehicleType::CAR:
                case common::vehicles::VehicleType::TRUCK: {
                    markerType = "motor_inf";
                    break;
                }
                case common::vehicles::VehicleType::MECHANIZED: {
                    markerType = "mech_inf";
                    break;
                }
                case common::vehicles::VehicleType::ANTI_AIR:
                case common::vehicles::VehicleType::TANK: {
                    markerType = "armor";
                    break;
                }
                case common::vehicles::VehicleType::ARTILLERY: {
                    markerType = "art";
                    break;
                }
                case common::vehicles::VehicleType::HELICOPTER: {
                    markerType = "air";
                    break;
                }
                case common::vehicles::VehicleType::PLANE: {
                    markerType = "plane";
                    break;
                }
                case common::vehicles::VehicleType::STATIC_WEAPON: {
                    markerType = "mortar";
                    break;
                }
                case common::vehicles::VehicleType::SHIP: {
                    markerType = "naval";
                    break;
                }
                default: {
                    markerType = "unknown";
                }
            }

            // use faction side to get correct
            // marker for empty vehicles

            if ((common::allegiance::getFactionSide(_faction)) == common::RV::get().sides.East) {
                sqf::set_marker_color(_debugMarker, "ColorRed");
                sqf::set_marker_type(_debugMarker, "o_" + markerType);
            } else if (_side == common::RV::get().sides.West) {
                sqf::set_marker_color(_debugMarker, "ColorBlue");
                sqf::set_marker_type(_debugMarker, "b_" + markerType);
            } else if (_side == common::RV::get().sides.Guer) {
                sqf::set_marker_color(_debugMarker, "ColorGreen");
                sqf::set_marker_type(_debugMarker, "n_" + markerType);
            }

            // vehicle is unnocupied

            if (false)
                sqf::set_marker_color(_debugMarker, "ColorYellow");
        }

        void ProfileVehicle::_initializeHitpoints() {
            sqf::config_entry configPath = sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> _vehicleClass >> "HitPoints";

            int hitpointCount = static_cast<int>(sqf::count(configPath));

            for (float i = 0.f; i < hitpointCount; i++) {
                _hitpoints.push_back(
                    {
                        sqf::config_name(sqf::select(configPath, i)),
                        0.f
                    }
                );
            }
        }

        void ProfileVehicle::_initializeMagazines() {
            sqf::config_entry vehicleConfig = sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> _vehicleClass;

            // add driver magazines

            types::auto_array<game_value> gameValueMagazines = sqf::get_array(vehicleConfig >> "magazines").to_array();
            std::vector<std::string> magazines{ gameValueMagazines.begin(),gameValueMagazines.end() };

            for (auto& mag : magazines) _magazines.push_back({ mag , {-1} });

            // add turret magazines

            _addTurretMagsRecurse(vehicleConfig >> "Turrets");
        }

        void ProfileVehicle::_addTurretMagsRecurse(sqf::config_entry turretConfig_, std::vector<int> turretPath_) {
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

                    for (auto& mag : magazines) _magazines.push_back({ mag , path });

                    // add magazines for sub turrets

                    sqf::config_entry subTurrets = turretConfig_ >> "Turrets";

                    if (sqf::is_class(subTurrets))
                        _addTurretMagsRecurse(subTurrets, path);
                }
            }
        }


    }
}