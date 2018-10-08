//
// Created by gwillen on 6/20/18.
//

#ifndef RTSM_REGION_H
#define RTSM_REGION_H

#include "Collection.h"
#include "utils.h"
#include "State.h"
#if !defined(__arm__) || !defined(NON_CONCURRENT_REGIONS)
#include <future>
#endif

namespace rtsm {


    struct Region : uml::Region {

    };

    template<class TARGET>
    struct initial;

    template<class SUBMACHINE>
    struct submachineRegion : Region {

        typedef uml::Region::type type;
        struct SubmachineState : submachineState<SUBMACHINE> {
        };
        typedef collection<initial<SubmachineState>, SubmachineState> subvertex;

    };


    template<class ...CLASSIFIERS>
    struct region : Region {

        template<class CLASSIFIER>
        using is_vertex = std::is_base_of<uml::Vertex, CLASSIFIER>;

        template<class CLASSIFIER>
        using is_transition = std::is_base_of<uml::Transition, CLASSIFIER>;

        typedef typename uml::Collection::select<uml::collection<CLASSIFIERS...>, is_vertex>::type subvertex;
        typedef typename uml::Collection::select<uml::collection<CLASSIFIERS...>, is_transition>::type transition;
    };


    template<class STATE, class CONTAINER>
    struct Object<uml::Region::type, void, Object<uml::State::type, STATE, CONTAINER>>
            : Object<uml::Element::type, void> {
        typedef void SubvertexObject;
        typedef Object<uml::State::type, STATE, CONTAINER> StateObject;

        Object(StateObject &) {}

        template<class CONTEXT>
        void activate(CONTEXT &) {}

        template<class CONTEXT, class EVENT>
        bool process(CONTEXT &object, EVENT &event) {
            return false;
        }

    };

    template<class STATEMACHINE>
    struct Object<uml::Region::type, void, Object<uml::StateMachine::type, STATEMACHINE>>
            : Object<uml::Element::type, void> {
        typedef void SubvertexObject;
        typedef Object<uml::StateMachine::type, STATEMACHINE> StateMachineObject;

        Object(StateMachineObject &) {}

        template<class CONTEXT>
        void activate(CONTEXT &) {}

        template<class CONTEXT, class EVENT>
        bool process(CONTEXT &object, EVENT &event) {
            return false;
        }

    };




//    template<class ...REST>
//    struct Object<uml::Vertex::type, void, REST...> {};

