#include "core.hpp"
#include "intercept.hpp"

#include "sys_profile\profile_system.hpp"
#include "common\include.hpp"


namespace alive {

    namespace sqf_commands {
        namespace handles {
            intercept::types::registered_sqf_function _initALiVE;
        }

        game_value initALiVE() {
            // cache SQF values
            // initialize ALiVE functions

            Core::get().initializeALiVE();

            return game_value();
        }
    }

    Core::Core() {

    }

    Core::~Core() {

    }

    void Core::registerScriptCommands() {
        sqf_commands::handles::_initALiVE = intercept::client::host::functions.register_sqf_function_nular(
            "initALiVE",
            "Initializes ALiVE base systems.",
            userFunctionWrapper<sqf_commands::initALiVE>,
            intercept::types::__internal::GameDataType::NOTHING
        );
    }

    void Core::initializeALiVE() {
        common::RV::get().initialize();
    }

    std::vector< std::shared_ptr<alive::common::Module> >& Core::getModules() {
        return _modules;
    }

    void Core::registerModule(alive::common::Module* module_) {
        _modules.push_back(std::shared_ptr<alive::common::Module>(module_));
    }

    void Core::onPreStart() {
        // register script commands
        // must be done on pre start

        Core::registerScriptCommands();
        sys_profile::ProfileSystem::registerScriptCommands();

        for (auto& module : _modules)
            module->onPreStart();
    }

    void Core::onPreInit() {
        for (auto& module : _modules)
            module->onPreInit();
    }

    void Core::onPostInit() {
        for (auto& module : _modules)
            module->onPostInit();
    }

    void Core::onSimulationStep() {
        for (auto& module : _modules)
            module->onSimulationStep();
    }

}