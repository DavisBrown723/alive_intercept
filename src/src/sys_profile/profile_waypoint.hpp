#pragma once

#include "intercept.hpp"


namespace alive {
    namespace sys_profile {


        struct ProfileWaypoint {

            enum class Speed { LIMITED, NORMAL, FULL };
            enum class Formation { FILE, COLUMN, STAG_COLUMN, WEDGE, ECH_LEFT, ECH_RIGHT, VEE, LINE, DIAMOND };

            ProfileWaypoint(
                const intercept::types::vector3& pos_,
                const int completionRadius_ = 50,
                const Speed speed_ = Speed::NORMAL,
                const Formation formation_ = Formation::WEDGE
            )
                : position(pos_), completionRadius(completionRadius_), speed(speed_), formation(formation_)
            {

            }
                

            intercept::types::vector3 position;
            int completionRadius = 50;
            Speed speed;
            Formation formation;
        };


    }
}