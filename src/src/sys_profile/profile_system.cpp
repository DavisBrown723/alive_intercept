#include "profile_system.hpp"

#include "common\include.hpp"
#include "core\core.hpp"
#include "profile.hpp"
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
            _profileHandler._profilesEast.clear();
            _profileHandler._profilesWest.clear();
            _profileHandler._profilesGuer.clear();

            _profileHandler._profiles.clear();
            _profileHandler._profileMap.clear();
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

            // simulate profiles

            int simulatedThisFrame = 0;
            float dt = std::max(std::ceil(static_cast<float>(_profileHandler.getProfiles().size() / 50)),1.f) * dt_;
            
            for(
                auto i = _profileHandler.getProfiles().begin() + _nextProfileToSimIndex;
                simulatedThisFrame < 50 && i != _profileHandler.getProfiles().end();
                i++, simulatedThisFrame++
            )
                (*i)->update(dt);

            _nextProfileToSimIndex += simulatedThisFrame;

            if (_nextProfileToSimIndex == _profileHandler.getProfiles().size())
                _nextProfileToSimIndex = 0;
        }

        void ProfileSystem::onMissionStopped() {
            _profileHandler.reset();
        
            _nextProfileToSimIndex = 0;
        }

        void ProfileSystem::onUnitKilled(intercept::types::object& killed_, intercept::types::object& killer_) {
            std::string profileID = intercept::sqf::get_variable(killed_, "alive_profileID", "");

            if (profileID != "") {

                if (intercept::sqf::is_kind_of(intercept::sqf::type_of(killed_), "Man")) {
                    ProfileGroup* profile = static_cast<ProfileGroup*>(_profileHandler.getProfile(profileID));

                    std::string profileUnitID = intercept::sqf::get_variable(killed_, "alive_profileUnitID", "");

                    profile->removeUnit(profileUnitID);
                } else {
                    ProfileVehicle* profile = static_cast<ProfileVehicle*>(_profileHandler.getProfile(profileID));

                    ProfileSystem::get().getProfileHandler().unregisterProfile(profile);
                }
            }
        }

        void ProfileSystem::_activateProfiles() {
            // get player positions

            std::vector<intercept::types::vector3> playerPositions;

            for (auto& unit : intercept::sqf::all_players())
                playerPositions.push_back(intercept::sqf::get_pos(unit));

            // activate profiles within
            // spawn distance of a player


            int spawnDistanceSqr = _spawnDistance * _spawnDistance;
            int despawnDistanceSqr = static_cast<int>(spawnDistanceSqr * 1.2);

            bool inRange;
            intercept::types::vector3 profilePos;


            for (auto& profile : _profileHandler._profiles) {
                if (profile->getProfileType() == ProfileType::VEHICLE && static_cast<ProfileVehicle*>(profile)->isGarrisoned()) continue;

                profilePos = profile->getPosition();

                for (auto& playerPos : playerPositions) {
                    if (profile->isActive()) {
                        inRange = true;

                        if (inRange) {
                            if (common::math::distanceSqr(playerPos, profilePos) > despawnDistanceSqr) {
                                inRange = false;

                                profile->despawn();
                            }
                        }
                    } else {
                        inRange = false;

                        if (!inRange) {
                            if (common::math::distanceSqr(playerPos, profilePos) < spawnDistanceSqr) {
                                inRange = true;

                                profile->spawn();
                            }
                        }
                    }
                }
            }
        }


    }
}