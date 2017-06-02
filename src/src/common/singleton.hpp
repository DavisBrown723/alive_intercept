#pragma once

#include <assert.h>


namespace alive {
    namespace common {


        template<class T>
        class Singleton {


            public:


                static T& get() {
                    if (_instance == nullptr) {
                        assert(_initialized == false);

                        _initialized = true;
                        _instance = new T();
                    }

                    return *_instance;
                }

                static bool isInitialized() {
                    return _initialized;
                }

                static void release() {
                    delete _instance;
                }


            protected:

            
                static T* _instance;
                static bool _initialized;


        };

        template<typename T>
        T* Singleton<T>::_instance = nullptr;

        template<typename T>
        bool Singleton<T>::_initialized = false;


    }
}