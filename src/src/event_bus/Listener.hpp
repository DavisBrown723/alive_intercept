#pragma once

#include "Event.hpp"
#include "EventBus.hpp"

#include <functional>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>


namespace alive {
    namespace events {


        class Listener {


            public:


                Listener() { }

                Listener(const Listener& toCopy_) = delete;

                virtual ~Listener() {
                    // delete temporary function pointers
                    // created during connection

                    for (auto& pair : _eventHandlers)
                        delete pair.second;
                }

                template<class T>
                void connect(std::function<void(T)>& eventHandler_) {
                    // create function pointer we can keep alive
                    auto tempFunc = new std::function<void(T)>(eventHandler_);

                    bool existingHandler = false;

                    if (_eventHandlers.count(typeid(T)) == 1)
                        existingHandler = true;

                    if (existingHandler)
                        delete static_cast<std::function<void(T)>*>(_eventHandlers[typeid(T)]);

                    _eventHandlers[typeid(T)] = static_cast<void*>(tempFunc);

                    if (!existingHandler)
                        EventBus::registerListener<T>(this);
                }

                template<class T>
                void disconnect() {
                    disconnect(typeid(T));
                }

                bool disconnect(std::type_index&& type_) {
                    if (_eventHandlers.count(type_) == 1) {
                        _eventHandlers.erase(type_);

                        EventBus::unregisterListener(std::forward<std::type_index>(type_), this);

                        return true;
                    }

                    return false;
                }

                void disconnectAll() {
                    while (!_eventHandlers.empty())
                        disconnect(static_cast<std::type_index>(_eventHandlers.begin()->first));
                }

                template<class T>
                std::function<void(T)>* const getEventHandler() {
                    if (_eventHandlers.count(typeid(T)) == 1)
                        return static_cast<std::function<void(T)>*>(_eventHandlers[typeid(T)]);

                    return nullptr;
                }


            private:


                std::unordered_map<std::type_index, void*> _eventHandlers;


        };


    }
}