#include "sqf_commands.hpp"

#include "intercept.hpp"
#include "core\core.hpp"
#include "sys_profile\include.hpp"

#include "common\include.hpp"
namespace alive {
    namespace sys_profile {
        namespace sqf_commands {
            namespace handles {
                //

                intercept::types::registered_sqf_function _profileWaypointComplete;

                //

                intercept::types::registered_sqf_function _startProfileSystem;
                intercept::types::registered_sqf_function _enableProfileSystemDebug;

                intercept::types::registered_sqf_function _getProfiles;
                intercept::types::registered_sqf_function _getProfilesBySide;

                intercept::types::registered_sqf_function _createProfileGroup;
                intercept::types::registered_sqf_function _createProfileVehicle;
                intercept::types::registered_sqf_function _getProfilePosition;
                intercept::types::registered_sqf_function _getProfileSpeed;

                intercept::types::registered_sqf_function _profileAddWaypoint;
                intercept::types::registered_sqf_function _profileRemoveWaypoint;
            }

            void registerScriptCommands() {
                // 

                sqf_commands::handles::_profileWaypointComplete = intercept::client::host::registerFunction(
                    "profileWaypointComplete",
                    "Registers an active profile's waypoint as complete.",
                    userFunctionWrapper<sqf_commands::profileWaypointComplete>,
                    intercept::types::GameDataType::NOTHING,
                    intercept::types::GameDataType::STRING
                );

                // Profile System

                sqf_commands::handles::_startProfileSystem = intercept::client::host::registerFunction(
                    "startProfileSystem",
                    "Starts the ALiVE Profile System with the given arguments.",
                    userFunctionWrapper<sqf_commands::startProfileSystem>,
                    intercept::types::GameDataType::NOTHING,
                    intercept::types::GameDataType::ARRAY
                );

                sqf_commands::handles::_enableProfileSystemDebug = intercept::client::host::registerFunction(
                    "enableProfileSystemDebug",
                    "Sets debug on the ALiVE Profile System.",
                    userFunctionWrapper<sqf_commands::enableProfileSystemDebug>,
                    intercept::types::GameDataType::NOTHING,
                    intercept::types::GameDataType::BOOL
                );

                // Profile Handler

                sqf_commands::handles::_getProfiles = intercept::client::host::registerFunction(
                    "getProfiles",
                    "Returns an array of all ALiVE Profiles.",
                    userFunctionWrapper<sqf_commands::getProfiles>,
                    intercept::types::GameDataType::ARRAY
                );

                sqf_commands::handles::_getProfilesBySide = intercept::client::host::registerFunction(
                    "getProfilesBySide",
                    "Returns an array of all ALiVE Profiles belonging to the given side.",
                    userFunctionWrapper<sqf_commands::getProfilesBySide>,
                    intercept::types::GameDataType::ARRAY,
                    intercept::types::GameDataType::SIDE
                );

                // Profiles

                sqf_commands::handles::_createProfileGroup = intercept::client::host::registerFunction(
                    "createProfileGroup",
                    "Creates an ALiVE profile group.",
                    userFunctionWrapper<sqf_commands::createProfileGroup>,
                    intercept::types::GameDataType::STRING,
                    intercept::types::GameDataType::ARRAY
                );

                sqf_commands::handles::_createProfileVehicle = intercept::client::host::registerFunction(
                    "createProfileVehicle",
                    "Creates an ALiVE profile vehicle.",
                    userFunctionWrapper<sqf_commands::createProfileVehicle>,
                    intercept::types::GameDataType::STRING,
                    intercept::types::GameDataType::ARRAY
                );

                sqf_commands::handles::_getProfilePosition = intercept::client::host::registerFunction(
                    "getProfilePosition",
                    "Gets the position of an ALiVE Profile.",
                    userFunctionWrapper<sqf_commands::getProfilePosition>,
                    intercept::types::GameDataType::ARRAY,
                    intercept::types::GameDataType::STRING
                );

                sqf_commands::handles::_getProfileSpeed = intercept::client::host::registerFunction(
                    "getProfileSpeed",
                    "Gets the speed of an ALiVE Profile.",
                    userFunctionWrapper<sqf_commands::getProfileSpeed>,
                    intercept::types::GameDataType::ARRAY,
                    intercept::types::GameDataType::STRING
                );
            }

            //

            game_value profileWaypointComplete(game_value rightArg_) {
                if (ProfileSystem::isInitialized())
                    return game_value();

                ProfileGroup* profile = (ProfileGroup*)ProfileSystem::get().getProfileHandler().getProfile(static_cast<std::string>(rightArg_));

                if (profile != nullptr)
                    profile->removeWaypoint(0);

                return game_value();
            }

            //

            // Profile System


