#include "profile_vehicle.hpp"

#include "intercept.hpp"

#include "common\include.hpp"
#include "sys_profile\helpers.hpp"

#include "profile_system.hpp"
#include "profile_handler.hpp"
#include "profile_group.hpp"

#include <string>
#include <vector>
#include <algorithm>

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

            _initialize();
        }

        ProfileVehicle::ProfileVehicle(
            const types::side& side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const types::config& _vehicleConfig
        )
            :
            Profile(side_, faction_, pos_),
            _vehicleClass(sqf::config_name(_vehicleConfig)),
            _dir(0),
            _fuel(1.f),
            _engineOn(false)
        {
            _vehicleType = common::vehicles::getVehicleType(_vehicleClass);

            _initialize();
        }

        ProfileVehicle::~ProfileVehicle() {
            enableDebug(false);

            if (_vehicleAssignment != nullptr)
                _vehicleAssignment->group->unGarrisonVehicle(_vehicleAssignment);

            if (_active && sqf::alive(_vehicleObject))
                sqf::delete_vehicle(_vehicleObject);
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

        
        // getters


        // setters


        void ProfileVehicle::setPosition(const intercept::types::vector3& newPos_, bool moveObjects_) {
            _pos = newPos_;

            if (moveObjects_ && _active)
                sqf::set_pos(_vehicleObject, newPos_);

            if (_debugEnabled)
                sqf::set_marker_pos(_debugMarker, newPos_);
        }

        
        // functional


        void ProfileVehicle::spawn() {
            if (_active)
                return;

            _vehicleObject = sqf::create_vehicle(_vehicleClass, _pos, {}, 0.f, "CAN_COLLIDE");

            // set vehicle info

            sqf::set_variable(_vehicleObject, "alive_profileID", _id);

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

            // set cargo items

            sqf::clear_item_cargo_global(_vehicleObject);
            sqf::clear_magazine_cargo_global(_vehicleObject);
            sqf::clear_weapon_cargo_global(_vehicleObject);

            for (auto& item : _cargoItems) sqf::add_item_cargo_global(_vehicleObject, item, 1);

            // spawn complete

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 0.7f);

            _active = true;
        }

        void ProfileVehicle::despawn() {
            if (!_active)
                return;

            // update vehicle info

            _dir = static_cast<int>(sqf::get_dir(_vehicleObject));
            _fuel = sqf::fuel(_vehicleObject);
            _engineOn = sqf::is_engine_on(_vehicleObject);

            // update hitpoints

            for (auto& hitpoint : _hitpoints) {
                hitpoint.damage = sqf::get_hit_point_damage(_vehicleObject, hitpoint.name);
            }

            // update magazines

            _magazines.clear();
            for (auto& turretMag : sqf::magazines_all_turrets(_vehicleObject))
                if (turretMag.count > 0) _magazines.push_back({ turretMag.name, turretMag.turret_path, turretMag.count });

            // update cargo items

            _cargoItems.clear();
            common::containers::vectorAppend(_cargoItems, sqf::item_cargo(_vehicleObject));
            common::containers::vectorAppend(_cargoItems, sqf::magazine_cargo(_vehicleObject));
            common::containers::vectorAppend(_cargoItems, sqf::weapon_cargo(_vehicleObject));

            sqf::delete_vehicle(_vehicleObject);

            // despawn complete

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 0.3f);

            _active = false;
        }

        void ProfileVehicle::update(const float dt_) {
            _updateMovement();
        }

        void ProfileVehicle::onKilled() {

        }

        bool ProfileVehicle::seatUnit(ProfileUnit* unit_) {
            if (_seatsLeft == 0)
                return false;

            _vehicleAssignment->units.push_back(unit_);

            _seatsLeft--;

            return true;
        }

        void ProfileVehicle::unseatUnit(ProfileUnit* unit_) {
            _vehicleAssignment->group->onUnitLeftAssignedVehicle(unit_, this);

            _seatsLeft++;
        }


        // protected


        void ProfileVehicle::_calculateSpeed() {
            _speed = sqf::get_number(
                sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> _vehicleClass >> "maxSpeed"
            ) * 0.20f;
        }

        void ProfileVehicle::_createDebugMarker() {
            _debugMarker = sqf::create_marker(_id + "_debug", _pos);

            sqf::set_marker_pos(_debugMarker, _pos);
            sqf::set_marker_size(_debugMarker, types::vector2(0.9f, 0.9f));

            if (_active)
                sqf::set_marker_alpha(_debugMarker, 0.7f);
            else
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
        
        void ProfileVehicle::_updateMovement() {
            if (_active) {
                if (!sqf::is_null(_vehicleObject))
                    this->setPosition(sqf::get_pos(_vehicleObject));
            } else {
                if (_vehicleAssignment != nullptr)
                    this->setPosition(_vehicleAssignment->group->getPosition());
            }
        }

        void ProfileVehicle::_initialize() {
            _initializeHitpoints();
            _initializeMagazines();
            _initializeSeats();
            _initializeCargoItems();
            _calculateSpeed();
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
            std::vector<common::vehicles::TurretMagazine> turretMags = common::vehicles::getVehicleTurretMagazines(_vehicleClass);

            for (auto& mag : turretMags)
                _magazines.push_back({ mag.name,mag.turretPath });
        }

        void ProfileVehicle::_initializeSeats() {
            _seatCount = common::vehicles::getVehiclePositionCount(_vehicleClass);

            _seatsLeft = _seatCount;
        }

        void ProfileVehicle::_initializeCargoItems() {
            sqf::config_entry configPath = sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> _vehicleClass;

            int itemTypeIndex = 0;
            sqf::config_entry itemTypePath;
            sqf::config_entry itemPath;
            int itemCount;

            int countToAdd;
            std::string itemClass;
            int i;
            int j;

            for (auto& itemConfigPathName : { "TransportItems" ,"TransportMagazines" ,"TransportWeapons" ,"TransportBackpacks" }) {
                itemTypePath = configPath >> itemConfigPathName;

                if (sqf::is_class(itemTypePath)) {
                    itemCount = static_cast<int>(sqf::count(itemTypePath));

                    for (i = 0; i < itemCount; i++) {
                        itemPath = sqf::select(itemTypePath, static_cast<float>(i));

                        countToAdd = static_cast<int>(sqf::get_number(itemPath >> "count"));

                        switch (itemTypeIndex) {
                            case 0: {
                                itemClass = sqf::get_text(itemPath >> "name");
                                break;
                            }
                            case 1: {
                                itemClass = sqf::get_text(itemPath >> "magazine");
                                break;
                            }
                            case 2: {
                                itemClass = sqf::get_text(itemPath >> "weapon");
                                break;
                            }
                            case 3: {
                                itemClass = sqf::get_text(itemPath >> "backpack");
                                break;
                            }
                        }

                        _cargoItems.reserve(countToAdd);
                        for (j = 0; j < countToAdd; j++) _cargoItems.push_back(itemClass);
                    }
                }

                itemTypeIndex++;
            }
        }


    }
}