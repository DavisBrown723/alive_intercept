#pragma once

#include "intercept.hpp"

#include <string>


namespace alive {
    namespace sys_profile {

        
        class ProfileGroup;


        class ProfileUnit {


            friend class ProfileGroup;


            public:


                ProfileUnit();
                ProfileUnit(const std::string& unitClass_, int health_ = 1);
                virtual ~ProfileUnit();


                // getters / setters


                std::string getUnitClass() { return _unitClass; }
                bool isActive() { return _active; }
                int getHealth() { return _health; }
                int getSpeed() { return _speed; }
                intercept::types::vector3 getPosition() {
                    if (!_unitObject.is_null())
                        intercept::sqf::get_pos(_unitObject);
                }

                void setPosition(const intercept::types::vector3& pos_) {
                    if (!_unitObject.is_null())
                        intercept::sqf::set_pos(_unitObject, pos_);
                }


                // functional


                virtual void spawn(ProfileGroup* profile_);
                virtual void despawn();


            protected:


                const std::string _unitClass;
                std::string _id;

                bool _active = false;
                int _health = 1;
                int _speed;

                intercept::types::object _unitObject;


        };


    }
}