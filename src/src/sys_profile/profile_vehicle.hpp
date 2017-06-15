#pragma once

#include "profile.hpp"

#include "intercept.hpp"
#include "common\include.hpp"
#include "sys_profile\helpers.hpp"

#include <string>


namespace alive {
    namespace sys_profile {


        class ProfileVehicle : public Profile {

            friend class ProfileHandler;

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

                common::vehicles::VehicleType getVehicleType() const    { return _vehicleType; }
                int getDir() const                                      { return _dir; }
                float getFuel() const                                   { return _fuel; }

                const std::vector<HitPoint>& getHitpoints() const       { return _hitpoints; }
                const std::vector<TurretMagazine>& getMagazines() const { return _magazines; }
                ProfileType getProfileType() const override             { return ProfileType::VEHICLE; }

                // functional

                virtual void spawn();
                virtual void despawn();
                virtual void update(const float dt_);


            protected:


                const std::string _vehicleClass;

                int _dir;
                float _fuel;
                bool _engineOn;

                std::vector<HitPoint> _hitpoints;
                std::vector<TurretMagazine> _magazines;

                common::vehicles::VehicleType _vehicleType;

                intercept::types::object _vehicleObject;

                // functions

                virtual void _calculateSpeed() override;
                virtual void _createDebugMarker() override;

                void _initializeHitpoints();
                void _initializeMagazines();
                void _addTurretMagsRecurse(intercept::sqf::config_entry turretConfig_, std::vector<int> turretPath_ = {});

        };
        

    }
}