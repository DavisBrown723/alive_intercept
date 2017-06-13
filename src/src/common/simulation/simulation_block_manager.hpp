#pragma once

#include "simulation_block.hpp"


namespace alive {
    namespace common {
        namespace simulation {


            template<class T>
            class SimulationBlockManager {


                public:


                    using ValueType = T;

                    SimulationBlockManager()
                        :
                        _blockCapacity(50)
                    {}

                    SimulationBlockManager(int maxItemsPerBlock_)
                        :
                        _blockCapacity(maxItemsPerBlock_)
                    {}

                    void insertItem(T*& item_) {
                        // insert item into
                        // first open block
                        ///*
                        for (auto& block : _blocks) {
                            if (block._items.size() != _blockCapacity) {
                                block._items.push_back(item_);
                                return;
                            }
                        }
                        //*/
                        // all blocks full
                        // add new block

                        _blocks.push_back(SimulationBlock<ValueType>());
                        _blocks[_blocks.size() - 1]._items.push_back(item_);
                    }

                    SimulationBlock<ValueType>* getNextBlock() {
                        if (_blocks.size() == 0)
                            return nullptr;

                        if (_nextBlock == _blocks.size())
                            _nextBlock = 0;

                        // remove null entries
                        /*
                        _blocks[_nextBlock]._items.erase(
                            std::remove_if(
                                _blocks[_nextBlock]._items.begin(),
                                _blocks[_nextBlock]._items.end(),
                                [](ValueType*& item) { return item == nullptr; }
                            )
                        );
                        */

                        return &_blocks[_nextBlock++];
                    }


                private:


                    int _nextBlock = 0;
                    int _blockCapacity;
                    std::vector< SimulationBlock<T> > _blocks;


            };


        }
    }
}