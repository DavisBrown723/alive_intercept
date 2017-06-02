#include "profile_system.hpp"

#include "common\include.hpp"
#include "core\core.hpp"
#include "profile_group.hpp"
#include "sqf_commands.hpp"

#include <algorithm>



namespace alive {
    namespace sys_profile {

        ProfileSystem::ProfileSystem()
            :
            ProfileSystem(1500)
        {}

        ProfileSystem::ProfileSystem(int spawnDistance_)
            :
            _spawnDistance(spawnDistance_)
        {

        }

        ProfileSystem::~ProfileSystem() {

        }
        
        void ProfileSystem::registerScriptCommands() {
            // Profile System

            sqf_commands::handles::_startProfileSystem = intercept::client::host::functions.register_sqf_function_unary(
                "startProfileSystem",
                "Starts the ALiVE Profile System with the given arguments.",
                userFunctionWrapper<sqf_commands::startProfileSystem>,
                intercept::types::__internal::GameDataType::NOTHING,
                intercept::types::__internal::GameDataType::ARRAY
            );

            sqf_commands::handles::_enableProfileSystemDebug = intercept::client::host::functions.register_sqf_function_unary(
                "enableProfileSystemDebug",
                "Sets debug on the ALiVE Profile System.",
                userFunctionWrapper<sqf_commands::enableProfileSystemDebug>,
                intercept::types::__internal::GameDataType::NOTHING,
                intercept::types::__internal::GameDataType::BOOL
            );

            // Profile Handler

            sqf_commands::handles::_getProfiles = intercept::client::host::functions.register_sqf_function_nular(
                "getProfiles",
                "Returns an array of all ALiVE Profiles.",
                userFunctionWrapper<sqf_commands::getProfiles>,
                intercept::types::__internal::GameDataType::ARRAY
            );

            sqf_commands::handles::_getProfilesBySide = intercept::client::host::functions.register_sqf_function_unary(
                "getProfilesBySide",
                "Returns an array of all ALiVE Profiles belonging to the given side.",
                userFunctionWrapper<sqf_commands::getProfilesBySide>,
                intercept::types::__internal::GameDataType::ARRAY,
                intercept::types::__internal::GameDataType::SIDE
            );

            // Profiles

            sqf_commands::handles::_createProfile = intercept::client::host::functions.register_sqf_function_unary(
                "createProfile",
                "Creates an ALiVE profile.",
                userFunctionWrapper<sqf_commands::createProfile>,
                intercept::types::__internal::GameDataType::STRING,
                intercept::types::__internal::GameDataType::ARRAY
            );

            sqf_commands::handles::_getProfilePosition = intercept::client::host::functions.register_sqf_function_unary(
                "getProfilePosition",
                "Gets the position of an ALiVE Profile.",
                userFunctionWrapper<sqf_commands::getProfilePosition>,
                intercept::types::__internal::GameDataType::ARRAY,
                intercept::types::__internal::GameDataType::STRING
            );

            sqf_commands::handles::_getProfileSpeed = intercept::client::host::functions.register_sqf_function_unary(
                "getProfileSpeed",
                "Gets the speed of an ALiVE Profile.",
                userFunctionWrapper<sqf_commands::getProfileSpeed>,
                intercept::types::__internal::GameDataType::ARRAY,
                intercept::types::__internal::GameDataType::STRING
            );
        }

        void ProfileSystem::onSimulationStep() {
            _activateProfiles();
            _simulateProfileMovement();

            for (auto& profile : _profileHandler.getProfiles())
                profile->updateOnSimulationStep();
        }

        void ProfileSystem::_activateProfiles() {
            // get player positions

            std::vector<intercept::types::vector3> playerPositions;

            for (auto& unit : intercept::sqf::all_players())
                playerPositions.push_back(intercept::sqf::get_pos(unit));

            // activate profiles within
            // spawn distance of a player

            bool inRange;
            intercept::types::vector3 profilePos;

            for (auto& profile : ProfileSystem::get().getProfileHandler().getProfiles()) {
                inRange = false;
                profilePos = profile->getPosition();

                for (auto& playerPos : playerPositions) {
                    if (!inRange) {
                        if (playerPos.distance(profilePos) < _spawnDistance)
                            inRange = true;
                    }
                }

                if (inRange)
                    profile->spawn();
                else
                    if (profile->isActive()) profile->despawn();
            }
        }

        void ProfileSystem::_simulateProfileMovement() {
            // calculate delta time

            float currTime = intercept::sqf::time();

            if (_lastSimTime == 0) _lastSimTime = currTime;

            float dt = currTime - _lastSimTime / 1;

            _lastSimTime - currTime;

            // simulate profiles

            std::vector<ProfileWaypoint> waypoints;
            int distToCompletion;
            int moveDist;
            bool waypointComplete;
            intercept::types::vector3 newPos;

            for (auto& profile : ProfileSystem::get().getProfileHandler().getProfiles()) {
                if (!profile->isActive()) {
                    if (profile->getWaypoints().size() > 0) {
                        waypoints = profile->getWaypoints();

                        distToCompletion = (waypoints[0].position.distance(profile->getPosition())) - waypoints[0].completionRadius;

                        waypointComplete = false;

                        // determine distance to move

                        if (profile->getSpeed() < distToCompletion) {
                            moveDist = profile->getSpeed();
                        } else {
                            moveDist = distToCompletion;
                            waypointComplete = true;
                        }

                        // move profile

                        newPos = common::math::getRelPos(
                            profile->getPosition(),
                            common::math::getRelDir(profile->getPosition(), waypoints[0].position),
                            moveDist * dt
                        );

                        profile->setPosition(newPos);

                        if (waypointComplete)
                            waypoints.erase(waypoints.begin());
                    }
                } else {
                    // profile is spawned
                }
            }
        }


    }
}