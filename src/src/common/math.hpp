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

            intercept::types::vector3 lerp(const intercept::types::vector3& start_, const intercept::types::vector3& end_, float percent_);

            inline float distance(const intercept::types::vector3& v1_, const intercept::types::vector3& v2_) {
                return std::sqrt(
                    (v2_.x - v1_.x) * (v2_.x - v1_.x) +
                    (v2_.y - v1_.y) * (v2_.y - v1_.y) +
                    (v2_.z - v1_.z) * (v2_.z - v1_.z)
                );
            }

            inline float distanceSqr(const intercept::types::vector3& v1_, const intercept::types::vector3& v2_) {
                return (
                    (v2_.x - v1_.x) * (v2_.x - v1_.x) +
                    (v2_.y - v1_.y) * (v2_.y - v1_.y) +
                    (v2_.z - v1_.z) * (v2_.z - v1_.z)
                );
            }

            inline float distance2D(const intercept::types::vector3& v1_, const intercept::types::vector3& v2_) {
                return std::sqrt(
                    (v2_.x - v1_.x) * (v2_.x - v1_.x) +
                    (v2_.y - v1_.y) * (v2_.y - v1_.y)
                );
            }

            inline float distance2DSqr(const intercept::types::vector3& v1_, const intercept::types::vector3& v2_) {
                return (
                    (v2_.x - v1_.x) * (v2_.x - v1_.x) +
                    (v2_.y - v1_.y) * (v2_.y - v1_.y)
                );
            }






            // intercept functionality extenders

            inline intercept::types::vector3 vectorAdd(const intercept::types::vector3& vec_, float toAdd_) { return { vec_.x + toAdd_ , vec_.y + toAdd_ , vec_.z + toAdd_ }; }

            inline intercept::types::vector3 vectorSubtract(const intercept::types::vector3& vec_, float toSub_) { return { vec_.x - toSub_ , vec_.y - toSub_ , vec_.z - toSub_ }; }

            inline intercept::types::vector3 vectorMultiply(const intercept::types::vector3& vec_, float toMult_) { return { vec_.x * toMult_ , vec_.y * toMult_ , vec_.z * toMult_ }; }

            inline intercept::types::vector3 vectorDivide(const intercept::types::vector3& vec_, float toDiv_) { return { vec_.x / toDiv_ , vec_.y / toDiv_ , vec_.z / toDiv_ }; }


        }
    }
}