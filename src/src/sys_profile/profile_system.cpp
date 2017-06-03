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


        void ProfileSystem::onSimulationStep(const float dt_) {
            _activateProfiles();

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


    }
}