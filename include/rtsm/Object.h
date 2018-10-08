//
// Created by gwillen on 6/19/18.
//

#ifndef RTSM_OBJECT_H
#define RTSM_OBJECT_H

#include "uml.h"


namespace rtsm {


    template<class ...>
    struct Object;


    template<class ...>
    struct declobject;

    template<class CLASSIFIER, class ...REST>
    struct declobject<Object<uml::Behavior::type, CLASSIFIER, REST...>, void> {
        typedef void type;
    };

    template<class CLASSIFIER, class ...REST>
    struct declobject<Object<uml::StateMachine::type, CLASSIFIER, REST...>, void> {
        typedef void type;
    };

    template<class STATEMACHINE, class ...REST, class CLASSIFIER>
    struct declobject<Object<uml::StateMachine::type, STATEMACHINE, REST...>, uml::Behavior::type, CLASSIFIER> {
        typedef Object<uml::StateMachine::type, STATEMACHINE, REST...> StateMachineObject;
        typedef typename declobject<Object<uml::Region::type, typename STATEMACHINE::region, StateMachineObject>, uml::Behavior::type, CLASSIFIER>::type NestedBehavior;

        typedef Object<uml::Behavior::type, CLASSIFIER, Object<uml::StateMachine::type, STATEMACHINE, REST...>> type;
    };

    template<class CONTEXT, class REGION, class ...REST, class TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Region::type, REGION, REST...>, TYPE, CLASSIFIER> {
        typedef Object<uml::Vertex::type, typename REGION::subvertex, Object<uml::Region::type, REGION, REST...>> SubvertexObject;
        typedef typename std::conditional<std::is_same<REGION, CLASSIFIER>::value, Object<uml::Region::type, REGION, REST...>, typename declobject<CONTEXT, SubvertexObject, TYPE, CLASSIFIER>::type>::type type;

    };


    template<class CONTEXT, class ...REST, class TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Region::type, void, REST...>, TYPE, CLASSIFIER> {
        typedef void type;
    };

    template<class CONTEXT, class ...REST, class TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Vertex::type, void, REST...>, TYPE, CLASSIFIER> {
        typedef void type;
    };

    template<class CONTEXT, class STATE, class CONTAINER, class SUBMACHINE, class TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::State::type, STATE, CONTAINER, SUBMACHINE>, TYPE, CLASSIFIER> {
        typedef typename std::conditional<std::is_same<STATE, CLASSIFIER>::value, Object<uml::State::type, STATE, CONTAINER>, typename declobject<Object<uml::StateMachine::type, SUBMACHINE, CONTEXT>, TYPE, CLASSIFIER>::type>::type type;
    };

    template<class CONTEXT, class STATE, class CONTAINER, class TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::State::type, STATE, CONTAINER, void>, TYPE, CLASSIFIER> {
        typedef typename std::conditional<std::is_same<STATE, CLASSIFIER>::value, Object<uml::State::type, STATE, CONTAINER>, typename declobject<CONTEXT, Object<uml::Region::type, typename STATE::region, Object<uml::State::type, STATE, CONTAINER>>, TYPE, CLASSIFIER>::type>::type type;
    };

    template<class CONTEXT, class STATE, class CONTAINER, class TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::State::type, STATE, CONTAINER>, TYPE, CLASSIFIER>
            : declobject<CONTEXT, Object<uml::State::type, STATE, CONTAINER, typename STATE::submachine>, TYPE, CLASSIFIER> {

    };

    template<class CONTEXT, class STATE, class ...REST, class TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Psuedostate::type, STATE, REST...>, TYPE, CLASSIFIER>
            : std::conditional<std::is_same<STATE, CLASSIFIER>::value, Object<uml::Psuedostate::type, STATE, REST...>, void> {
    };


    template<class STATEMACHINE, class ...REST, class TYPE, class CLASSIFIER>
    struct declobject<Object<uml::StateMachine::type, STATEMACHINE, REST...>, TYPE, CLASSIFIER> {
        typedef Object<uml::StateMachine::type, STATEMACHINE, REST...> StateMachineObject;
        typedef typename std::conditional<std::is_same<STATEMACHINE, CLASSIFIER>::value, StateMachineObject, typename declobject<StateMachineObject, Object<uml::Region::type, typename STATEMACHINE::region, StateMachineObject>, typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER>::type>::type type;
    };


