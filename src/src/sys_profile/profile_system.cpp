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
            sqf_commands::registerScriptCommands();
        }

        void ProfileSystem::start() {
            // register profile system with core

            alive::Core::get().registerModule(&ProfileSystem::get());

            // start systems

            
        }


        // functional


        void ProfileSystem::onSimulationStep(float dt_) {
            _activateProfiles();
            _simulateProfileMovement();

            for (auto& profile : _profileHandler.getProfiles())
                profile->update(dt_);
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
                            moveDist
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