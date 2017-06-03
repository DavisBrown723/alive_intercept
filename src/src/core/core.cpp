#include "core.hpp"
#include "intercept.hpp"

#include "sys_profile\profile_system.hpp"
#include "common\include.hpp"

#include <chrono>


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

        // reset frame time
        // to avoid large initial
        // simulation jumps

        _lastFrameTime = std::chrono::system_clock::now();
    }

    void Core::onPreInit() {
        for (auto& module : _modules)
            module->onPreInit();

        _missionRunning = true;
    }

    void Core::onPostInit() {
        for (auto& module : _modules)
            module->onPostInit();
    }

    void Core::onMissionStopped() {
        for (auto& module : _modules)
            module->onMissionStopped();

        _missionRunning = false;
    }

    void Core::onSimulationStep() {
        // calculate elapsed time since last simulation step
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = now - _lastFrameTime;

        _lastFrameTime = now;

        float dt = diff.count();

        for (auto& module : _modules)
            module->onSimulationStep(dt);
    }

}