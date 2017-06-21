#include "intercept.hpp"

#include "profile_group.hpp"

#include "profile.hpp"
#include "profile_unit.hpp"
#include "profile_system.hpp"
#include "profile_handler.hpp"

#include <vector>
#include <string>
#include <memory>

using namespace intercept;


namespace alive {
    namespace sys_profile {


        ProfileGroup::ProfileGroup()
            :
            Profile(common::RV::get().sides.West, "BLU_F", { 0,0,0 })
        {

        }

        ProfileGroup::ProfileGroup(
            const types::side side_,
            const std::string& faction_,
            const types::vector3& pos_,
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
            const types::side side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const types::config& groupConfig_
        )
            :
            Profile(side_, faction_, pos_)
        {
            std::vector<std::string> unitClasses;

            // get unit classes from group config

            int configSize = static_cast<int>(sqf::count(groupConfig_));
            types::config configItem;
            sqf::config_entry configEntry;

            std::vector<ProfileVehicle*> vehiclesToGarrison;

            for (int i = 0; i != configSize; i++) {
                configItem = sqf::select(groupConfig_, static_cast<float>(i));

                if (sqf::is_class(configItem)) {
                    configEntry = configItem;
                    std::string unitClass = sqf::get_text(configEntry >> "vehicle");

                    if (sqf::is_kind_of(unitClass, "Man")) {
                        // add unit to profile

                        unitClasses.push_back(sqf::get_text(configEntry >> "vehicle"));
                    } else {
                        // create vehicle profile
                        // link to this profile

                        auto vehicleProfile = ProfileVehicle::Create(side_, faction_, pos_, sqf::get_text(configEntry >> "vehicle"));

                        vehiclesToGarrison.push_back(vehicleProfile);
                    }
                }
            }

            // create units

            for (auto& unitClass : unitClasses)
                addUnit(new ProfileUnit(unitClass));

            // garrison vehicles

            for (auto& vehicle : vehiclesToGarrison)
                this->garrisonVehicle(vehicle);

            _calculateSpeed();
        }

        ProfileGroup::~ProfileGroup() {
            enableDebug(false);
        }

        ProfileGroup* ProfileGroup::Create(
            const types::side side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const std::vector<std::string>& unitClasses_
        ) {
            ProfileGroup* profile = new ProfileGroup(side_, faction_, pos_, unitClasses_);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }

        ProfileGroup* ProfileGroup::Create(
            const types::side side_,
            const std::string& faction_,
            const types::vector3& pos_,
            const types::config& groupConfig_
        ) {
            ProfileGroup* profile = new ProfileGroup(side_, faction_, pos_, groupConfig_);

            ProfileSystem::get().getProfileHandler().registerProfile(profile);

            return profile;
        }

        
        // getters

        
        // setters


        void ProfileGroup::setPosition(const intercept::types::vector3& newPos_, bool moveObjects_) {
            _pos = newPos_;

            // move garrisoned vehicles
            // #TODO: Improve

            if (_pos.z > 2)
                sqf::system_chat("SENDING ABOVE ZERO");

            for (auto& unit : _units)
                if (unit->isInVehicle()) unit->getOccupiedVehicle()->setPosition(_pos, moveObjects_);

            if (moveObjects_ && _active)
                for (auto& unit : _units) sqf::set_pos(unit->_unitObject, newPos_);

            if (_debugEnabled)
                sqf::set_marker_pos(_debugMarker, newPos_);
        }


        // functional


        void ProfileGroup::update(const float dt_) {
            _updateMovement(dt_);
        }

        void ProfileGroup::spawn() {
            if (_active)
                return;

            // spawn garrisoned vehicles first

            for (auto& vehicle : _garrisonedVehicles)
                vehicle->spawn();

            // spawn this profile

            _groupObject = sqf::create_group(_side);

            for (auto& unit : _units)
                unit->spawn(this);

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 1.f);

            // ProfileWaypoints to real waypoints

            for (auto& waypoint : _waypoints)
                waypoint.toRVWaypoint(this, _groupObject);

            _active = true;
        }

