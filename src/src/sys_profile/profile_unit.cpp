#include "profile_unit.hpp"

#include "profile_group.hpp"

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

            if (_occupiedVehicle != nullptr)
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

            if (_occupiedVehicle != nullptr)
                sqf::move_in_any(_unitObject, _occupiedVehicle->getVehicleObject());

            _active = true;
        }

        void ProfileUnit::despawn() {
            if (!_active || sqf::is_null(_unitObject))
                return;

            // update information

            _health = 1.f - sqf::damage(_unitObject);

            sqf::delete_vehicle(_unitObject);

            _active = false;
        }

        void ProfileUnit::getInVehicle(ProfileVehicle* vehicle_) {
            if (_occupiedVehicle != nullptr)
                return;

            vehicle_->seatUnit(this);

            _occupiedVehicle = vehicle_;
        }

        void ProfileUnit::leaveVehicle() {
            _occupiedVehicle->unseatUnit(this);

            _occupiedVehicle = nullptr;
        }

        void ProfileUnit::switchGroup(ProfileGroup* profile_) {
            // uninitialize with current group

            this->leaveVehicle();

            _profile->removeUnit(this);

            // initialize with other group

            profile_->addUnit(this);
        }

    }
}