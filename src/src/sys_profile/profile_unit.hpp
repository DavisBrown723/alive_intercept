#pragma once

#include "intercept.hpp"

#include "common\include.hpp"

#include <string>


namespace alive {
    namespace sys_profile {

        
        class ProfileGroup;


        class ProfileUnit {


            friend class ProfileGroup;


            public:


                ProfileUnit();
                ProfileUnit(ProfileGroup* profile_, const std::string& unitClass_, int health_ = 1);
                virtual ~ProfileUnit();


                // setters



                // getters


                const std::string& getID() const    { return _id; }
                ProfileGroup* getGroup() const      { return _profile; }
                std::string getUnitClass() const    { return _unitClass; }
                bool isActive() const               { return _active; }
                int getHealth() const               { return _health; }
                float getSpeed() const              { return _speed; }

                intercept::types::vector3 getPosition() const {
                    if (!_unitObject.is_null())
                        intercept::sqf::get_pos(_unitObject);
                }

                bool isInVehicle() const                    { return _occupiedVehicle != nullptr; }
                ProfileVehicle* getOccupiedVehicle() const  { return _occupiedVehicle; }


                // setters


                void setPosition(const intercept::types::vector3& pos_) {
                    if (!_unitObject.is_null() && _occupiedVehicle == nullptr)
                        intercept::sqf::set_pos(_unitObject, pos_);
                }


                // functional


                virtual void spawn(ProfileGroup* profile_);
                virtual void despawn();

                void getInVehicle(ProfileVehicle* vehicle_);
                void leaveVehicle();

                void switchGroup(ProfileGroup* profile_);


            protected:


                const std::string _unitClass;
                std::string _id;
                ProfileGroup* _profile = nullptr;

                bool _active;
                float _health;
                float _speed;

                ProfileVehicle* _occupiedVehicle = nullptr;

                intercept::types::object _unitObject;


        };


    }
}