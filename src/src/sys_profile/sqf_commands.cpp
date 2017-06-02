#include "sqf_commands.hpp"

#include "intercept.hpp"
#include "core\core.hpp"
#include "sys_profile\include.hpp"

#include "common\include.hpp"
namespace alive {
    namespace sys_profile {
        namespace sqf_commands {
            namespace handles {
                intercept::types::registered_sqf_function _startProfileSystem;
                intercept::types::registered_sqf_function _enableProfileSystemDebug;

                intercept::types::registered_sqf_function _getProfiles;
                intercept::types::registered_sqf_function _getProfilesBySide;

                intercept::types::registered_sqf_function _createProfile;
                intercept::types::registered_sqf_function _getProfilePosition;
                intercept::types::registered_sqf_function _getProfileSpeed;
            }

            // Profile System


            game_value startProfileSystem(game_value rightArg) {
                if (ProfileSystem::isInitialized())
                    return game_value();

                // parse args from input

                int spawnDistance = rightArg[0];

                // set args to profile system

                ProfileSystem::get().setSpawnDistance(spawnDistance);

                // register profile system with core

                alive::Core::get().registerModule(&ProfileSystem::get());

                return game_value();
            }

            game_value enableProfileSystemDebug(game_value rightArg_) {
                if (!ProfileSystem::isInitialized())
                    return game_value();

                bool enabled = rightArg_;

                ProfileSystem::get().enableDebug(enabled);

                return game_value();
            }


            // Profile Handler


            game_value getProfiles() {
                if (!ProfileSystem::isInitialized())
                    return auto_array<game_value>();

                auto profileVec = ProfileSystem::get().getProfileHandler().getProfiles();

                std::vector<std::string> profileStringVec;
                profileStringVec.reserve(profileVec.size());

                for (auto& profile : profileVec)
                    profileStringVec.push_back(profile->getID());

                return auto_array<game_value>(profileStringVec.begin(), profileStringVec.end());
            }

            game_value getProfilesBySide(game_value rightArg_) {
                if (!ProfileSystem::isInitialized())
                    return auto_array<game_value>();

                auto profileVec = ProfileSystem::get().getProfileHandler().getProfiles(rightArg_);

                std::vector<std::string> profileStringVec;
                profileStringVec.reserve(profileVec.size());

                for(auto& profile : profileVec)
                    profileStringVec.push_back(profile->getID());

                return auto_array<game_value>(profileStringVec.begin(), profileStringVec.end());
            }


            // Profiles


            game_value createProfile(game_value profileArgs) {
                if (!ProfileSystem::isInitialized())
                    return "";

                // parse args from input

                intercept::types::side side = profileArgs[0];
                intercept::types::vector3 pos = profileArgs[1];
                intercept::types::config groupConfig = profileArgs[2];

                // create profile

                ProfileGroup* profile = ProfileGroup::Create(side, pos, groupConfig);

                profile->getWaypoints().push_back(ProfileWaypoint{ {0,0,0} });

                return profile->getID();
            }

            game_value getProfilePosition(game_value profileID) {
                if (!ProfileSystem::isInitialized())
                    return intercept::types::vector3(0, 0, 0);

                std::string profileIDString = profileID;

                ProfileGroup* profile = ProfileSystem::get().getProfileHandler().getProfile(profileIDString);

                if (profile != nullptr)
                    return profile->getPosition();
                else
                    return intercept::types::vector3(0, 0, 0);
            }

            game_value getProfileSpeed(game_value profileID_) {
                if (!ProfileSystem::isInitialized())
                    return game_value(-1.f);

                std::string profileIDString = profileID_;

                ProfileGroup* profile = ProfileSystem::get().getProfileHandler().getProfile(profileIDString);

                if (profile != nullptr)
                    return static_cast<float>(profile->getSpeed());
                else
                    return game_value(-1.f);
            }
        }
    }
}