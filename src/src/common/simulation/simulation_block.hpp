#pragma once


namespace alive {
    namespace common {
        namespace simulation {


            template<class T>
            class SimulationBlock {


                template<class U>
                friend class SimulationBlockManager;


                public:


                    using ValueType = T*;
                    using ItemVector = std::vector<ValueType>;

                    SimulationBlock()
                        :
                        _lastSimulationTime(std::chrono::system_clock::now())
                    {}

                    ~SimulationBlock() {}

                    const ItemVector& getItems() { return _items; }

                    float getTimeSinceSimulation() {
                        return std::chrono::duration<float>(std::chrono::system_clock::now() - _lastSimulationTime).count();
                    }

                    void resetLastSimulationTime() { _lastSimulationTime = std::chrono::system_clock::now(); }


                protected:


                    ItemVector _items;
                    std::chrono::time_point<std::chrono::system_clock> _lastSimulationTime;


            };


        }
    }
}