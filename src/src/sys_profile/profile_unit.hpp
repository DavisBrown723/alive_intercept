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

                std::string getUnitClass() { return _unitClass; }
                bool isActive() { return _active; }
                int getHealth() { return _health; }
                int getSpeed() { return _speed; }

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