#pragma once

#include "singleton.hpp"

#include "intercept.hpp"


namespace alive {
    namespace common {

        /*
            Singleton class to store cached RV game values
        */

        class RV : public Singleton<RV> {


            public:


                void initialize();

                struct Side {
                    intercept::types::side East;
                    intercept::types::side West;
                    intercept::types::side Guer;
                    intercept::types::side Civ;
                } sides;


            private:


                bool _initialized = false;


        };


    }
}