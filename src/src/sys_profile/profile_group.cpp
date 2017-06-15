#include "intercept.hpp"

#include "profile_group.hpp"

#include "profile.hpp"
#include "profile_unit.hpp"
#include "profile_system.hpp"
#include "profile_handler.hpp"

#include <vector>
#include <string>
#include <memory>


namespace alive {
    namespace sys_profile {


        ProfileGroup::ProfileGroup()
            :
            Profile(common::RV::get().sides.West, "BLU_F", { 0,0,0 })
        {

        }

        ProfileGroup::ProfileGroup(
            const intercept::types::side side_,
            const std::string& faction_,
            const intercept::types::vector3& pos_,
            const std::vector<std::string>& unitClasses_
        )
            :
            Profile(side_, faction_, pos_)
        {
            for (auto& unitClass : unitClasses_)
                addUnit(new ProfileUnit(unitClass));

            _calculateSpeed();
        }

        ProfileGroup::ProfileGroup(
            const intercept::types::side side_,
            const std::string& faction_,
            const intercept::types::vector3& pos_,
            const intercept::types::config& groupConfig_
        )
            :
            Profile(side_, faction_, pos_)
        {
            std::vector<std::string> unitClasses;

            // get unit classes from group config

            int configSize = static_cast<int>(intercept::sqf::count(groupConfig_));
            intercept::types::config configItem;
            intercept::sqf::config_entry configEntry;

            for (int i = 0; i != configSize; i++) {
                configItem = intercept::sqf::select(groupConfig_, static_cast<float>(i));

                if (intercept::sqf::is_class(configItem)) {
                    configEntry = configItem;
                    unitClasses.push_back(intercept::sqf::get_text(configEntry >> "vehicle"));
                }
            }

            // create units

            for (auto& unitClass : unitClasses)
                addUnit(new ProfileUnit(unitClass));

            _calculateSpeed();
        }

        ProfileGroup::~ProfileGroup() {
            enableDebug(false);
        }

        ProfileGroup* ProfileGroup::Create(
            const intercept::types::side side_,
            const std::string& faction_,
            const intercept::types::vector3& pos_,
            const std::vector<std::string>& unitClasses_
        ) {
            ProfileGroup* profile = new ProfileGroup(side_, faction_, pos_, unitClasses_);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }

        ProfileGroup* ProfileGroup::Create(
            const intercept::types::side side_,
            const std::string& faction_,
            const intercept::types::vector3& pos_,
            const intercept::types::config& groupConfig_
        ) {
            ProfileGroup* profile = new ProfileGroup(side_, faction_, pos_, groupConfig_);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }

        
        // getters

        ProfileType getProfileType() {
            return ProfileType::INFANTRY;

            // get profile type based on occupied vehicles
        }


        // functional


        void ProfileGroup::update(const float dt_) {
            _updateMovement(dt_);

            if (_debugEnabled)
                intercept::sqf::set_marker_pos(_debugMarker, _pos);
        }

        void ProfileGroup::spawn() {
            if (_active)
                return;

            _groupObject = intercept::sqf::create_group(_side);

            for (auto& unit : _units)
                unit->spawn(this);

            if (_debugEnabled)
                intercept::sqf::set_marker_alpha(_debugMarker, 1.f);

            // ProfileWaypoints to real waypoints

            for (auto& waypoint : _waypoints)
                waypoint.toRVWaypoint(this, _groupObject);

            _active = true;
        }

        void ProfileGroup::despawn() {
            if (!_active)
                return;

            // real waypoints to profile waypoints

            auto groupWaypoints = intercept::sqf::waypoints(_groupObject);

            // update profile waypoints
            // with any waypoints added
            // during spawn

            if (_waypoints.size() != groupWaypoints.size()) {
                _waypoints.clear();

                for (int i = static_cast<int>(intercept::sqf::current_waypoint(_groupObject)); i != groupWaypoints.size(); i++)
                    _waypoints.push_back(ProfileWaypoint(groupWaypoints[i]));
            }

            for (auto& unit : _units)
                unit->despawn();

            intercept::sqf::delete_group(_groupObject);

            if (_debugEnabled)
                intercept::sqf::set_marker_alpha(_debugMarker, 0.3f);

            _active = false;
        }

        void ProfileGroup::addUnit(ProfileUnit* unit_) {
            _units.push_back(std::shared_ptr<ProfileUnit>(unit_));

            unit_->_id = _generateNextUnitID();

            _calculateSpeed();
        }

        void ProfileGroup::removeUnit(ProfileUnit* unit_) {
            this->removeUnit(unit_->getID());
        }