            game_value startProfileSystem(game_value rightArg_) {
                if (ProfileSystem::isInitialized())
                    return game_value();

                // parse args from input

                int spawnDistance = rightArg_[0];

                // set args to profile system

                ProfileSystem::get().setSpawnDistance(spawnDistance);

                // start profile system

                ProfileSystem::get().start();

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


            game_value createProfileGroup(game_value profileArgs_) {
                if (!ProfileSystem::isInitialized())
                    return "";

                // parse args from input

                intercept::types::side side = profileArgs_[0];
                std::string faction = profileArgs_[1];
                intercept::types::vector3 pos = profileArgs_[2];

                // create profile

                ProfileGroup* profile;

                if (profileArgs_[3].type() == intercept::types::game_data_config().type_def) {
                    // create profile from group config

                    intercept::types::config groupConfig = profileArgs_[3];

                    profile = ProfileGroup::Create(side, faction, pos, groupConfig);
                } else {
                    // create profile from unit class array

                    std::vector<std::string> unitClasses;

                    for (auto& item : profileArgs_[3].to_array())
                        unitClasses.push_back(static_cast<std::string>(item));

                    profile = ProfileGroup::Create(side, faction, pos, unitClasses);
                }

                profile->addWaypoint(ProfileWaypoint{ {5000,5000,0} });

                return profile->getID();
            }

            game_value createProfileVehicle(game_value profileArgs_) {
                if (!ProfileSystem::isInitialized())
                    return "";

                // parse args from input

                intercept::types::side side = profileArgs_[0];
                std::string faction = profileArgs_[1];
                intercept::types::vector3 pos = profileArgs_[2];
                std::string vehicleClass = profileArgs_[3];

                // create profile

                ProfileVehicle* profile;

                if (profileArgs_[3].type() == intercept::types::game_data_config().type_def) {
                    // create profile from group config

                    intercept::types::config groupConfig = profileArgs_[3];

                    profile = ProfileVehicle::Create(side, faction, pos, groupConfig);
                } else {
                    // create profile from unit class array

                    std::string vehicleClass = profileArgs_[3];

                    profile = ProfileVehicle::Create(side, faction, pos, vehicleClass);
                }

                return profile->getID();
            }

            game_value getProfilePosition(game_value profileID_) {
                if (!ProfileSystem::isInitialized())
                    return intercept::types::vector3(0, 0, 0);

                ProfileGroup* profile = (ProfileGroup*)ProfileSystem::get().getProfileHandler().getProfile(static_cast<std::string>(profileID_));

                if (profile != nullptr)
                    return profile->getPosition();
                else
                    return intercept::types::vector3(0, 0, 0);
            }

            game_value getProfileSpeed(game_value profileID_) {
                if (!ProfileSystem::isInitialized())
                    return game_value(-1.f);

                ProfileGroup* profile = static_cast<ProfileGroup*>(
                    ProfileSystem::get().getProfileHandler().getProfile(static_cast<std::string>(profileID_))
                );

                if (profile != nullptr)
                    return static_cast<float>(profile->getSpeed());
                else
                    return game_value(-1.f);
            }

            game_value profileAddWaypoint(game_value profileID_, game_value waypointArgs_) {
                if (!ProfileSystem::isInitialized())
                    return game_value();

                ProfileGroup* profile = static_cast<ProfileGroup*>(
                    ProfileSystem::get().getProfileHandler().getProfile(static_cast<std::string>(profileID_))
                );

                if (profile != nullptr) {
                    intercept::types::vector3 position = waypointArgs_[0];
                    auto type = ProfileWaypoint::Type::MOVE;
                    int completionRadius = 50;
                    auto speed = ProfileWaypoint::Speed::UNCHANGED;
                    auto formation = ProfileWaypoint::Formation::WEDGE;
                    auto behavior = ProfileWaypoint::Behavior::UNCHANGED;
                    auto combatMode = ProfileWaypoint::CombatMode::NO_CHANGE;

                    if (waypointArgs_.size() > 1)
                        type = ProfileWaypoint::RVWaypoint::__parse_type(waypointArgs_[1]);
                    
                    if (waypointArgs_.size() > 2)
                        completionRadius = waypointArgs_[2];

                    if (waypointArgs_.size() > 3)
                        speed = ProfileWaypoint::RVWaypoint::__parse_speed(waypointArgs_[3]);

                    if (waypointArgs_.size() > 4)
                        formation = ProfileWaypoint::RVWaypoint::__parse_formation(waypointArgs_[4]);

                    if (waypointArgs_.size() > 5)
                        behavior = ProfileWaypoint::RVWaypoint::__parse_behaviour(waypointArgs_[5]);

                    if (waypointArgs_.size() > 6)
                        combatMode = ProfileWaypoint::RVWaypoint::__parse_combat_mode(waypointArgs_[6]);

                    int index = profile->addWaypoint(ProfileWaypoint(position, type, completionRadius, speed, formation, behavior, combatMode));
                        
                    return game_value(index);
                } else {
                    return game_value(-1);
                }
            }

            game_value profileRemoveWaypoint(game_value profileID_, game_value waypointIndex_) {
                return game_value();
            }
        }
    }
}