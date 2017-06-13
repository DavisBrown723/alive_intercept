#pragma once

#include "common\include.hpp"
#include "profile_group.hpp"

#include <string>
#include <vector>
#include <memory>


namespace alive {
    namespace sys_profile {


        class ProfileHandler {


            friend class ProfileSystem;


            public:


                using RV = common::RV;

                typedef std::shared_ptr<ProfileGroup> SharedProfilePtr;
                typedef std::unordered_map< std::string , SharedProfilePtr > ProfileMap;
                typedef std::vector<ProfileGroup*> ProfileVector;

                ProfileHandler();
                ~ProfileHandler();


                // getters / setters


                bool debugEnabled() { return _debugEnabled; }

                void enableDebug(bool enabled_) {
                    _debugEnabled = enabled_;

                    for (auto& profile : _profilesEast) profile->enableDebug(enabled_);
                    for (auto& profile : _profilesWest) profile->enableDebug(enabled_);
                    for (auto& profile : _profilesGuer) profile->enableDebug(enabled_);
                }


                // functional


                void registerProfile(ProfileGroup* profile_);
                void unregisterProfile(ProfileGroup* profile_);

                ProfileGroup* getProfile(const std::string& id_);

                const ProfileVector& getProfiles();
                const ProfileVector& getProfiles(const intercept::types::side side_);


            protected:


                bool _debugEnabled = false;

                ProfileMap _profileMap;
                ProfileVector _profiles;

                ProfileVector _profilesEast;
                ProfileVector _profilesWest;
                ProfileVector _profilesGuer;

                int nextProfileID = 0;

                std::string _generateID();


        };


    }
}