    template<class CONTEXT, class ITEM, class ...REST, class CLASSIFIER_TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Collection::type, uml::collection<ITEM>, REST...>, CLASSIFIER_TYPE, CLASSIFIER>
            : declobject<CONTEXT, Object<typename ITEM::type, ITEM, REST...>, CLASSIFIER_TYPE, CLASSIFIER> {
    };

    template<class CONTEXT, class ITEM, class ...ITEMS, class ...REST, class CLASSIFIER_TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Collection::type, uml::collection<ITEM, ITEMS...>, REST...>, CLASSIFIER_TYPE, CLASSIFIER> {
        typedef typename declobject<CONTEXT, Object<typename ITEM::type, ITEM, REST...>, CLASSIFIER_TYPE, CLASSIFIER>::type DeclaredObject;
        typedef typename std::conditional<std::is_void<DeclaredObject>::value, typename declobject<CONTEXT,
                Object<uml::Collection::type, uml::collection<ITEMS...>, REST...>, typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER>::type, DeclaredObject>
        ::type type;
    };


    template<class CONTEXT, class ...ITEMS, class ...REST, class CLASSIFIER_TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Region::type, uml::collection<ITEMS...>, REST...>, CLASSIFIER_TYPE, CLASSIFIER>
            : declobject<CONTEXT, Object<uml::Collection::type, uml::collection<ITEMS...>, REST...>, CLASSIFIER_TYPE, CLASSIFIER> {
    };

    template<class CONTEXT, class ...ITEMS, class ...REST, class CLASSIFIER_TYPE, class CLASSIFIER>
    struct declobject<CONTEXT, Object<uml::Vertex::type, uml::collection<ITEMS...>, REST...>, CLASSIFIER_TYPE, CLASSIFIER>
            : declobject<CONTEXT, Object<uml::Collection::type, uml::collection<ITEMS...>, REST...>, CLASSIFIER_TYPE, CLASSIFIER> {
    };

    template<class CONTEXT, class ...REST, class TYPE>
    struct declobject<CONTEXT, Object<REST...>, TYPE, uml::collection<>> {
        typedef Object<uml::Classifier::type, void> type;
    };

    template<class CONTEXT, class ...REST, class TYPE, class ITEM>
    struct declobject<CONTEXT, Object<REST...>, TYPE, uml::collection<ITEM>>
            : declobject<CONTEXT, Object<REST...>, typename ITEM::type, ITEM> {
    };

    template<class CONTEXT, class ...REST, class TYPE, class ITEM, class ...ITEMS>
    struct declobject<CONTEXT, Object<REST...>, TYPE, uml::collection<ITEM, ITEMS...>> {
        typedef typename uml::Collection::concat<typename declobject<CONTEXT,
                Object<REST...>, typename ITEM::type, ITEM>::type, typename declobject<CONTEXT,
                Object<REST...>, TYPE, uml::collection<ITEMS...>>
        ::type>
        ::type type;
    };

    template<class CONTEXT, class ...REST, class CLASSIFIER>
    struct declobject<Object<uml::Behavior::type, CONTEXT, REST...>, CLASSIFIER>
            : declobject<Object<typename CONTEXT::type, CONTEXT, REST...>, typename CLASSIFIER::type, CLASSIFIER> {
    };

    template<class CONTEXT, class ...REST, class CLASSIFIER>
    struct declobject<Object<uml::StateMachine::type, CONTEXT, REST...>, CLASSIFIER>
            : declobject<Object<uml::StateMachine::type, CONTEXT, REST...>, typename CLASSIFIER::type, CLASSIFIER> {
    };

//    template<class CONTEXT, class ...REST, class CLASSIFIER>
//    struct declobject<CONTEXT, Object<REST...>, CLASSIFIER>
//            : declobject<CONTEXT, Object<REST...>, typename CLASSIFIER::type, CLASSIFIER> {
//    };



    template<class CLASSIFIER, class ...REST>
    struct Object<CLASSIFIER, REST...> : Object<typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER, REST...> {
        using Object<typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER, REST...>::Object;

    };


}

#endif //RTSM_OBJECT_H
