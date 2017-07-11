#pragma once


#include "intercept.hpp"
#include "common\include.hpp"
#include "sys_profile\helpers.hpp"

#include "profile.hpp"
#include "profile_unit.hpp"

#include <string>


namespace alive {
    namespace sys_profile {


        class ProfileVehicle : public Profile {

            friend class ProfileHandler;
            friend class ProfileGroup;
            friend class GroupVehicleAssignment;

            public:


                ProfileVehicle();
                ProfileVehicle(
                    const intercept::types::side& side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const std::string& vehicleClass_
                );

                ProfileVehicle(
                    const intercept::types::side& side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const intercept::types::config& _vehicleConfig
                );

                virtual ~ProfileVehicle();

                static ProfileVehicle* Create(
                    const intercept::types::side& side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const std::string& vehicleClass_
                );

                static ProfileVehicle* Create(
                    const intercept::types::side& side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const intercept::types::config& _vehicleConfig
                );


                // getters


                intercept::types::object getVehicleObject() const                           { return _vehicleObject; }
                common::vehicles::VehicleType getVehicleType() const                        { return _vehicleType; }
                int getDir() const                                                          { return _dir; }
                float getFuel() const                                                       { return _fuel; }
                GroupVehicleAssignment* getVehicleAssignment() const                        { return _vehicleAssignment; }

                const std::vector<HitPoint>& getHitpoints() const                           { return _hitpoints; }
                const std::vector<common::vehicles::TurretMagazine>& getMagazines() const   { return _magazines; }
                int getSeatCount() const                                                    { return _seatCount; }
                int getCountSeatsLeft() const                                               { return _seatsLeft; }
                ProfileType getProfileType() const override                                 { return ProfileType::VEHICLE; }


                // setters


                virtual void setPosition(const intercept::types::vector3& newPos_, bool moveObjects_ = false) override;

                void setDir(int newDir_)        { _dir = newDir_; }
                void setFuel(float newFuel_)    { _fuel = newFuel_; }


                // functional


                virtual void spawn();
                virtual void despawn();
                virtual void update(const float dt_);

                virtual void onKilled() override;

                virtual bool isGarrisoned() const       { return _seatCount > _seatsLeft; }
                virtual bool isFullyGarrisoned() const  { return _seatsLeft == 0; }
                bool seatUnit(ProfileUnit* unit_);
                void unseatUnit(ProfileUnit* unit_);


            protected:


                const std::string _vehicleClass;

                int _dir;
                float _fuel;
                bool _engineOn;

                common::vehicles::VehicleType _vehicleType;

                std::vector<HitPoint> _hitpoints;
                std::vector<common::vehicles::TurretMagazine> _magazines;
                std::vector<std::string> _cargoItems;

                int _seatCount = 0;
                int _seatsLeft = 0;

                GroupVehicleAssignment* _vehicleAssignment = nullptr;

                intercept::types::object _vehicleObject;

                // functions

                virtual void _calculateSpeed() override;
                virtual void _createDebugMarker() override;

                virtual void _updateMovement();

                void _initialize();
                void _initializeHitpoints();
                void _initializeMagazines();
                void _initializeSeats();
                void _initializeCargoItems();

        };
        

    }
}