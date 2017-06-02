#include "math.hpp"

#include "intercept.hpp"

#include <cmath>


namespace alive {
    namespace common {
        namespace math {

            const double PI = 3.1415926535897;

            int getRelDir(const intercept::types::vector3& pos1_, const intercept::types::vector3& pos2_) {
                intercept::types::vector3 posDiff{ pos2_ - pos1_ };

                float deg = toDegrees(std::atan2(posDiff.y, posDiff.x));
                deg += 90 - 180;
                deg = std::abs(deg);

                return deg;
            }

            intercept::types::vector3 getRelPos(const intercept::types::vector3& pos_, const float dir_, const float dist_) {
                return {
                    pos_.x + (dist_ * std::sin(toRadians(dir_))),
                    pos_.y + (dist_ * std::cos(toRadians(dir_))),
                    pos_.z
                };
            }


        }
    }
}