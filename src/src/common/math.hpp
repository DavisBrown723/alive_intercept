#pragma once

#include "intercept.hpp"


namespace alive {
    namespace common {
        namespace math {

            extern const double PI;

            template<typename T>
            inline T toRadians(T deg_) { return deg_ * (PI / 180); }

            template<typename T>
            inline T toDegrees(T rad_) { return rad_ * (180 / PI); }

            inline int normalizeDegrees(int deg_) { return (deg_ + 360) % 360; }

            int getRelDir(const intercept::types::vector3& pos1_, const intercept::types::vector3& pos2_);

            intercept::types::vector3 getRelPos(const intercept::types::vector3& pos_, const float dir_, const float _dist);


        }
    }
}