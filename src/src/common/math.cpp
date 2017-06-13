#include "math.hpp"

#include "intercept.hpp"

#include <cmath>


namespace alive {
    namespace common {
        namespace math {


            const double PI = 3.1415926535897;

            intercept::types::vector3 lerp(const intercept::types::vector3& start_, const intercept::types::vector3& end_, float percent_) {
                return start_ + vectorMultiply(end_ - start_, percent_);
            }


        }
    }
}