#pragma once


namespace alive {
    namespace events {


        class Event {


            public:


                Event() : handled(false) { };
                virtual ~Event() { }

                bool handled;


        };


    }
}