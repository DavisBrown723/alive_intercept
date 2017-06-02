#pragma once

#include "Event.hpp"

#include <list>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <functional>


namespace alive {
    namespace events {


        class Listener;


        class EventBus {


            public:


                template<class T>
                using EventHandler = std::function<void(T)>;

                EventBus() { }

                virtual ~EventBus() { }

                static EventBus* const get() {
                    if (_instance == nullptr)
                        _instance = new EventBus();

                    return _instance;
                }

                template<class T>
                static void registerListener(Listener* const listener_) {
                    EventBus* instance = EventBus::get();

                    // create listener list if no listeners
                    // have been registered for given type yet

                    if (instance->_listeners.count(typeid(T)) == 0)
                        instance->_listeners[typeid(T)] = std::vector<Listener*>();

                    instance->_listeners[typeid(T)].push_back(listener_);
                }

                template<class T>
                static void unregisterListener(Listener* const listener_) {
                    unregisterListener(typeid(T), listener_);
                }

                static void unregisterListener(std::type_index&& type_, Listener* listener_) {
                    EventBus* instance = EventBus::get();

                    if (instance->_listeners.count(type_) == 1) {
                        std::vector<Listener*> listeners = instance->_listeners[type_];

                        for (auto i = listeners.begin(); i != listeners.end(); ) {
                            if (*i == listener_)
                                i = listeners.erase(i);
                            else
                                i++;
                        }

                        if (listeners.empty())
                            instance->_listeners.erase(type_);
                    }
                }

                template<class T>
                static void registerEvent(T& event) {
                    EventBus* instance = EventBus::get();

                    if (instance->_listeners.count(typeid(T)) == 1) {
                        std::vector<Listener*> listeners = instance->_listeners[typeid(T)];

                        for (auto&& listener : listeners) {
                            (*(listener->getEventHandler<T>()))(event);
                        }
                    }
                }


            private:


                typedef std::unordered_map<std::type_index, std::vector<Listener*> > ListenerTypeMap;


                static EventBus* _instance;

                ListenerTypeMap _listeners;


        };


    }
}