        void ProfileGroup::despawn() {
            if (!_active)
                return;

            // real waypoints to profile waypoints

            auto groupWaypoints = sqf::waypoints(_groupObject);

            // update profile waypoints
            // with any waypoints added
            // during spawn

            if (_waypoints.size() != groupWaypoints.size()) {
                _waypoints.clear();

                for (int i = static_cast<int>(sqf::current_waypoint(_groupObject)); i != groupWaypoints.size(); i++)
                    _waypoints.push_back(ProfileWaypoint(groupWaypoints[i]));
            }

            for (auto& unit : _units)
                unit->despawn();

            sqf::delete_group(_groupObject);

            // despawn garrisoned vehicles

            for (auto& vehicle : _garrisonedVehicles)
                vehicle->despawn();

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 0.3f);

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

            _calculateSpeed();
        }

        bool ProfileGroup::garrisonVehicle(ProfileVehicle* vehicle_) {
            if (_garrisonedVehicles.size() + 1 >= _units.size() || vehicle_->isGarrisoned())
                return false;

            _garrisonedVehicles.push_back(vehicle_);

            auto it = _units.begin();

            while (it != _units.end()) {
                if (!(*it)->isInVehicle()) (*it)->getInVehicle(vehicle_);
                it++;
            }

            _calculateSpeed();

            return true;
        }

        bool ProfileGroup::unGarrisonVehicle(unsigned int index_) {
            if (index_ >= _garrisonedVehicles.size())
                return false;

            _garrisonedVehicles.erase(_garrisonedVehicles.begin() + index_);

            _calculateSpeed();

            return true;
        }

        bool ProfileGroup::unGarrisonVehicle(ProfileVehicle* vehicle_) {
            auto it = std::find(_garrisonedVehicles.begin(), _garrisonedVehicles.end(), vehicle_);

            if (it != _garrisonedVehicles.end()) {
                unGarrisonVehicle(it - _garrisonedVehicles.begin());
                return true;
            }

            return false;
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
            float minSpeed;

            bool allUnitsGarrisoned = true;

            for (auto& unit : _units) if (!unit->isInVehicle()) allUnitsGarrisoned = false;

            if (allUnitsGarrisoned) {
                minSpeed = _garrisonedVehicles[0]->getSpeed();

                for(auto& vehicle : _garrisonedVehicles)
                    if (vehicle->getSpeed() < minSpeed) minSpeed = vehicle->getSpeed();
            } else {
                if (_units.size() > 0) {
                    minSpeed = _units[0]->getSpeed();

                    for (auto& unit : _units)
                        if (unit->getSpeed() < minSpeed) minSpeed = unit->getSpeed();
                }
            }

            _speed = minSpeed;
        }

        void ProfileGroup::_createDebugMarker() {
            _debugMarker = sqf::create_marker(_id + "_debug", _pos);

            sqf::set_marker_pos(_debugMarker, _pos);
            sqf::set_marker_size(_debugMarker, types::vector2(0.6f, 0.6f));

            if (!_active)
                sqf::set_marker_alpha(_debugMarker, 0.3f);

            if (_side == common::RV::get().sides.East) {
                sqf::set_marker_color(_debugMarker, "ColorRed");
                sqf::set_marker_type(_debugMarker, "o_inf");
            } else if (_side == common::RV::get().sides.West) {
                sqf::set_marker_color(_debugMarker, "ColorBlue");
                sqf::set_marker_type(_debugMarker, "b_inf");
            } else if (_side == common::RV::get().sides.Guer) {
                sqf::set_marker_color(_debugMarker, "ColorGreen");
                sqf::set_marker_type(_debugMarker, "n_inf");
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

                    float moveDist;
                    bool waypointComplete = false;
                    float speed;

                    switch (waypoint.speed) {
                        case ProfileWaypoint::Speed::LIMITED:
                        {
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
                        case ProfileWaypoint::Speed::UNCHANGED:
                        {
                            speed = _speed;
                        }
                    }

                    // determine distance to move

                    if (speed < distToCompletion) {
                        moveDist = speed;
                    } else {
                        moveDist = static_cast<float>(distToCompletion);
                        waypointComplete = true;
                    }

                    // move profile

                    for (auto& vehicle : _garrisonedVehicles)
                        if (vehicle->getDir() == 0) vehicle->setDir(common::world::getRelDir(_pos, waypoint.position));

                    types::vector3 newPos = common::math::lerp(
                        _pos,
                        waypoint.position,
                        moveDist * dt_ / static_cast<float>(distToCompletion)
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

                types::object leader = sqf::leader(_groupObject);

                intercept::types::vector3 pos = sqf::get_pos_atl(leader);
                pos.z = 0;

                if (!leader.is_null())
                    this->setPosition(pos);
            }
        }


    }
}