#include "rv.hpp"


namespace alive {
    namespace common {


        void RV::initialize() {
            if (_initialized)
                return;

            // sides

            sides.East = intercept::sqf::east();
            sides.West = intercept::sqf::west();
            sides.Guer = intercept::sqf::independent();
            sides.Civ = intercept::sqf::civilian();

            _initialized = true;
        }


    }
}