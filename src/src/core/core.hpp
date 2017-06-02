#pragma once

#include "common\include.hpp"

#include <vector>
#include <memory>
#include <string>


namespace alive {


    class Core : public alive::common::Singleton<Core> {


        public:

        
            Core();
            ~Core();

            static void registerScriptCommands();

            void initializeALiVE();

            std::vector< std::shared_ptr<alive::common::Module> >& getModules();

            void registerModule(alive::common::Module* module_);

            void onPreStart();
            void onPreInit();
            void onPostInit();
            void onSimulationStep();


        private:


            bool _initialized = false;

            std::vector< std::shared_ptr<alive::common::Module> > _modules;



    };


}