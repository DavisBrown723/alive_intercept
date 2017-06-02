#pragma once

#include "intercept.hpp"

#include "profile_unit.hpp"
#include "profile_waypoint.hpp"

#include "common\include.hpp"

#include <vector>
#include <string>
#include <memory>


namespace alive {
    namespace sys_profile {


        class ProfileGroup {

            friend class ProfileUnit;


            friend class ProfileHandler;


            public:


                ProfileGroup();
                ProfileGroup(const intercept::types::side side_, const intercept::types::vector3& pos_, const std::vector<std::string>& unitClasses_);
                ProfileGroup(const intercept::types::side side_, const intercept::types::vector3& pos_, const intercept::types::config& groupConfig_);

                virtual ~ProfileGroup();

                static ProfileGroup* Create(const intercept::types::side side_, const intercept::types::vector3& pos_, const std::vector<std::string>& unitClasses_);
                static ProfileGroup* Create(const intercept::types::side side_, const intercept::types::vector3& pos_, const intercept::types::config& groupConfig_);

                
                // getters / setters


                const std::string& getID()                                  { return _id; }
                intercept::types::side getSide()                            { return _side; }
                bool isActive()                                             { return _active; }
                bool debugEnabled()                                         { return _debugEnabled; }
                int getSpeed()                                              { return _speed; }
                intercept::types::vector3& getPosition()                    { return _pos; }
                std::vector< std::shared_ptr<ProfileUnit> >& getUnits()     { return _units; }
                std::vector< ProfileWaypoint >& getWaypoints()              { return _waypoints; }
                
                virtual void setPosition(intercept::types::vector3 newPos_) { _pos = newPos_; }
                void enableDebug(bool enabled_);


                // functional

                
                virtual void updateOnSimulationStep();

                virtual void spawn();
                virtual void despawn();

                virtual void addUnit(ProfileUnit* unit_);
                virtual void removeUnit(const std::string& unitID_);


            protected:

                std::string _id;
                bool _active;
                bool _debugEnabled = false;
                int _speed;

                intercept::types::side _side;

                intercept::types::vector3 _pos;

                std::vector< std::shared_ptr<ProfileUnit> > _units;
                std::vector< ProfileWaypoint > _waypoints;

                intercept::types::group _groupObject;

                intercept::types::marker _debugMarker;

                int _nextUnitID = 0;


                // functions


                virtual void _calculateSpeed();
                virtual void _createDebugMarker();

                std::string _generateNextUnitID();


        };


    }
}