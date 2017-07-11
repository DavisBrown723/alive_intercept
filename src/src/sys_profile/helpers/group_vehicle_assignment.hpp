#pragma once

#include <vector>


namespace alive {
    namespace sys_profile {


        class ProfileGroup;
        class ProfileVehicle;
        class ProfileUnit;


        class GroupVehicleAssignment {

            public:

                ProfileGroup* group;
                ProfileVehicle* vehicle;
                std::vector<ProfileUnit*> units;

                GroupVehicleAssignment() : group(nullptr), vehicle(nullptr) {}
                GroupVehicleAssignment(ProfileGroup* group_, ProfileVehicle* vehicle_);
                ~GroupVehicleAssignment();

                void removeUnit(ProfileUnit* unit_);

                bool operator==(const GroupVehicleAssignment& veh_) const;

        };


    }
}