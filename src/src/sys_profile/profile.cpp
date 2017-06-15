#include "profile.hpp"

#include "intercept.hpp"

#include <string>

using namespace intercept;


namespace alive {
    namespace sys_profile {


        Profile::Profile()
            :
            _id(""),
            _faction("BLU_F"),
            _debugEnabled(false),
            _active(false),
            _speed(0),
            _pos({0,0,0})
        {

        }

        Profile::Profile(const intercept::types::side& side_, const std::string& faction_, const intercept::types::vector3& pos_)
            :
            _id(""),
            _side(side_),
            _faction(faction_),
            _debugEnabled(false),
            _active(false),
            _speed(0),
            _pos(pos_)
        {

        }

        Profile::~Profile() {

        }

        
        // public


        void Profile::enableDebug(bool enabled_) {
            if (enabled_ == _debugEnabled)
                return;

            _debugEnabled = enabled_;

            if (_debugEnabled) {
                _createDebugMarker();
            } else {
                intercept::sqf::delete_marker(_debugMarker);
            }
        }


    }
}