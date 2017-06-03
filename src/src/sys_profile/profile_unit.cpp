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
            _speed = intercept::sqf::get_number(
                intercept::sqf::config_entry(intercept::sqf::config_file()) >> "CfgVehicles" >> _unitClass >> "maxSpeed"
            );
        }

        ProfileUnit::ProfileUnit(const std::string& unitClass_, int health_ )
            :
            _unitClass(unitClass_),
            _active(false),
            _health(health_)
        {
            _speed = intercept::sqf::get_number(
               intercept::sqf::config_entry(intercept::sqf::config_file()) >> "CfgVehicles" >> _unitClass >> "maxSpeed"
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

            _unitObject = intercept::sqf::create_unit(profile_->_groupObject, _unitClass, profile_->_pos);
            
            // apply profile information to unit

            intercept::sqf::set_damage(_unitObject, 1 - _health, false);

            _active = true;
        }

        void ProfileUnit::despawn() {
            if (!_active || intercept::sqf::is_null(_unitObject))
                return;

            // update information

            _health = 1 - (intercept::sqf::damage(_unitObject));

            intercept::sqf::delete_vehicle(_unitObject);

            _active = false;
        }

    }
}