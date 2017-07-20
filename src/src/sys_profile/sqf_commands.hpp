#pragma once

#include "intercept.hpp"
#include "sys_profile\include.hpp"

namespace alive {
    namespace sys_profile {
        namespace sqf_commands {
            namespace handles {
                //

                extern intercept::types::registered_sqf_function _profileWaypointComplete;

                //

                extern intercept::types::registered_sqf_function _startProfileSystem;
                extern intercept::types::registered_sqf_function _enableProfileSystemDebug;

                extern intercept::types::registered_sqf_function _allProfiles;
                extern intercept::types::registered_sqf_function _getProfilesBySide;

                extern intercept::types::registered_sqf_function _createProfileGroup;
                extern intercept::types::registered_sqf_function _createProfileVehicle;

                extern intercept::types::registered_sqf_function _unregisterProfile;

                extern intercept::types::registered_sqf_function _profileGetPosition;

                extern intercept::types::registered_sqf_function _profileAddWaypoint;
                extern intercept::types::registered_sqf_function _profileRemoveWaypoint;

                extern intercept::types::registered_sqf_function _profileUnitGetIn;
                extern intercept::types::registered_sqf_function _profileUnitGetOut;
            }

            void registerScriptCommands();

            //

            game_value profileWaypointComplete(game_value rightArg_);

            //

            // Profile System

            game_value startProfileSystem(game_value rightArg_);
            game_value enableProfileSystemDebug(game_value rightArg_);

            // Profile Handler

            game_value allProfiles();
            game_value getProfilesBySide(game_value rightArg_);

            // Profiles

            game_value createProfileGroup(game_value profileArgs_);
            game_value createProfileVehicle(game_value profileArgs_);

            game_value unregisterProfile(game_value profileID_);

            game_value profileGetPosition(game_value profileID_);

            game_value profileAddWaypoint(game_value profileID_, game_value waypointArgs_);
            game_value profileRemoveWaypoint(game_value profileID_, game_value waypointIndex_);

            game_value profileUnitGetIn(game_value unitObject_, game_value vehicleObject_);
            game_value profileUnitGetOut(game_value unitObject_, game_value vehicleObject_);
        }
    }
}