#include "profile_waypoint.hpp"

#include "intercept.hpp"

#include "profile_group.hpp"


namespace alive {
    namespace sys_profile {


        ProfileWaypoint::ProfileWaypoint(
            const intercept::types::vector3&    pos_,
            ProfileWaypoint::Type               type_,
            int                                 completionRadius_,
            ProfileWaypoint::Speed              speed_,
            ProfileWaypoint::Formation          formation_,
            ProfileWaypoint::Behavior           behavior_,
            ProfileWaypoint::CombatMode         combatMode_,
            const std::string&                  statements_
        )
            :
            position(pos_),
            type(type_),
            completionRadius(completionRadius_),
            speed(speed_),
            formation(formation_),
            behavior(behavior_),
            combatMode(combatMode_)
            //statements(statements_)
        {

        }

        ProfileWaypoint::ProfileWaypoint(RVWaypoint& wp_) {
            position = intercept::sqf::waypoint_position(wp_);
            type = intercept::sqf::waypoint_type(wp_);
            completionRadius = static_cast<int>(intercept::sqf::waypoint_completeion_radius(wp_));
            speed = intercept::sqf::waypoint_speed(wp_);
            formation = intercept::sqf::waypoint_formation(wp_);
            behavior = intercept::sqf::waypoint_behaviour(wp_);
            combatMode = intercept::sqf::waypoint_combat_mode(wp_);
            //statements = intercept::sqf::waypoint_statements; // non-existant wrapper
        }

        ProfileWaypoint::~ProfileWaypoint() {

        }

        ProfileWaypoint::RVWaypoint ProfileWaypoint::toRVWaypoint(ProfileGroup* profile_, intercept::types::group& group_) {
            ProfileWaypoint::RVWaypoint wp = intercept::sqf::add_waypoint(group_, position, 0.f);
            
            intercept::sqf::set_waypoint_completion_radius(wp, static_cast<float>(completionRadius));
            intercept::sqf::set_waypoint_type(wp, type);
            intercept::sqf::set_waypoint_speed(wp, speed);
            intercept::sqf::set_waypoint_formation(wp, formation);
            intercept::sqf::set_waypoint_behaviour(wp, behavior);
            intercept::sqf::set_waypoint_combat_mode(wp, combatMode);
            
            auto startIndex = statements.find("profileWaypointComplete");
            if (startIndex != std::string::npos) {
                statements.erase(startIndex, statements.find(';', startIndex));
            }

            intercept::sqf::set_waypoint_statements(wp, "", statements + ("profileWaypointComplete " + profile_->getID() + ";"));
            
            return wp;
        }


    }
}