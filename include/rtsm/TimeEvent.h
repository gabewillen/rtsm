//
// Created by gwillen on 7/13/18.
//

#ifndef RTSM_TIMEEVENT_H
#define RTSM_TIMEEVENT_H

#include "Object.h"
#include "utils.h"
#include <chrono>


namespace rtsm {

    typedef std::chrono::high_resolution_clock clock;


    template<class DURATION, typename DURATION::rep VALUE>
    struct duration : DURATION, uml::Duration {

        duration() : DURATION(VALUE) {}

        duration &operator =(typename DURATION::rep &&value) {
            *static_cast<DURATION *>(this) = DURATION(value);
            return *this;
        }

        constexpr explicit duration(typename DURATION::rep &&value) : DURATION(value || VALUE) {}

        static const typename DURATION::rep value = VALUE;
    };

    namespace {
//
        template<class ...>
        struct duration_add;

        template<template<class DURATION, typename DURATION::rep> class UNIT1, class DURATION1, typename DURATION1::rep VALUE1>
        struct duration_add<UNIT1<DURATION1, VALUE1>> {
            typedef duration<DURATION1, VALUE1> type;
        };

        template<template<class DURATION, typename DURATION::rep> class UNIT1, class DURATION1, typename DURATION1::rep VALUE1,
                template<class DURATION, typename DURATION::rep> class UNIT2, class DURATION2, typename DURATION2::rep VALUE2>
        struct duration_add<UNIT1<DURATION1, VALUE1>, UNIT2<DURATION2, VALUE2>> {
            typedef duration<DURATION2, VALUE1 * DURATION2::period::num * DURATION2::period::den + VALUE2> type;
        };


        template<template<class DURATION, typename DURATION::rep> class UNIT1, class DURATION1, typename DURATION1::rep VALUE1,
                template<class DURATION, typename DURATION::rep> class UNIT2, class DURATION2, typename DURATION2::rep VALUE2,
                template<class DURATION, typename DURATION::rep> class UNIT3, class DURATION3, typename DURATION2::rep VALUE3, class ...UNITS>
        struct duration_add<UNIT1<DURATION1, VALUE1>, UNIT2<DURATION2, VALUE2>, UNIT3<DURATION3, VALUE3>, UNITS...>
                : duration_add<typename duration_add<UNIT1<DURATION1, VALUE1>, UNIT2<DURATION2, VALUE2>>::type, typename duration_add<UNIT3<DURATION3, VALUE3>, UNITS...>::type>{

                };
    }


    template<std::chrono::hours::rep VALUE>
    using hours = duration<std::chrono::hours, VALUE>;

    template<std::chrono::minutes::rep VALUE>
    using minutes = duration<std::chrono::minutes, VALUE>;

    template<std::chrono::seconds::rep VALUE>
    using seconds = duration<std::chrono::seconds, VALUE>;

    template<std::chrono::microseconds::rep VALUE>
    using microseconds = duration<std::chrono::microseconds, VALUE>;

    template<std::chrono::milliseconds::rep VALUE>
    using milliseconds = duration<std::chrono::milliseconds, VALUE>;

    template<std::chrono::hours::rep HOURS, std::chrono::minutes::rep MINUTES, std::chrono::seconds::rep SECONDS>
    using time_of_day = duration<std::chrono::seconds, (HOURS * 3600) + (MINUTES * 60) + SECONDS>;

    template<class ...>
    struct TimeEvent;


    template<template<class DURATION, typename DURATION::rep> class UNIT, class DURATION, typename DURATION::rep VALUE>
    struct TimeEvent<UNIT<DURATION, VALUE>> : uml::TimeEvent {

        typedef UNIT<DURATION, VALUE> when;

        clock::duration elapsed = clock::duration::zero();
//    protected:
//
//        template<class _DURATION>
//        TimeEvent(_DURATION &&value) : DURATION(DURATION(VALUE) + value) {}


    };

    template<template<class DURATION, typename DURATION::rep> class UNIT1, class DURATION1, typename DURATION1::rep VALUE1,
            template<class DURATION, typename DURATION::rep> class UNIT2, class DURATION2, typename DURATION2::rep VALUE2, class ...UNITS>
    struct TimeEvent<UNIT1<DURATION1, VALUE1>, UNIT2<DURATION2, VALUE2>, UNITS...> : uml::TimeEvent {



        typedef typename duration_add<UNIT1<DURATION1, VALUE1>, UNIT2<DURATION2, VALUE2>, UNITS...>::type when;
        clock::duration elapsed;

    };

    template<class DURATION, typename DURATION::rep VALUE>
    struct Object<uml::Duration::type, duration<DURATION, VALUE>> : Object<uml::Element::type, duration<DURATION, VALUE>> {

        typedef Object<uml::Element::type, duration<DURATION, VALUE>> BaseObject;

        Object() : BaseObject(VALUE) {}
    };

    template<class CLASSIFIER, class CONTEXT>
    struct Object<uml::TimeEvent::type, CLASSIFIER, CONTEXT> : Object<uml::Event::type, CLASSIFIER, CONTEXT> {

        typedef Object<uml::Event::type, CLASSIFIER, CONTEXT> BaseObject;
        typedef Object<uml::Duration::type, typename CLASSIFIER::when> WhenObject;

        template<class SOURCE=typename BaseObject::SourceObject>
        Object(CONTEXT &object): BaseObject(object), start() {}


        bool active() {
            if (this->transition.source.active()) {
                if (!BaseObject::active()) {
                    BaseObject::active(true);
                    reset();
                } else {
                    this->elapsed = clock::now() - start;
                    if (this->elapsed >= std::chrono::duration_cast<clock::duration>(when)) {
                        return true;
                    }
                }
            } else {
                BaseObject::active(false);
            }
            return false;
        }

        using BaseObject::active;


    private:
        WhenObject when;
        clock::time_point start;

        inline void reset() {
            this->elapsed = clock::duration(0);
            start = clock::now();
        }

    };


}

#endif //RTSM_TIMEEVENT_H
