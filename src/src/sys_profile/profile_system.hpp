#pragma once

#include "intercept.hpp"

#include "common\include.hpp"
#include "profile_handler.hpp"

#include <vector>


namespace alive {
    namespace sys_profile {

        
        class ProfileSystem : public alive::common::Singleton<ProfileSystem> , public alive::common::Module {

            public:


                ProfileSystem();
                ProfileSystem(int spawnDistance_);
                virtual ~ProfileSystem();


                // getters / setters


                ProfileHandler& getProfileHandler() { return _profileHandler; }

                int getSpawnDistance() { return _spawnDistance; }
                void setSpawnDistance(int distance_) { _spawnDistance = distance_; }


                // functional

                static void registerScriptCommands();

                virtual void onSimulationStep() override;

                bool debugEnabled() { return _debugEnabled; }

                void enableDebug(bool enabled_) { 
                    _debugEnabled = enabled_;

                    _profileHandler.enableDebug(enabled_);
                }


            private:

                
                bool _debugEnabled = false;

                ProfileHandler _profileHandler;

                int _spawnDistance;

                float _lastSimTime = 0.f;

                void _activateProfiles();
                void _simulateProfileMovement();


        };


    }
}