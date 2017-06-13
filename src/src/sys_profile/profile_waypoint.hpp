#pragma once

#include "intercept.hpp"


namespace alive {
    namespace sys_profile {


        class ProfileGroup;


        class ProfileWaypoint {

            
            public:


                using RVWaypoint = intercept::sqf::waypoint;
                using Type = RVWaypoint::type;
                using Speed = RVWaypoint::speed;
                using Formation = RVWaypoint::formation;
                using Behavior = RVWaypoint::behaviour;
                using CombatMode = RVWaypoint::combat_mode;

                ProfileWaypoint(
                    const intercept::types::vector3&    pos_,
                    Type                                type_ = Type::MOVE,
                    int                                 completionRadius_ = 50,
                    Speed                               speed_ = Speed::UNCHANGED,
                    Formation                           formation_ = Formation::WEDGE,
                    Behavior                            behavior_ = Behavior::UNCHANGED,
                    CombatMode                          combatMode_ = CombatMode::NO_CHANGE,
                    const std::string&                  statements_ = std::string()
                );

                ProfileWaypoint(RVWaypoint& wp_);

                virtual ~ProfileWaypoint();

                RVWaypoint toRVWaypoint(ProfileGroup* profile_, intercept::types::group& group_);

                intercept::types::vector3   position;
                int                         completionRadius = 50;
                Type                        type;
                Speed                       speed;
                Formation                   formation;
                Behavior                    behavior;
                CombatMode                  combatMode;
                std::string                 statements;


        };


    }
}