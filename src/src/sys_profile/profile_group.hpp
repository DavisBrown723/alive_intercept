#pragma once

#include "intercept.hpp"

#include "sys_profile\helpers.hpp"

#include "profile.hpp"
#include "profile_vehicle.hpp"
#include "profile_unit.hpp"
#include "profile_waypoint.hpp"

#include <vector>
#include <string>
#include <memory>

namespace alive {
    namespace sys_profile {



        class ProfileGroup : public Profile {

            friend class ProfileHandler;
            friend class GroupVehicleAssignment;

            public:


                ProfileGroup();
                ProfileGroup(
                    const intercept::types::side side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const std::vector<std::string>& unitClasses_
                );

                ProfileGroup(
                    const intercept::types::side side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const intercept::types::config& groupConfig_
                );

                virtual ~ProfileGroup();

                static ProfileGroup* Create(
                    const intercept::types::side side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const std::vector<std::string>& unitClasses_
                );

                static ProfileGroup* Create(
                    const intercept::types::side side_,
                    const std::string& faction_,
                    const intercept::types::vector3& pos_,
                    const intercept::types::config& groupConfig_
                );

                
                // getters


                intercept::types::group getGroupObject() const                              { return _groupObject; }
                intercept::types::side getSide() const                                      { return _side; }
                const std::vector< std::shared_ptr<ProfileUnit> >& getUnits()const          { return _units; }
                const std::vector<GroupVehicleAssignment*>& getVehicleAssignments() const   { return _vehicleAssignments; }
                const std::vector< ProfileWaypoint >& getWaypoints() const                  { return _waypoints; }
                ProfileType getProfileType() const override                                 { return ProfileType::INFANTRY; }

                ProfileUnit* getUnit(const std::string& id_);
                

                // setters


                virtual void setPosition(const intercept::types::vector3& newPos_, bool moveObjects_ = false) override;


                // functional

                
                virtual void update(const float dt_) override;

                virtual void spawn() override;
                virtual void despawn() override;

                virtual void addUnit(ProfileUnit* unit_);
                virtual void removeUnit(ProfileUnit* unit_);
                virtual void removeUnit(const std::string& unitID_);

                virtual bool garrisonVehicle(ProfileVehicle* vehicle_);
                virtual bool unGarrisonVehicle(unsigned int index_);
                virtual bool unGarrisonVehicle(GroupVehicleAssignment* assignment_);
                virtual bool unGarrisonVehicle(ProfileVehicle* vehicle_);
                virtual GroupVehicleAssignment* getVehicleAssignment(ProfileVehicle* vehicle_);

                virtual int addWaypoint(ProfileWaypoint& wp_);
                virtual void removeWaypoint(unsigned int index_);
                virtual void removeWaypoint(const std::vector<ProfileWaypoint>::iterator& toDelete_);

                void onUnitLeftAssignedVehicle(ProfileUnit* unit_, ProfileVehicle* vehicle_);


            protected:


                int _cycleWaypointCount = 0;
                int _nextUnitID = 0;

                std::vector< std::shared_ptr<ProfileUnit> > _units;
                std::vector<GroupVehicleAssignment*>         _vehicleAssignments;
                std::vector<ProfileWaypoint>                _waypoints;
                std::vector<ProfileWaypoint>                _waypointsCompleted;
                intercept::types::group                     _groupObject;

                bool _controlsHelicopter = false;
                bool _controlsPlane = false;

                bool _ungarrisonStarted = false;


                // functions


                std::string _generateNextUnitID();

                virtual void _calculateSpeed() override;
                virtual void _createDebugMarker() override;

                virtual void _updateMovement(const float dt_);

        };


    }
}