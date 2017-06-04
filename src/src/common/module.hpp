#pragma once

#include "intercept.hpp"


namespace alive {
    namespace common {

        class Module {


            public:


                Module() {}
                Module(const Module& copy_) = delete;
                virtual ~Module() {}

                virtual void pause() { _paused = true; }
                virtual void unpause() { _paused = false; }

                virtual void onSimulationStep(const float dt_) {}
                virtual void onPreStart() {}
                virtual void onPreInit() {}
                virtual void onPostInit() {}
                virtual void onMissionStopped() {}
                virtual void onUnitKilled(intercept::types::object& killed_, intercept::types::object& killer_) {}


            private:


                bool _paused = false;


        };


    }
}