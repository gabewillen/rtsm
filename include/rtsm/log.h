//
// Created by gwillen on 10/1/18.
//

#ifndef RTSM_LOG_H
#define RTSM_LOG_H

#include <iostream>
#include <cxxabi.h>
#include <memory>

namespace rtsm {

    namespace log {



        std::string demangle(const char* name) {

            int status = -4; // some arbitrary value to eliminate the compiler warning

            // enable c++11 by passing the flag -std=c++11 to g++
            std::unique_ptr<char, void(*)(void*)> res {
                    abi::__cxa_demangle(name, NULL, NULL, &status),
                    std::free
            };

            return (status==0) ? res.get() : name ;
        }


        template<class STATE>
        struct entry: rtsm::Behavior {
            template<class CONTEXT>
            static void execute(CONTEXT &) {
                std::cout << demangle(typeid(STATE).name()) << "::entry" << std::endl;
            }
        };

        template<class STATE>
        struct exit : rtsm::Behavior {
            template<class CONTEXT>
            static void execute(CONTEXT &) {
                std::cout << demangle(typeid(STATE).name()) << "::exit" << std::endl;
            }
        };

        template<class TRANSITION>
        struct effect : rtsm::Behavior {
            template<class CONTEXT, class ...ARGS>
            static void execute(CONTEXT &, ARGS  &&...args) {
                std::cout << demangle(typeid(TRANSITION).name()) << "::effect" << std::endl;


            }
        };


        template<class TRANSITION>
        struct guard : rtsm::Constraint {
            template<class CONTEXT, class ...ARGS>
            static bool evaluate(CONTEXT &, ARGS  &&...args) {
                std::cout << demangle(typeid(TRANSITION).name()) << "::guard" << std::endl;
                return true;
            }
        };
    }

}


#endif //RTSM_LOG_H
