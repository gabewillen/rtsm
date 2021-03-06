//
// Created by gwillen on 8/28/18.
//

#ifndef RTSM_ELEMENT_H
#define RTSM_ELEMENT_H

#include "Object.h"
#if !(__arm__ || NON_CONCURRENT_REGIONS)
#include <atomic>
#endif

namespace rtsm {


    struct Element : uml::Element {


    };

    template<>
    struct Object<uml::Element::type, void> {

//        typedef void classifier;

        template<class TARGET>
        void get() {}

        bool active(bool value = false) { return false; }
    };



    template<class ELEMENT>
    struct Object<uml::Element::type, ELEMENT> : ELEMENT {


        template<class ...ARGS, class _ELEMENT=ELEMENT, typename std::enable_if<std::is_default_constructible<_ELEMENT>::value, bool>::type= 0>
        Object(ARGS &&...args) : _active(0) {}

        template<class ...ARGS, class _ELEMENT=ELEMENT, typename std::enable_if<!std::is_default_constructible<_ELEMENT>::value, bool>::type= 0>
        Object(ARGS &&...args) : ELEMENT(std::forward<ARGS>(args)...), _active(0) {}

        template<class TARGET,
                bool IS_SAME = std::is_same<ELEMENT, TARGET>::value,
                typename std::enable_if<IS_SAME, bool>::type= 0>
        inline ELEMENT &get() {
            return *this;
        }

        template<class TARGET,
                bool IS_SAME = std::is_same<ELEMENT, TARGET>::value,
                typename std::enable_if<!IS_SAME, bool>::type= 0>
        inline void get() {
        }

        bool active() {
            return _active > 0;
        }

        void active(bool value) {
            if(value) {
                _active++;
            } else if(_active) {
                _active--;
            }
//            _active = static_cast<unsigned int>(value);
        }

        const unsigned int &refs() {
            return _active;
        }
    private:

    #if !(__arm__ || NON_CONCURRENT_REGIONS)
        std::atomic<unsigned int> _active;
    #else
        unsigned int _active;
    #endif
    };




}

#endif //RTSM_ELEMENT_H
