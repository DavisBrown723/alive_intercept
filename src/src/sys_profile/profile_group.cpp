#include "intercept.hpp"

#include "profile_group.hpp"

#include "profile_unit.hpp"
#include "profile_system.hpp"
#include "profile_handler.hpp"


namespace alive {
    namespace sys_profile {


        ProfileGroup::ProfileGroup()
            :
            _id(""),
            _active(false),
            _speed(0),
            _side(common::RV::get().sides.West),
            _pos(intercept::types::vector3())
        {

        }

        ProfileGroup::ProfileGroup(const intercept::types::side side_, const intercept::types::vector3& pos_, const std::vector<std::string>& unitClasses_)
            :
            _id(""),
            _active(false),
            _side(side_),
            _pos(pos_)
        {
            for (auto& unitClass : unitClasses_)
                _units.push_back(std::make_shared<ProfileUnit>(unitClass));

            _calculateSpeed();
        }

        ProfileGroup::ProfileGroup(const intercept::types::side side_, const intercept::types::vector3& pos_, const intercept::types::config& groupConfig_)
            :
            _id(""),
            _active(false),
            _side(side_),
            _pos(pos_)
        {
            std::vector<std::string> unitClasses;

            // get unit classes from group config

            int configSize = intercept::sqf::count(groupConfig_);
            intercept::types::config configItem;
            intercept::sqf::config_entry configEntry;

            for (int i = 0; i != configSize; i++) {
                configItem = intercept::sqf::select(groupConfig_, i);

                if (intercept::sqf::is_class(configItem)) {
                    configEntry = configItem;
                    unitClasses.push_back(intercept::sqf::get_text(configEntry >> "vehicle"));
                }
            }

            // create units

            for (auto& unitClass : unitClasses)
                _units.push_back(std::make_shared<ProfileUnit>(unitClass));

            _calculateSpeed();
        }

        ProfileGroup::~ProfileGroup() {

        }

        ProfileGroup* ProfileGroup::Create(const intercept::types::side side_, const intercept::types::vector3& pos_, const std::vector<std::string>& unitClasses_) {
            ProfileGroup* profile = new ProfileGroup(side_, pos_, unitClasses_);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }

        ProfileGroup* ProfileGroup::Create(const intercept::types::side side_, const intercept::types::vector3& pos_, const intercept::types::config& groupConfig_) {
            ProfileGroup* profile = new ProfileGroup(side_, pos_, groupConfig_);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }


        // getters / setters


        void ProfileGroup::enableDebug(bool enabled_) {
            if (enabled_ == _debugEnabled)
                return;

            _debugEnabled = enabled_;

            if (_debugEnabled) {
                _createDebugMarker();
            } else {
                intercept::sqf::delete_marker(_debugMarker);
            }
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

            _active = true;
        }

        void ProfileGroup::despawn() {
            if (!_active)
                return;

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

        void ProfileGroup::removeUnit(const std::string& unitID_) {
            for (auto& i = _units.begin(); i != _units.end(); i++) {
                if (i->get()->_id == unitID_) {
                    _units.erase(i);

                    return;
                }
            }

            _calculateSpeed();
        }


        // protected


        std::string ProfileGroup::_generateNextUnitID() {
            return _id + "_unit_" + std::to_string(_nextUnitID);
        }

        void ProfileGroup::_calculateSpeed() {
            int minSpeed = 0;

            if (_units.size() > 0) {
                minSpeed = _units[0]->getSpeed();

                for (auto& unit : _units)
                    if (unit->getSpeed() < minSpeed) minSpeed = unit->getSpeed();
            }

            _speed = minSpeed * 0.20;
        }

        void ProfileGroup::_createDebugMarker() {
            _debugMarker = intercept::sqf::create_marker(_id + "_debug", _pos);

            intercept::sqf::set_marker_pos(_debugMarker, _pos);
            intercept::sqf::set_marker_size(_debugMarker, intercept::types::vector2(0.6, 0.6));

            if (!_active)
                intercept::sqf::set_marker_alpha(_debugMarker, 0.3);

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
            if (!this->isActive()) {
                if (this->getWaypoints().size() > 0) {
                    int distToCompletion = (_waypoints[0].position.distance(this->getPosition())) - _waypoints[0].completionRadius;

                    int moveDist;
                    bool waypointComplete = false;

                    // determine distance to move

                    if (this->getSpeed() < distToCompletion) {
                        moveDist = this->getSpeed();
                    } else {
                        moveDist = distToCompletion;
                        waypointComplete = true;
                    }

                    // move profile

                    intercept::types::vector3 newPos = common::world::getRelPos(
                        this->getPosition(),
                        common::world::getRelDir(this->getPosition(), _waypoints[0].position),
                        moveDist * dt_
                    );

                    this->setPosition(newPos);

                    // #TODO: Handle different waypoint types

                    if (waypointComplete)
                        _waypoints.erase(_waypoints.begin());
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