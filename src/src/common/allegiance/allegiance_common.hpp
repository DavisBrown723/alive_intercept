#pragma once

#include "intercept.hpp"

#include <string>


namespace alive {
    namespace common {
        namespace allegiance {


            intercept::types::side numberToSide(int num_);
            int sideToNumber(intercept::types::side side_);

            intercept::types::config getFactionConfig(const std::string& faction_);
            intercept::types::side getFactionSide(const std::string& faction_);

            std::string getSideColor(const intercept::types::side& side_);


        }
    }
}