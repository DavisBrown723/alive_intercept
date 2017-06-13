#include "profile_unit.hpp"

#include "profile_group.hpp"


namespace alive {
    namespace sys_profile {


        ProfileUnit::ProfileUnit()
            :
            _unitClass("B_Soldier_F"),
            _active(false),
            _health(1)
        {
            _speed = static_cast<int>(
                intercept::sqf::get_number(
                    intercept::sqf::config_entry(intercept::sqf::config_file()) >> "CfgVehicles" >> _unitClass >> "maxSpeed"
                )
            );
        }

        ProfileUnit::ProfileUnit(const std::string& unitClass_, int health_ )
            :
            _unitClass(unitClass_),
            _active(false),
            _health(health_)
        {
            _speed = static_cast<int>(
                intercept::sqf::get_number(
                    intercept::sqf::config_entry(intercept::sqf::config_file()) >> "CfgVehicles" >> _unitClass >> "maxSpeed"
                )
            );
        }

        ProfileUnit::~ProfileUnit() {
            if (intercept::sqf::is_null(_unitObject))
                intercept::sqf::delete_vehicle(_unitObject);
        }


        // functional


        void ProfileUnit::spawn(ProfileGroup* profile_) {
            if (_active)
                return;

            if (!intercept::sqf::is_null(_unitObject))
                intercept::sqf::delete_vehicle(_unitObject);

            _unitObject = intercept::sqf::create_unit(profile_->_groupObject, _unitClass, profile_->getPosition());
            
            // apply profile information to unit

            intercept::sqf::set_variable(_unitObject, "alive_profileID", profile_->getID());
            intercept::sqf::set_variable(_unitObject, "alive_profileUnitID", game_value(_id));
            intercept::sqf::set_damage(_unitObject, static_cast<float>(1 - _health), false);

            _active = true;
        }

        void ProfileUnit::despawn() {
            if (!_active || intercept::sqf::is_null(_unitObject))
                return;

            // update information

            _health = 1 - static_cast<int>(intercept::sqf::damage(_unitObject));

            intercept::sqf::delete_vehicle(_unitObject);

            _active = false;
        }

    }
}