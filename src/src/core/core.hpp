#pragma once

#include "common\include.hpp"

#include <vector>
#include <memory>
#include <string>
#include <chrono>


namespace alive {


    class Core : public alive::common::Singleton<Core> {


        public:

        
            Core();
            ~Core();

            static void registerScriptCommands();

            void initializeALiVE();

            std::vector< std::shared_ptr<alive::common::Module> >& getModules();

            void registerModule(alive::common::Module* module_);

            bool isMissionRunning() { return _missionRunning; }

            void onPreStart();
            void onPreInit();
            void onPostInit();
            void onSimulationStep();
            void onMissionStopped();
            void onUnitKilled(intercept::types::object& killed_, intercept::types::object& killer_);


        private:


            bool _initialized = false;
            bool _missionRunning = false;

            std::chrono::time_point<std::chrono::system_clock> _lastFrameTime;

            std::vector< std::shared_ptr<alive::common::Module> > _modules;



    };


}