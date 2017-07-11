#pragma once

#include <vector>


namespace alive {
    namespace common {
        namespace containers {


            template<typename T>
            void vectorAppend(std::vector<T>& a, const std::vector<T>& b) {
                a.insert(a.end(), b.begin(), b.end());
            }


        }
    }
}