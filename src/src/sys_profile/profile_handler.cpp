#include "profile_handler.hpp"

#include "common\include.hpp"
#include "profile_group.hpp"

#include <memory>
#include <algorithm>

namespace alive {
    namespace sys_profile {


        ProfileHandler::ProfileHandler() {

        }
        
        ProfileHandler::~ProfileHandler() {

        }

        void ProfileHandler::registerProfile(ProfileGroup* profile_) {
            profile_->_id = _generateID();

            // profile is owned by profile map

            _profileMap.insert({ profile_->getID(), ProfileHandler::SharedProfilePtr(profile_) });

            // additional filtered profile
            // storages for quick access

            _profiles.push_back(profile_);

            if (profile_->getSide() == RV::get().sides.East)
                _profilesEast.push_back(profile_);
            else if (profile_->getSide() == RV::get().sides.West)
                _profilesWest.push_back(profile_);
            else if (profile_->getSide() == RV::get().sides.Guer)
                _profilesGuer.push_back(profile_);

            if (_debugEnabled) profile_->enableDebug(true);
        }

        void ProfileHandler::unregisterProfile(ProfileGroup* profile_) {
            ProfileVector::iterator it;

            it = std::find(_profiles.begin(), _profiles.end(), profile_);

            // exit if profile was not registered

            if (it == _profiles.end())
                return;

            _profiles.erase(it);

            if (profile_->getSide() == RV::get().sides.East)
                _profilesEast.erase(std::find(_profilesEast.begin(), _profilesEast.end(), profile_));
            else if (profile_->getSide() == RV::get().sides.West)
                _profilesWest.erase(std::find(_profilesWest.begin(), _profilesWest.end(), profile_));
            else if (profile_->getSide() == RV::get().sides.Guer)
                _profilesGuer.erase(std::find(_profilesGuer.begin(), _profilesGuer.end(), profile_));

            // remove profile from map
            // destroys profile object

            _profileMap.erase(profile_->getID());
        }

        ProfileGroup* ProfileHandler::getProfile(const std::string& id_) {
            auto it = _profileMap.find(id_);

            if (it == _profileMap.end())
                return nullptr;

            return it->second.get();
        }

        const ProfileHandler::ProfileVector& ProfileHandler::getProfiles() {            
            return _profiles;
        }

        const ProfileHandler::ProfileVector& ProfileHandler::getProfiles(const intercept::types::side side_) {
            if (side_ == RV::get().sides.East)
                return _profilesEast;
            else if (side_ == RV::get().sides.West)
                return _profilesWest;
            else if (side_ == RV::get().sides.Guer)
                return _profilesGuer;

            throw std::invalid_argument("ProfileHandler: getProfiles - Invalid Side Passed");

            return _profilesEast;
        }


        // private


        std::string ProfileHandler::_generateID() {
            return "p_" + std::to_string(nextProfileID++);
        }

    }
}