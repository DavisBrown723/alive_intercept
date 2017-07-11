#include "profile_unit.hpp"

#include "intercept.hpp"

#include "profile_group.hpp"
#include "helpers.hpp"

using namespace intercept;


namespace alive {
    namespace sys_profile {


        ProfileUnit::ProfileUnit()
            :
            _unitClass("B_Soldier_F"),
            _active(false),
            _health(1)
        {
            _speed = sqf::get_number(
                    sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> _unitClass >> "maxSpeed"
            ) * 0.20f;
        }

        ProfileUnit::ProfileUnit(ProfileGroup* profile_, const std::string& unitClass_, float health_ )
            :
            _unitClass(unitClass_),
            _profile(profile_),
            _active(false),
            _health(health_)
        {
            _speed = sqf::get_number(
                sqf::config_entry(sqf::config_file()) >> "CfgVehicles" >> _unitClass >> "maxSpeed"
            ) * 0.20f;
        }

        ProfileUnit::~ProfileUnit() {
            if (sqf::is_null(_unitObject))
                sqf::delete_vehicle(_unitObject);

            if (_vehicleAssignment != nullptr)
                leaveVehicle();
        }


        // functional


        void ProfileUnit::spawn(ProfileGroup* profile_) {
            if (_active)
                return;

            if (!sqf::is_null(_unitObject))
                sqf::delete_vehicle(_unitObject);

            _unitObject = sqf::create_unit(profile_->getGroupObject(), _unitClass, profile_->getPosition(), {}, 0.f, "FORM");
            
            // apply profile information to unit

            sqf::set_variable(_unitObject, "alive_profileID", profile_->getID());
            sqf::set_variable(_unitObject, "alive_profileUnitID", _id);
            sqf::set_damage(_unitObject, 1.f - _health, false);

            if (_vehicleAssignment != nullptr)
                sqf::move_in_any(_unitObject, _vehicleAssignment->vehicle->getVehicleObject());

            sqf::add_event_handler(_unitObject, "GetInMan", "(_this select 0) profileUnitGetIn (_this select 2)");
            sqf::add_event_handler(_unitObject, "GetOutMan", "(_this select 0) profileUnitGetOut (_this select 2)");

            _active = true;
        }

        void ProfileUnit::despawn() {
            if (!_active || sqf::is_null(_unitObject))
                return;

            // update information

            _health = 1.f - sqf::damage(_unitObject);

            _beingDeleted = true;
            sqf::delete_vehicle(_unitObject);
            _beingDeleted = false;

            _active = false;
        }

        void ProfileUnit::getInVehicle(GroupVehicleAssignment* vehicleAssignment_) {
            if (_vehicleAssignment != nullptr)
                return;

            vehicleAssignment_->vehicle->seatUnit(this);

            _vehicleAssignment = vehicleAssignment_;
        }

        void ProfileUnit::getInVehicle(ProfileVehicle* vehicle_) {
            if (_vehicleAssignment != nullptr)
                return;

            this->getInVehicle(_profile->getVehicleAssignment(vehicle_));
        }

        void ProfileUnit::leaveVehicle() {
            // if GetOut EH fired because the
            // unit was being deleted
            // exit
            if (_beingDeleted) {
                _beingDeleted = false;
                return;
            }

            _vehicleAssignment->vehicle->unseatUnit(this);

            _vehicleAssignment = nullptr;
        }

        void ProfileUnit::switchGroup(ProfileGroup* profile_) {
            // initialize with other group

            profile_->addUnit(this);

            // uninitialize with current group

            this->leaveVehicle();

            _profile->removeUnit(this);
        }

        void ProfileUnit::onLeftAssignedVehicle() {
            _vehicleAssignment = nullptr;
        }

    }
}