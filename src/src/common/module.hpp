#pragma once


namespace alive {
    namespace common {

        class Module {


            public:


                Module() {}
                Module(const Module& copy_) = delete;
                virtual ~Module() {}

                virtual void pause() { _paused = true; }
                virtual void unpause() { _paused = false; }

                virtual void onSimulationStep(float dt_) {}
                virtual void onPreStart() {}
                virtual void onPreInit() {}
                virtual void onPostInit() {}
                virtual void onMissionStopped() {}


            private:


                bool _paused = false;


        };


    }
}