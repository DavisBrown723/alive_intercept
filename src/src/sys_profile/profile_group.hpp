#pragma once

#include "intercept.hpp"

#include "profile.hpp"
#include "profile_unit.hpp"
#include "profile_waypoint.hpp"

#include "common\include.hpp"

#include <vector>
#include <string>
#include <memory>


namespace alive {
    namespace sys_profile {


        class ProfileGroup : public Profile {

            friend class ProfileHandler;
            friend class ProfileUnit;

            public:


                ProfileGroup();
                ProfileGroup(const intercept::types::side side_, const intercept::types::vector3& pos_, const std::vector<std::string>& unitClasses_);
                ProfileGroup(const intercept::types::side side_, const intercept::types::vector3& pos_, const intercept::types::config& groupConfig_);

                virtual ~ProfileGroup();

                static ProfileGroup* Create(const intercept::types::side side_, const intercept::types::vector3& pos_, const std::vector<std::string>& unitClasses_);
                static ProfileGroup* Create(const intercept::types::side side_, const intercept::types::vector3& pos_, const intercept::types::config& groupConfig_);

                
                // getters


                intercept::types::side getSide()                            { return _side; }
                std::vector< std::shared_ptr<ProfileUnit> >& getUnits()     { return _units; }
                std::vector< ProfileWaypoint >& getWaypoints()              { return _waypoints; }

                // setters

                // functional

                
                virtual void update(const float dt_) override;

                virtual void spawn() override;
                virtual void despawn() override;

                virtual void addUnit(ProfileUnit* unit_);
                virtual void removeUnit(ProfileUnit* unit_);
                virtual void removeUnit(const std::string& unitID_);

                virtual int addWaypoint(ProfileWaypoint& wp_);
                virtual void removeWaypoint(unsigned int index_);
                virtual void removeWaypoint(const std::vector<ProfileWaypoint>::iterator& toDelete_);


            protected:


                int _cycleWaypointCount = 0;
                int _nextUnitID = 0;

                intercept::types::side                      _side;
                std::vector< std::shared_ptr<ProfileUnit> > _units;
                std::vector< ProfileWaypoint >              _waypoints;
                std::vector< ProfileWaypoint >              _waypointsCompleted;
                intercept::types::group                     _groupObject;


                // functions


                std::string _generateNextUnitID();

                virtual void _calculateSpeed() override;
                virtual void _createDebugMarker() override;

                virtual void _updateMovement(const float dt_);

        };


    }
}