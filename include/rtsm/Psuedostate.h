//
// Created by gwillen on 6/26/18.
//

#ifndef RTSM_PSUEDOSTATE_H
#define RTSM_PSUEDOSTATE_H

#include "Transition.h"

namespace rtsm {

    template<class KIND>
    struct Psuedostate : uml::Psuedostate {
        typedef KIND kind;
    };

    typedef Psuedostate<uml::PsuedostateKind::initial> Initial;
    template<class STATE>
    struct initial : Initial {
        typedef transition<initial, STATE> outgoing;
    };




    typedef Psuedostate<uml::PsuedostateKind::fork> Fork;
    typedef Psuedostate<uml::PsuedostateKind::choice> Choice;

    template<class TRANSITION, class ...TRANSITIONS>
    struct choice : Choice {
        using outgoing = collection<TRANSITION, TRANSITIONS...>;
    };
    typedef Psuedostate<uml::PsuedostateKind::shallowHistory> ShallowHistory;

    template<class CLASSIFIER, class CONTAINER>
    struct Object<uml::Psuedostate::type, uml::PsuedostateKind::initial, CLASSIFIER, CONTAINER>
            : Object<uml::Vertex::type, CLASSIFIER, CONTAINER> {

        typedef Object<uml::Vertex::type, CLASSIFIER, CONTAINER> BaseObject;
        using BaseObject::Object;

        static_assert(BaseObject::Outgoing::size() == 1, "Initial vertex requires one outgoing transition");

        template<class ...ARGS>
        void enter(typename CONTAINER::StateMachineObject &object, ARGS &&...args) {
            this->outgoing.execute(object, *this, std::forward<ARGS>(args)...);
        }
    };

    template<class CLASSIFIER, class CONTAINER>
    struct Object<uml::Psuedostate::type, uml::PsuedostateKind::fork, CLASSIFIER, CONTAINER>
            : Object<uml::Vertex::type, CLASSIFIER, CONTAINER> {

        using Object<uml::Vertex::type, CLASSIFIER, CONTAINER>::Object;

        template<class ...ARGS>
        void enter(typename CONTAINER::StateMachineObject &object, ARGS &&...args) {
            this->outgoing.execute(object, *this, std::forward<ARGS>(args)...);
        }
    };

    template<class CLASSIFIER, class CONTAINER>
    struct Object<uml::Psuedostate::type, uml::PsuedostateKind::choice, CLASSIFIER, CONTAINER>
            : Object<uml::Vertex::type, CLASSIFIER, CONTAINER> {

        using Object<uml::Vertex::type, CLASSIFIER, CONTAINER>::Object;

        template<class ...ARGS>
        void enter(typename CONTAINER::StateMachineObject &object, ARGS &&...args) {
            this->outgoing.execute(object, *this, std::forward<ARGS>(args)...);
        }
    };

    template<class CLASSIFIER, class CONTAINER>
    struct Object<uml::Psuedostate::type, uml::PsuedostateKind::shallowHistory, CLASSIFIER, CONTAINER>
            : Object<uml::Vertex::type, CLASSIFIER, CONTAINER> {

        using Object<uml::Vertex::type, CLASSIFIER, CONTAINER>::Object;

        template<class ...ARGS>
        void enter(typename CONTAINER::StateMachineObject &object, ARGS &&...args) {
            this->outgoing.execute(object, *this, std::forward<ARGS>(args)...);
        }
    };


    template<class CLASSIFIER, class CONTAINER>
    struct Object<uml::Psuedostate::type, CLASSIFIER, CONTAINER>
            : Object<uml::Psuedostate::type, typename CLASSIFIER::kind, CLASSIFIER, CONTAINER> {

        using Object<uml::Psuedostate::type, typename CLASSIFIER::kind, CLASSIFIER, CONTAINER>::Object;
    };

//    template<class ...ITEMS, class CONTAINER>
//    struct Object<uml::Psuedostate::type, uml::collection<ITEMS...>, CONTAINER>
//            : Object<uml::Vertex::type, uml::collection<ITEMS...>, CONTAINER> {
//        using Object<uml::Vertex::type, uml::collection<ITEMS...>, CONTAINER>::Object;
//    };

}

#endif //RTSM_PSUEDOSTATE_H
