#include "profile_group.hpp"

#include "intercept.hpp"

#include "sys_profile\helpers.hpp"

#include "profile.hpp"
#include "profile_unit.hpp"
#include "profile_system.hpp"
#include "profile_handler.hpp"

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

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
                addUnit(new ProfileUnit(this, unitClass));

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

                    if (sqf::is_kind_of(unitClass,"Man")) {
                        // add unit to profile

                        addUnit(new ProfileUnit(this, sqf::get_text(configEntry >> "vehicle")));
                    } else {
                        // create vehicle profile
                        // link to this profile

                        auto vehicleProfile = ProfileVehicle::Create(side_, faction_, pos_, sqf::get_text(configEntry >> "vehicle"));

                        vehiclesToGarrison.push_back(vehicleProfile);
                    }
                }
            }

            for (auto& vehicle : vehiclesToGarrison)
                garrisonVehicle(vehicle);

            _calculateSpeed();
        }

        ProfileGroup::~ProfileGroup() {
            enableDebug(false);

            _units.clear();

            while (_vehicleAssignments.size() > 0)
                unGarrisonVehicle(unsigned int(0));
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


        ProfileUnit* ProfileGroup::getUnit(const std::string& id_) {
            for (auto unit : _units) if (unit->getID() == id_) return unit.get();

            return nullptr;
        }

        
        // setters


        void ProfileGroup::setPosition(const intercept::types::vector3& newPos_, bool moveObjects_) {
            _pos = newPos_;

            // move garrisoned vehicles

            if (moveObjects_)
                for (auto& assignment : _vehicleAssignments) assignment->vehicle->setPosition(_pos, moveObjects_);

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

            for (auto& vehicleAssignment : _vehicleAssignments)
                vehicleAssignment->vehicle->spawn();

            // spawn this profile

            _groupObject = sqf::create_group(_side);

            for (auto& unit : _units)
                unit->spawn(this);

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 0.8f);

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

                for (int i = static_cast<int>(sqf::current_waypoint(_groupObject)); i < groupWaypoints.size(); i++)
                    _waypoints.push_back(ProfileWaypoint(groupWaypoints[i]));
            }

            for (auto& unit : _units)
                unit->despawn();

            sqf::delete_group(_groupObject);

            // despawn garrisoned vehicles

            for (auto& vehicleAssignment : _vehicleAssignments)
                vehicleAssignment->vehicle->despawn();

            if (_debugEnabled)
                sqf::set_marker_alpha(_debugMarker, 0.3f);

            _active = false;
        }

        void ProfileGroup::addUnit(ProfileUnit* unit_) {
            _units.push_back(std::shared_ptr<ProfileUnit>(unit_));

            unit_->_id = _generateNextUnitID();

            // garrison unit to vehicle if possible

            for (auto& vehicleAssignment : _vehicleAssignments)
                if (!unit_->isInVehicle() && !vehicleAssignment->vehicle->isFullyGarrisoned()) unit_->getInVehicle(vehicleAssignment);

            _calculateSpeed();
        }

        void ProfileGroup::removeUnit(ProfileUnit* unit_) {
            this->removeUnit(unit_->getID());
        }

        void ProfileGroup::removeUnit(const std::string& unitID_) {
            auto it = std::find_if(_units.begin(), _units.end(), [&](std::shared_ptr<ProfileUnit>& unit_) { return unit_->getID() == unitID_; });

            if (it != _units.end()) {
                std::shared_ptr<ProfileUnit> unitCopy(*it); // #TODO: Keeps unit alive as long as the _units vector is being accessed inside it's destructor
                
                _units.erase(it);

                if (_units.size() > 0)
                    _calculateSpeed();
                else
                    ProfileSystem::get().getProfileHandler().unregisterProfile(this);
            }
        }

        bool ProfileGroup::garrisonVehicle(ProfileVehicle* vehicle_) {
            if (_vehicleAssignments.size() + 1 >= _units.size() || vehicle_->isGarrisoned())
                return false;
            
            _vehicleAssignments.push_back(new GroupVehicleAssignment(this, vehicle_));
            
            if (!_controlsHelicopter && vehicle_->getVehicleType() == common::vehicles::VehicleType::HELICOPTER)
                _controlsHelicopter = true;
            else if (!_controlsPlane && vehicle_->getVehicleType() == common::vehicles::VehicleType::PLANE)
                _controlsPlane = true;

            // assign units to vehicle
            
            GroupVehicleAssignment* assignment = _vehicleAssignments[_vehicleAssignments.size() - 1];
            
            auto it = _units.begin();

            while (it != _units.end() && !assignment->vehicle->isFullyGarrisoned()) {
                if (!(*it)->isInVehicle()) (*it)->getInVehicle(assignment);
                it++;
            }
            
            _calculateSpeed();
            
            return true;
        }

        bool ProfileGroup::unGarrisonVehicle(GroupVehicleAssignment* assignment_) {
            auto it = std::find(_vehicleAssignments.begin(), _vehicleAssignments.end(), assignment_);

            if (it == _vehicleAssignments.end())
                return false;

            // ungarrison units from vehicle

            for (auto& unit : (*it)->units)
                unit->_vehicleAssignment = nullptr;

            // remove garrison vehicle from vehicle

            (*it)->vehicle->_vehicleAssignment = nullptr;
            
            // delete vehicle assignment

            _vehicleAssignments.erase(it);

            // check if group still garrisons an air vehicle

            if (_controlsHelicopter || _controlsPlane) {
                _controlsHelicopter = false;
                _controlsPlane = false;

                for (auto& vehicleAssignment : _vehicleAssignments) {
                    if (vehicleAssignment->vehicle->getVehicleType() == common::vehicles::VehicleType::HELICOPTER)
                        _controlsHelicopter = true;
                    else if (vehicleAssignment->vehicle->getVehicleType() == common::vehicles::VehicleType::PLANE)
                        _controlsPlane = true;
                }
            }

            _calculateSpeed();

            return true;
        }

        bool ProfileGroup::unGarrisonVehicle(unsigned int index_) {
            if (index_ >= _vehicleAssignments.size())
                return false;

            return unGarrisonVehicle(*(_vehicleAssignments.begin() + index_));
        }

        bool ProfileGroup::unGarrisonVehicle(ProfileVehicle* vehicle_) {
            for (auto& assignment : _vehicleAssignments)
                if (vehicle_->getVehicleAssignment() == assignment) return unGarrisonVehicle(assignment);
            
            return false;
        }

        GroupVehicleAssignment* ProfileGroup::getVehicleAssignment(ProfileVehicle* vehicle_) {
            // return assignment if it already exists

            for (auto& assignment : _vehicleAssignments)
                if (vehicle_->getVehicleAssignment() == assignment) return assignment;

            // vehicle is not assigned to group
            // create assignment

            _vehicleAssignments.push_back(new GroupVehicleAssignment(this, vehicle_));

            return _vehicleAssignments[_vehicleAssignments.size() - 1];
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

        void ProfileGroup::onWaypointComplete() {
            if (_cycleWaypointCount > 0)
                addWaypoint(ProfileWaypoint(_waypoints[0]));

            removeWaypoint(_waypoints.begin());
        }

        void ProfileGroup::onUnitLeftAssignedVehicle(ProfileUnit* unit_, ProfileVehicle* vehicle_) {
            unit_->getVehicleAssignment()->removeUnit(unit_);

            if (vehicle_->getVehicleAssignment()->units.size() == 0) unGarrisonVehicle(vehicle_->getVehicleAssignment());
        }


        // protected


        std::string ProfileGroup::_generateNextUnitID() {
            return "profileUnit_" + std::to_string(_nextUnitID++);
        }

        void ProfileGroup::_calculateSpeed() {
            if (_units.size() == 0) {
                _speed = 0;
                return;
            }

            float minSpeed;

            bool allUnitsGarrisoned = true;
            
            if (_vehicleAssignments.size() == 0)
                allUnitsGarrisoned = false;
            else
                for (auto& unit : _units) if (!unit->isInVehicle()) allUnitsGarrisoned = false;

            if (allUnitsGarrisoned) {
                minSpeed = _vehicleAssignments[0]->vehicle->getSpeed();

                for(auto& vehicleAssignment : _vehicleAssignments)
                    if (vehicleAssignment->vehicle->getSpeed() < minSpeed) minSpeed = vehicleAssignment->vehicle->getSpeed();
            } else {
                minSpeed = _units[0]->getSpeed();

                for (auto& unit : _units)
                    if (unit->getSpeed() < minSpeed) minSpeed = unit->getSpeed();
            }

            _speed = minSpeed;
        }

        void ProfileGroup::_createDebugMarker() {
            _debugMarker = sqf::create_marker(_id + "_debug", _pos);

            sqf::set_marker_pos(_debugMarker, _pos);
            sqf::set_marker_size(_debugMarker, types::vector2(0.6f, 0.6f));

            if (_active)
                sqf::set_marker_alpha(_debugMarker, 0.8f);
            else
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
                    
                    for (auto& vehicleAssignment : _vehicleAssignments)
                        if (vehicleAssignment->vehicle->getDir() == 0) vehicleAssignment->vehicle->setDir(common::world::getRelDir(_pos, waypoint.position));
                    
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