    template<class CLASSIFIER, class STATE, class CONTAINER>
    struct Object<uml::Region::type, CLASSIFIER, Object<uml::State::type, STATE, CONTAINER>>
            : Object<uml::Element::type, CLASSIFIER> {

        typedef Object<uml::Element::type, CLASSIFIER> BaseObject;
        typedef Object<uml::State::type, STATE, CONTAINER> StateObject;
        typedef typename CONTAINER::StateMachine StateMachine;
        typedef typename CONTAINER::StateMachineObject StateMachineObject;
        typedef Object<uml::Vertex::type, typename CLASSIFIER::subvertex, Object> SubvertexObject;
        typedef typename uml::Collection::select<typename CLASSIFIER::subvertex, partial<uml::Classifier::is_kind, uml::PsuedostateKind::initial>::type>::type InitialVertices;
        typedef Object<uml::Psuedostate::type, typename uml::Collection::classifier<InitialVertices, 0>::type, Object> InitialVertexObject;

        Object(StateObject &object) : BaseObject(object.containingStateMachine()), state(object), subvertex(*this) {

        }

        template<int INDEX = uml::Collection::index_of<typename CLASSIFIER::subvertex, typename uml::Collection::classifier<InitialVertices, 0>::type>::value, bool HAS_INITIAL = InitialVertices::size(), typename std::enable_if<HAS_INITIAL, bool>::type= 0>
        void activate(StateMachineObject &object) {
            this->active(true);
            this->subvertex.template get<INDEX>().enter(object);
        }

        template<bool HAS_INITIAL = InitialVertices::size(), typename std::enable_if<!HAS_INITIAL, bool>::type= 0>
        void activate(StateMachineObject &object) {
            this->active(true);
        }


        StateMachineObject &containingStateMachine() {
            return state.containingStateMachine();
        }

        template<class EVENT>
        bool process(StateMachineObject &object, EVENT &event) {
            return this->active() && subvertex.process(object, event);
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class SUBVERTEX_TARGET=decltype(std::declval<SubvertexObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                bool IS_SUBVERTEX_TARGET = std::is_same<SUBVERTEX_TARGET, TARGET &>::value,
                typename std::enable_if<IS_BASE_TARGET, bool>::type= 0
        >
        BASE_TARGET get() {
            return BaseObject::template get<TARGET>();
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class SUBVERTEX_TARGET=decltype(std::declval<SubvertexObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                bool IS_SUBVERTEX_TARGET = std::is_same<SUBVERTEX_TARGET, TARGET &>::value,
                typename std::enable_if<!IS_BASE_TARGET, bool>::type= 0
        >
        SUBVERTEX_TARGET get() {
            return subvertex.template get<TARGET>();
        }


        StateObject &state;
        SubvertexObject subvertex;
    };


    template<class CLASSIFIER, class STATEMACHINE, class ...REST>
    struct Object<uml::Region::type, CLASSIFIER, Object<uml::StateMachine::type, STATEMACHINE, REST...>>
            : Object<uml::Element::type, CLASSIFIER> {

        typedef Object<uml::Element::type, CLASSIFIER> BaseObject;
        typedef STATEMACHINE StateMachine;
        typedef Object<uml::StateMachine::type, STATEMACHINE, REST...> StateMachineObject;
        typedef Object<uml::Vertex::type, typename CLASSIFIER::subvertex, Object> SubvertexObject;
        typedef typename uml::Collection::select<typename CLASSIFIER::subvertex, partial<uml::Classifier::is_kind, uml::PsuedostateKind::initial>::type>::type InitialVertices;
        typedef Object<uml::State::type, void, Object> StateObject;
        typedef Object<uml::Psuedostate::type, typename uml::Collection::classifier<InitialVertices, 0>::type, Object> InitialVertexObject;

        static_assert((InitialVertices::size() > 0), "A direct region of a statemachine must have an initial vertex");

        template<class CONTEXT>
        Object(CONTEXT &object)
                : BaseObject(object), stateMachine(object), subvertex(*this) {}

        template<int INDEX = uml::Collection::index_of<typename CLASSIFIER::subvertex, typename uml::Collection::classifier<InitialVertices, 0>::type>::value>
        void activate(StateMachineObject &object) {
            this->active(true);
            this->subvertex.template get<INDEX>().enter(object);
        }

        template<class EVENT>
        bool process(StateMachineObject &object, EVENT &event) {
            return this->active() && subvertex.process(object, event);
        }

        StateMachineObject &containingStateMachine() {
            return stateMachine;
        }

//        using BaseObject::get;

//        template<class TARGET,
//                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
//                 class SUBVERTEX_TARGET=decltype(std::declval<SubvertexObject>().template get<TARGET>()),
//                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
//                bool IS_SUBVERTEX_TARGET = std::is_same<SUBVERTEX_TARGET, TARGET &>::value,
//                typename std::enable_if<!IS_BASE_TARGET, bool>::type= 0
//        >
//        SUBVERTEX_TARGET get() {
//            return subvertex.template get<TARGET>();
//        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class SUBVERTEX_TARGET=decltype(std::declval<SubvertexObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                bool IS_SUBVERTEX_TARGET = std::is_same<SUBVERTEX_TARGET, TARGET &>::value,
                typename std::enable_if<IS_BASE_TARGET, bool>::type= 0
        >
        BASE_TARGET get() {
            return BaseObject::template get<TARGET>();
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class SUBVERTEX_TARGET=decltype(std::declval<SubvertexObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                bool IS_SUBVERTEX_TARGET = std::is_same<SUBVERTEX_TARGET, TARGET &>::value,
                typename std::enable_if<!IS_BASE_TARGET, bool>::type= 0
        >
        SUBVERTEX_TARGET get() {
            return subvertex.template get<TARGET>();
        }

        StateObject stateObject;
        StateMachineObject &stateMachine;
        SubvertexObject subvertex;


    };

    template<class REGION, class STATE, class CONTAINER>
    struct Object<uml::Region::type, uml::collection<REGION>, Object<uml::State::type, STATE, CONTAINER>>
            : Object<uml::Collection::type, uml::collection<REGION>, Object<uml::State::type, STATE, CONTAINER>> {
        using Object<uml::Collection::type, uml::collection<REGION>, Object<uml::State::type, STATE, CONTAINER>>
        ::Object;
    };

    template<class REGION, class ...REGIONS, class STATE, class CONTAINER>
    struct Object<uml::Region::type,
            uml::collection<REGION, REGIONS...>, Object<uml::State::type, STATE, CONTAINER>>
            : Object<uml::Collection::type,
                    uml::collection<REGION, REGIONS...>, Object<uml::State::type, STATE, CONTAINER>> {
        typedef Object<uml::Collection::type,
                uml::collection<REGION, REGIONS...>, Object<uml::State::type, STATE, CONTAINER>>
                BaseObject;

        using BaseObject::BaseObject;

        void activate(typename BaseObject::StateMachineObject &object) {
            BaseObject::activate(object);
            this->next.activate(object);
        }


#if defined(__arm__) || defined(NON_CONCURRENT_REGIONS)
        template<class EVENT>
        bool process(typename BaseObject::StateMachineObject &object, EVENT &event) {
            return (BaseObject::process(object, event) |
                    (this->next.process(object, event) << 1)) > 0;
        }
#else

        template<class EVENT>
        bool process(typename BaseObject::StateMachineObject &object, EVENT &event) {
            std::future<bool> process1 = std::async(std::launch::async, &BaseObject::template process<EVENT>, this, std::ref(object), std::ref(event));
            std::future<bool> process2 = std::async(std::launch::async, &BaseObject::NextObject::template process<EVENT>, &this->next, std::ref(object), std::ref(event));
            return (process1.get() | (process2.get() << 1)) > 0;
        }
#endif

    };

    template<class REGION, class STATEMACHINE, class ...REST>
    struct Object<uml::Region::type, uml::collection<REGION>, Object<uml::StateMachine::type, STATEMACHINE, REST...>>
            : Object<uml::Collection::type, uml::collection<REGION>, Object<uml::StateMachine::type, STATEMACHINE, REST...>> {
        using Object<uml::Collection::type, uml::collection<REGION>, Object<uml::StateMachine::type, STATEMACHINE, REST...>>
        ::Object;
    };

    template<class REGION, class ...REGIONS, class STATEMACHINE, class ...REST>
    struct Object<uml::Region::type,
            uml::collection<REGION, REGIONS...>, Object<uml::StateMachine::type, STATEMACHINE, REST...>>
            : Object<uml::Collection::type,
                    uml::collection<REGION, REGIONS...>, Object<uml::StateMachine::type, STATEMACHINE, REST...>> {
        typedef Object<uml::Collection::type,
                uml::collection<REGION, REGIONS...>, Object<uml::StateMachine::type, STATEMACHINE, REST...>>
                BaseObject;

        using BaseObject::BaseObject;

        void activate(typename BaseObject::StateMachineObject &object) {
            BaseObject::activate(object);
            this->next.activate(object);
        }

#if defined(__arm__) || defined(NON_CONCURRENT_REGIONS)

        template<class EVENT>
        bool process(typename BaseObject::StateMachineObject &object, EVENT &event) {
            return (BaseObject::process(object, event) |
                    (this->next.process(object, event) << 1)) > 0;
        }
#else

        template<class EVENT>
        bool process(typename BaseObject::StateMachineObject &object, EVENT &event) {
            std::future<bool> process1 = std::async(std::launch::async, &BaseObject::template process<EVENT>, this, std::ref(object), std::ref(event));
            std::future<bool> process2 = std::async(std::launch::async, &BaseObject::NextObject::template process<EVENT>, &this->next, std::ref(object), std::ref(event));
            return (process1.get() | (process2.get() << 1)) > 0;
//            return (BaseObject::process(object, event) |
//                    (this->next.process(object, event) << 1)) > 0;
        }
#endif
    };
}

#endif //RTSM_REGION_H
