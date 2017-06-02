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

                virtual void onSimulationStep() {}
                virtual void onPreStart() {}
                virtual void onPreInit() {}
                virtual void onPostInit() {}


            private:


                bool _paused = false;


        };


    }
}