#include "group_vehicle_assignment.hpp"

#include "sys_profile\profile_group.hpp"
#include "sys_profile\profile_vehicle.hpp"
#include "sys_profile\profile_unit.hpp"

#include <algorithm>


namespace alive {
    namespace sys_profile {


            GroupVehicleAssignment::GroupVehicleAssignment(ProfileGroup* group_, ProfileVehicle* vehicle_)
                :
                group(group_),
                vehicle(vehicle_)
            {
                // set vehicle assignment to vehicle

                vehicle_->_vehicleAssignment = this;
            }

            GroupVehicleAssignment::~GroupVehicleAssignment() {}

            void GroupVehicleAssignment::removeUnit(ProfileUnit* unit_) {
                auto it = std::find(units.begin(), units.end(), unit_);

                if (it != units.end())
                    units.erase(it);
            }


            bool GroupVehicleAssignment::operator==(const GroupVehicleAssignment& veh_) const { return veh_.vehicle == vehicle; }






            bool operator==(GroupVehicleAssignment& lhs_, GroupVehicleAssignment& rhs_) {
                return lhs_.operator==(rhs_);
            }
    }
}