        void ProfileGroup::removeUnit(const std::string& unitID_) {
            for (auto& i = _units.begin(); i != _units.end(); i++) {
                if (i->get()->_id == unitID_) {
                    _units.erase(i);

                    if (_units.size() == 0)
                        ProfileSystem::get().getProfileHandler().unregisterProfile(this);
                    else
                        _calculateSpeed();

                    return;
                }
            }
        }

        int ProfileGroup::addWaypoint(ProfileWaypoint& wp_) {
            if (wp_.type == ProfileWaypoint::Type::CYCLE)
                _cycleWaypointCount++;

            _waypoints.push_back(wp_);

            if (_active)
                wp_.toRVWaypoint(this, _groupObject);

            return _waypoints.size() - 1;
        }

        void ProfileGroup::removeWaypoint(const std::vector<ProfileWaypoint>::iterator& toDelete_) {
            if (toDelete_->type == ProfileWaypoint::Type::CYCLE)
                _cycleWaypointCount--;

            _waypoints.erase(toDelete_);
        }

        void ProfileGroup::removeWaypoint(unsigned int index_) {
            if (index_ < _waypoints.size())
                removeWaypoint(_waypoints.begin() + index_);
        }


        // protected


        std::string ProfileGroup::_generateNextUnitID() {
            return "profileUnit_" + std::to_string(_nextUnitID++);
        }

        void ProfileGroup::_calculateSpeed() {
            int minSpeed = 0;

            if (_units.size() > 0) {
                minSpeed = _units[0]->getSpeed();

                for (auto& unit : _units)
                    if (unit->getSpeed() < minSpeed) minSpeed = unit->getSpeed();
            }

            _speed = static_cast<int>(minSpeed);
        }

        void ProfileGroup::_createDebugMarker() {
            _debugMarker = intercept::sqf::create_marker(_id + "_debug", _pos);

            intercept::sqf::set_marker_pos(_debugMarker, _pos);
            intercept::sqf::set_marker_size(_debugMarker, intercept::types::vector2(0.6f, 0.6f));

            if (!_active)
                intercept::sqf::set_marker_alpha(_debugMarker, 0.3f);

            if (_side == common::RV::get().sides.East) {
                intercept::sqf::set_marker_color(_debugMarker, "ColorRed");
                intercept::sqf::set_marker_type(_debugMarker, "o_inf");
            } else if (_side == common::RV::get().sides.West) {
                intercept::sqf::set_marker_color(_debugMarker, "ColorBlue");
                intercept::sqf::set_marker_type(_debugMarker, "b_inf");
            } else if (_side == common::RV::get().sides.Guer) {
                intercept::sqf::set_marker_color(_debugMarker, "ColorGreen");
                intercept::sqf::set_marker_type(_debugMarker, "n_inf");
            }
        }

        void ProfileGroup::_updateMovement(const float dt_) {
            if (!_active) {
                if (_waypoints.size() > 0) {
                    ProfileWaypoint& waypoint = _waypoints[0];

                    int distToCompletion = static_cast<int>(
                        common::math::distance(waypoint.position, _pos) - waypoint.completionRadius
                    );

                    if (distToCompletion < 1) {
                        this->removeWaypoint(_waypoints.begin());
                        return;
                    }

                    int moveDist;
                    bool waypointComplete = false;
                    int speed;

                    switch (waypoint.speed) {
                        case ProfileWaypoint::Speed::LIMITED: {
                            speed = _speed * 0.33f;
                            break;
                        }
                        case ProfileWaypoint::Speed::NORMAL:
                        {
                            speed = _speed;
                            break;
                        }
                        case ProfileWaypoint::Speed::FULL:
                        {
                            speed = _speed * 1.33f;
                            break;
                        }
                    }

                    // determine distance to move

                    if (_speed < distToCompletion) {
                        moveDist = _speed;
                    } else {
                        moveDist = distToCompletion;
                        waypointComplete = true;
                    }

                    // move profile

                    intercept::types::vector3 newPos = common::math::lerp(
                        _pos,
                        waypoint.position,
                        static_cast<float>(moveDist) * dt_ / static_cast<float>(distToCompletion)
                    );

                    this->setPosition(newPos);

                    // #TODO: Handle different waypoint types

                    if (waypointComplete) {
                        if (_cycleWaypointCount > 0)
                            _waypointsCompleted.push_back(ProfileWaypoint(waypoint));

                        this->removeWaypoint(_waypoints.begin());

                        if (waypoint.type == ProfileWaypoint::Type::CYCLE) {
                            for (auto& completedWP : _waypointsCompleted)
                                addWaypoint(ProfileWaypoint(completedWP));

                            _waypointsCompleted.clear();
                        }
                    }
                }
            } else {
                // profile is spawned

                intercept::types::object leader = intercept::sqf::leader(_groupObject);

                if (!leader.is_null())
                    this->setPosition(intercept::sqf::get_pos(leader));
            }
        }


    }
}