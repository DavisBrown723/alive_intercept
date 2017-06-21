#pragma once

#include "common\include.hpp"
#include "profile.hpp"

#include <string>
#include <vector>
#include <memory>


namespace alive {
    namespace sys_profile {


        class ProfileHandler {


            friend class ProfileSystem;


            public:


                using RV = common::RV;

                typedef std::shared_ptr<Profile> SharedProfilePtr;
                typedef std::unordered_map< std::string , SharedProfilePtr > ProfileMap;
                typedef std::vector<Profile*> ProfileVector;

                ProfileHandler();
                ~ProfileHandler();


                // getters


                bool debugEnabled() const                           { return _debugEnabled; }

                const ProfileVector& getProfiles() const            { return _profiles; }

                const ProfileVector& getProfiles(const intercept::types::side side_);


                // setters


                void enableDebug(bool enabled_) {
                    _debugEnabled = enabled_;

                    for (auto& profile : _profilesEast) profile->enableDebug(enabled_);
                    for (auto& profile : _profilesWest) profile->enableDebug(enabled_);
                    for (auto& profile : _profilesGuer) profile->enableDebug(enabled_);
                }


                // functional


                void registerProfile(Profile* profile_);
                void unregisterProfile(Profile* profile_);

                Profile* getProfile(const std::string& id_);


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