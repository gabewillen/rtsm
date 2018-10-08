//
// Created by gwillen on 6/28/18.
//

#ifndef RTSM_EVENT_H
#define RTSM_EVENT_H

#include "Collection.h"
//#include "eid.h"

namespace rtsm {


    struct Event : uml::Event {


    };

//    template<int ID>
//    struct UIDEvent : Event {
//        static const int id = ID;
//    };

    template<int ID, class EVENT=Event>
    struct event : EVENT {
        static const int id = ID;
    };


    template<class CONTEXT>
    struct Object<uml::Event::type, void, CONTEXT> : Object<uml::Element::type, void> {
        Object(CONTEXT &) {}
    };


    template<class CLASSIFIER, class TYPE, class CONTEXT, class ...REST>
    struct Object<uml::Event::type, CLASSIFIER, Object<TYPE, CONTEXT, REST...>> : Object<uml::Element::type, CLASSIFIER> {

        typedef Object<TYPE, CONTEXT, REST...> ContextObject;

        template<class ...>
        struct is_same;

        template<class TRANSITION, class TRIGGER>
        struct is_same<TRANSITION, TRIGGER> : std::is_same<typename TRIGGER::event, CLASSIFIER> {
        };

        template<class TRANSITION, class ...TRIGGERS>
        struct is_same<TRANSITION, uml::collection<TRIGGERS...>> {

            template<class TRIGGER>
            struct _is_same : std::is_same<typename TRIGGER::event, CLASSIFIER> {
            };
            typedef typename uml::Collection::select<uml::collection<TRIGGERS...>, _is_same>::type events;

            static const bool value = events::size() == 1;
        };

        template<class TRANSITION>
        struct is_same<TRANSITION, void> : std::false_type {
        };

        template<class TRANSITION>
        struct is_same<TRANSITION> : is_same<TRANSITION, typename TRANSITION::trigger> {
        };

        typedef typename uml::Collection::classifier<typename uml::Collection::select<typename uml::Transition::all<CONTEXT>::type, is_same>::type, 0>::type Transition;

        typedef typename declobject<ContextObject, typename Transition::source>::type SourceObject;
        typedef typename declobject<ContextObject, typename Transition::target>::type TargetObject;

        typedef Object<uml::Transition::type, Transition, SourceObject> TransitionObject;

//        Object() {}
        Object(ContextObject &object) : transition(static_cast<TransitionObject &>(object.template get<Transition>())) {}

        TransitionObject &transition;
    };


    template<class TYPE, class CONTEXT, class ...REST>
    struct Object<uml::Event::type, uml::collection<>, Object<TYPE, CONTEXT, REST...>>
            : Object<uml::Collection::type, uml::collection<>, Object<TYPE, CONTEXT, REST...>> {
        Object(Object<TYPE, CONTEXT, REST...> &) {}
    };

    template<class CLASSIFIER, class TYPE, class CONTEXT, class ...REST>
    struct Object<uml::Event::type, uml::collection<CLASSIFIER>, Object<TYPE, CONTEXT, REST...>> : Object<uml::Collection::type,
            uml::collection<CLASSIFIER>, Object<TYPE, CONTEXT, REST...>> {

        using Object<uml::Collection::type,
                uml::collection<CLASSIFIER>, Object<TYPE, CONTEXT, REST...>>::Object;


    };

    template<class CLASSIFIER, class ...CLASSIFIERS, class TYPE, class CONTEXT, class ...REST>
    struct Object<uml::Event::type, uml::collection<CLASSIFIER, CLASSIFIERS...>, Object<TYPE, CONTEXT, REST...>>
            : Object<uml::Collection::type,
                    uml::collection<CLASSIFIER, CLASSIFIERS...>, Object<TYPE, CONTEXT, REST...>> {
        using Object<uml::Collection::type,
                uml::collection<CLASSIFIER, CLASSIFIERS...>, Object<TYPE, CONTEXT, REST...>>::Object;
    };
}

#endif //RTSM_EVENT_H
