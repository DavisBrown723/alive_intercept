#include "world.hpp"

#include "math.hpp"

#include "intercept.hpp"


namespace alive {
    namespace common {
        namespace world {


            int getRelDir(const intercept::types::vector3& pos1_, const intercept::types::vector3& pos2_) {
                intercept::types::vector3 posDiff{ pos2_ - pos1_ };

                float deg = math::toDegrees(std::atan2(posDiff.y, posDiff.x));
                deg += 90 - 180;
                deg = std::abs(deg);

                return deg;
            }

            intercept::types::vector3 getRelPos(const intercept::types::vector3& pos_, const float dir_, const float dist_) {
                return {
                    pos_.x + (dist_ * std::sin(math::toRadians(dir_))),
                    pos_.y + (dist_ * std::cos(math::toRadians(dir_))),
                    pos_.z
                };
            }


        }
    }
}