//
// Created by gwillen on 6/20/18.
//

#ifndef RTSM_UML_H
#define RTSM_UML_H

#include <tuple>


/*!
 * uml
 *
 * This header contains the uml classifier's and SFINAE related structures
 */

namespace uml {

    namespace {
        template<class ...>
        struct is_descendant;


        template<class ...>
        struct type_of;

        template<class ...>
        struct base;

        template<class ...>
        struct is_type;

        template<class KIND, class CLASSIFIER, typename=void>
        struct is_kind : std::false_type {
            typedef void type;
        };

        template<class ...>
        struct transition_is_enabled;

        template<class ...>
        struct transition_all;

        template<class ...>
        struct event_all;

        template<class ...>
        struct region_all;

        template<class ...>
        struct behavior_all;

        template<class ...>
        struct common;

    }

    struct Element;
    struct Classifier;

    template<class ...>
    struct Type;

//    template<class ...>
    struct Collection;
    template<class ...>
    struct collection;
    struct Behavior;
    struct StateMachine;
    struct Vertex;
    struct Region;
    struct State;
    struct Psuedostate;
    struct Transition;


    template<>
    struct Type<Element> {
    };

    struct Element {
        typedef Type<Element> type;


        template<class ...ARGS>
        using type_of = type_of<ARGS...>;

        template<class ...ARGS>
        using base = base<ARGS...>;
    };

    template<class TYPE>
    struct Type<TYPE> {
        typedef Element::type type;
    };

    template<class TYPE, class GENERALIZATION>
    struct Type<TYPE, GENERALIZATION> {
        typedef typename GENERALIZATION::type type;
    };

    struct Property : Element {
        typedef Type<Property> type;
    };

    struct Classifier : Element {

        typedef Type<Classifier> type;
        typedef void attribute;

        static constexpr std::size_t size() {
            return 1;
        }

        template<class ...>
        struct all {
            typedef collection<> type;
        };

        template<class ...ARGS>
        using is_descendant = is_descendant<ARGS...>;




        template<class ...ARGS>
        using is_kind = is_kind<ARGS...>;


    };


    struct Collection : Element {
        typedef Type<Collection> type;

        static constexpr std::size_t size() {
            return 0;
        }

        template<class ...ARGS>
        using common = common<ARGS...>;

        template<class ...>
        struct concat;

        template<class ...COLLECTION_1_ITEMS, class ...COLLECTION_2_ITEMS>
        struct concat<collection<COLLECTION_1_ITEMS...>, collection<COLLECTION_2_ITEMS...>> {
            typedef collection<COLLECTION_1_ITEMS..., COLLECTION_2_ITEMS...> type;
        };

        template<class ...COLLECTION_1_ITEMS, class ...COLLECTION_2_ITEMS, class ...COLLECTIONS>
        struct concat<collection<COLLECTION_1_ITEMS...>, collection<COLLECTION_2_ITEMS...>, COLLECTIONS...> {
            typedef typename concat<collection<COLLECTION_1_ITEMS..., COLLECTION_2_ITEMS...>, COLLECTIONS...>::type type;
        };

        template<class ...ITEMS>
        struct concat<collection<ITEMS...>, void> {
            typedef collection<ITEMS...> type;
        };

        template<class ...ITEMS>
        struct concat<void, collection<ITEMS...>> {
            typedef collection<ITEMS...> type;
        };

        template<class, template<class ...> class>
        struct select;

        template<template<class ...> class EXPR>
        struct select<collection<>, EXPR> {
            typedef collection<> type;
        };

        template<class ITEM, template<class ...> class EXPR>
        struct select<uml::collection<ITEM>, EXPR>
                : std::conditional<EXPR<ITEM>::value, uml::collection<ITEM>, uml::collection<>> {
        };

        template<class ITEM, class ...ITEMS, template<class ...> class EXPR>
        struct select<uml::collection<ITEM, ITEMS...>, EXPR>
                : concat<typename select<collection<ITEM>, EXPR>::type, typename select<collection<ITEMS...>, EXPR>::type> {
        };

        template<template<class ...> class EXPR>
        struct select<void, EXPR> {
            typedef collection<> type;
        };

        template<class, std::size_t>
        struct classifier;

        template<class ...ITEMS, std::size_t INDEX>
        struct classifier<collection<ITEMS...>, INDEX> : std::tuple_element<INDEX, std::tuple<ITEMS...>> {
        };

        template<std::size_t INDEX>
        struct classifier<collection<>, INDEX> {
            typedef void type;
        };

        template<class, class, int= 0, int= 0>
        struct index_of;

        template<class ITEM, class ...ITEMS, class CLASSIFIER>
        struct index_of<collection<ITEM, ITEMS...>, CLASSIFIER> {
            static const int value = std::is_same<ITEM, CLASSIFIER>::value ? 0
                                                                           : index_of<collection<ITEMS...>, CLASSIFIER, 0, 1>::value;

        };

        template<class ITEM, class ...ITEMS, class CLASSIFIER>
        struct index_of<collection<ITEM, ITEMS...>, collection<CLASSIFIER>>
                : index_of<collection<ITEM, ITEMS...>, CLASSIFIER> {

        };

        template<class ITEM, class ...ITEMS, class CLASSIFIER, int START, int INDEX>
        struct index_of<collection<ITEM, ITEMS...>, CLASSIFIER, START, INDEX> {
            static const int value = INDEX > START && std::is_same<CLASSIFIER, ITEM>::value ? INDEX
                                                                                            : index_of<collection<ITEMS...>, CLASSIFIER, START,
                            INDEX + 1>::value;
        };

        template<class CLASSIFIER, int START, int INDEX>
        struct index_of<collection<>, CLASSIFIER, START, INDEX> {
            static const int value = -1;
        };

        template<class CLASSIFIER, int START, int INDEX>
        struct index_of<void, CLASSIFIER, START, INDEX> {
            static const int value = -1;
        };

        template<class COLLECTION, class CLASSIFIER>
        struct count {
            template<class ITEM>
            using is_same = std::is_same<ITEM, CLASSIFIER>;

            static const std::size_t value = select<COLLECTION, is_same>::type::size();
        };
    };

    template<class ...>
    struct collection : Collection {
        static constexpr std::size_t size() {
            return 0;
        }

        template<std::size_t INDEX, class TYPE>
        void assign(TYPE &value) {}
    };

    template<class ITEM>
    struct collection<ITEM> : Collection, std::tuple<ITEM> {
        using Collection::type;
        typedef std::tuple<ITEM> classifiers;

        static constexpr std::size_t size() {
            return 1;
        }

        template<std::size_t INDEX>
        typename std::tuple_element<INDEX, classifiers>::type &get() {
            return std::get<INDEX>(*this);
        }

    };

    template<class ITEM, class ...ITEMS>
    struct collection<ITEM, ITEMS...> : Collection, std::tuple<ITEM, ITEMS...> {
        using Collection::type;
        typedef std::tuple<ITEM, ITEMS...> classifiers;

        static constexpr std::size_t size() {
            return 1 + sizeof...(ITEMS);
        }

        template<std::size_t INDEX, class TYPE>
        void assign(TYPE value) {
            std::get<INDEX>(*this) = value;
        }

        template<std::size_t INDEX>
        typename std::tuple_element<INDEX, classifiers>::type &get() {
            return std::get<INDEX>(*this);
        }

    };

    struct Behavior : Classifier {
        typedef Type<Behavior> type;

        static const bool isReentrant = false;

        template<class ...ARGS>
        using all = behavior_all<ARGS...>;
        typedef void context;
    };


    struct Constraint : Classifier {
        typedef Type<Constraint> type;

        typedef void context;
    };
    struct Region : Classifier {
        typedef Type<Region> type;
        typedef void stateMachine;
        typedef void state;
        typedef void transition;
        typedef void subvertex;

        template<class ...ARGS>
        using all = region_all<ARGS...>;
    };

    struct StateMachine : Behavior {
        typedef Type<StateMachine, Behavior> type;

        template<class ...>
        struct lca;

        template<class CLASSIFIER, class SOURCE, class TARGET>
        struct lca<CLASSIFIER, SOURCE, TARGET> {

            template<class REGION>
            struct is_lca {
                static const bool value = Classifier::is_descendant<SOURCE, REGION>::value &&
                                          Classifier::is_descendant<TARGET, REGION>::value;
            };
            typedef typename Collection::select<typename Region::all<CLASSIFIER>::type, is_lca>::type ancestors;
            typedef typename Collection::classifier<ancestors, ancestors::size() - 1>::type type;
            static_assert(!std::is_void<type>::value, "lca lookup failed this most likely means you failed to add a state to a regions subvertex");
        };

        template<class CLASSIFIER, class SOURCE>
        struct lca<CLASSIFIER, SOURCE, void> : lca<CLASSIFIER, SOURCE, SOURCE> {
        };


        template<class ...>
                struct LCAState;

        template<class CLASSIFIER, class SOURCE, class TARGET>
                struct LCAState<CLASSIFIER, SOURCE, TARGET> {

                    typedef typename lca<CLASSIFIER, SOURCE, TARGET>::type LCA;
                };



        typedef void region;
    };

    struct Vertex : Element {
        typedef Type<Vertex> type;
        typedef void outgoing;
        typedef void container;
    };
    struct State : Vertex {
        typedef Type<State, Vertex> type;
        typedef void submachine;
        typedef void doActivity;
        typedef void entry;
        typedef void exit;
        typedef void deferrableTrigger;
        typedef void region;
        static const bool isSubmachineState = false;
    };
    struct PsuedostateKind {
        struct initial;
        struct fork;
        struct choice;
        struct shallowHistory;
        struct deepHistory;
    };
    struct Psuedostate : Vertex {
        typedef Type<Psuedostate, Vertex> type;
        typedef void kind;
    };
    struct TransitionKind {
        struct internal;
        struct local;
        struct external;
        struct self;
    };

    namespace details {

    }

    struct Transition : Classifier {
        typedef Type<Transition> type;
        typedef void target;
        typedef void source;
        typedef void trigger;
        typedef void effect;
        typedef void guard;

        template<class ...ARGS>
        using all = transition_all<ARGS...>;


        template<class TRANSITION>
        using kind = typename std::conditional<std::is_void<typename TRANSITION::target>::value,
                typename TransitionKind::internal,
                typename std::conditional<Classifier::is_descendant<typename TRANSITION::target, typename TRANSITION::source>::value,
                        typename TransitionKind::local,
                        typename TransitionKind::external>::type>::type;

        template<class ...ARGS>
        using is_enabled = transition_is_enabled<ARGS...>;
    };

    struct Event : Element {
        typedef Type<Event> type;


        template<class ...ARGS>
        using all = event_all<ARGS...>;

    };

    struct Port : Element {
        typedef Type<Port> type;
    };

    struct Trigger : Element {
        typedef Type<Trigger> type;
        typedef void event;
    };

    struct Duration : Element {
        typedef Type<Duration> type;
    };

    struct TimeExpression : Element {
        typedef Type<TimeExpression> type;
    };

    struct TimeEvent : Event {
        typedef Type<TimeEvent, Event> type;
        typedef TimeExpression when;
    };

    struct CallEvent : Event {
        typedef Type<CallEvent, Event> type;
    };

    struct ChangeEvent : Event {
        typedef Type<ChangeEvent, Event> type;
        typedef void changeExpression;
    };

    namespace {

        template<class CLASSIFIER>
        struct not_is_void {
            static const bool value = !std::is_void<CLASSIFIER>::value;
        };

        template<class TYPE>
        struct is_type<Element::type, TYPE, std::false_type> : std::false_type {
        };

        template<class CLASSIFIER_TYPE, class TYPE>
        struct is_type<CLASSIFIER_TYPE, TYPE, std::true_type> : std::true_type {
        };

        template<class CLASSIFIER_TYPE, class TYPE>
        struct is_type<CLASSIFIER_TYPE, TYPE, std::false_type>
                : is_type<typename CLASSIFIER_TYPE::type, TYPE, typename std::is_same<typename CLASSIFIER_TYPE::type, TYPE>::type> {
        };

        template<class CLASSIFIER, class TYPE>
        struct is_type<CLASSIFIER, TYPE>
                : is_type<typename CLASSIFIER::type, TYPE, typename std::is_same<typename CLASSIFIER::type, TYPE>::type> {
        };

        template<class KIND, class CLASSIFIER>
        struct is_kind<KIND, CLASSIFIER, typename is_kind<KIND, typename CLASSIFIER::kind>::type>
                : std::is_same<typename CLASSIFIER::kind, KIND> {
        };


        template<class CLASSIFIER>
        struct type_of<CLASSIFIER> : CLASSIFIER {
        };

        template<class CLASSIFIER, class GENERALIZATION>
        struct type_of<CLASSIFIER, GENERALIZATION> : CLASSIFIER {
        };

        template<class DEFAULT>
        struct type_of<void, DEFAULT> : DEFAULT {
        };

        template<>
        struct type_of<void> : Element {
        };

        template<class ...>
        struct is_descendant;

        template<class DESCENDANT>
        struct is_descendant<Element::type, DESCENDANT, void> : std::false_type {
        };

        template<class DESCENDANT, class ANCESTOR>
        struct is_descendant<Region::type, DESCENDANT, ANCESTOR> {

            static const bool value = std::is_same<DESCENDANT, ANCESTOR>::value ||
                                      is_descendant<Element::type, DESCENDANT, typename ANCESTOR::subvertex>::value;
        };

        template<class DESCENDANT, class ANCESTOR>
        struct is_descendant<Vertex::type, DESCENDANT, ANCESTOR> : std::is_same<DESCENDANT, ANCESTOR> {
        };

        template<class DESCENDANT, class ANCESTOR>
        struct is_descendant<Psuedostate::type, DESCENDANT, ANCESTOR> : is_descendant<Vertex::type, DESCENDANT, ANCESTOR> {
        };


        template<class DESCENDANT, class ANCESTOR>
        struct is_descendant<State::type, DESCENDANT, ANCESTOR> {
            static const bool value = is_descendant<Vertex::type, DESCENDANT, ANCESTOR>::value ||
                                      is_descendant<Element::type, DESCENDANT, typename ANCESTOR::region>::value;
        };

        template<class ...ARGS>
        struct is_descendant<Psuedostate::type, ARGS...> : is_descendant<Vertex::type, ARGS...> {
        };

        template<class DESCENDANT, class ANCESTOR>
        struct is_descendant<Element::type, DESCENDANT, ANCESTOR>
                : is_descendant<typename ANCESTOR::type, DESCENDANT, ANCESTOR> {
        };

        template<class DESCENDANT, class ANCESTOR>
        struct is_descendant<Collection::type, DESCENDANT, collection<ANCESTOR>>
                : is_descendant<typename ANCESTOR::type, DESCENDANT, ANCESTOR> {
        };

        template<class TYPE, class DESCENDANT, class ANCESTOR>
        struct is_descendant<TYPE, DESCENDANT, ANCESTOR> : is_descendant<typename TYPE::type, DESCENDANT, ANCESTOR> {
        };

        template<class DESCENDANT, class ANCESTOR, class ...ANCESTORS>
        struct is_descendant<Collection::type, DESCENDANT, collection<ANCESTOR, ANCESTORS...>> {

            static const bool value = is_descendant<typename ANCESTOR::type, DESCENDANT, ANCESTOR>::value ||
                                      is_descendant<Element::type, DESCENDANT, collection<ANCESTORS...>>
                                      ::value;
        };

        template<class DESCENDANT, class ANCESTOR>
        struct is_descendant<DESCENDANT, ANCESTOR> {
            static const bool value = std::is_same<DESCENDANT, ANCESTOR>::value ? false
                                                                                : is_descendant<typename ANCESTOR::type, DESCENDANT, ANCESTOR>::value;
        };

        template<class TYPE>
        struct base<TYPE, std::true_type> {
            typedef TYPE type;
        };

        template<>
        struct base<Element::type, std::false_type> {
            typedef Element::type type;
        };

        template<class TYPE>
        struct base<TYPE, std::false_type>
                : std::conditional<std::is_same<typename TYPE::type, Element::type>::value, TYPE, typename base<typename TYPE::type, std::false_type>::type> {
        };

        template<class CLASSIFIER>
        struct base<CLASSIFIER>
                : base<typename CLASSIFIER::type, typename std::is_same<typename CLASSIFIER::type, Element::type>::type> {
        };


        template<class CLASSIFIER>
        struct common<collection<CLASSIFIER>> : CLASSIFIER {
            typedef typename CLASSIFIER::type type;
        };

        template<class CLASSIFIER, class ...CLASSIFIERS>
        struct common<Element::type, collection<CLASSIFIER, CLASSIFIERS...>> {
            typedef typename Element::type type;
        };

        template<class TYPE, class CLASSIFIER, class ...CLASSIFIERS>
        struct common<TYPE, collection<CLASSIFIER, CLASSIFIERS...>> {
            template<class _CLASSIFIER>
            struct is_common_type : is_type<_CLASSIFIER, TYPE> {
            };

            typedef typename Collection::select<collection<CLASSIFIER, CLASSIFIERS...>, is_common_type>::type selection;

            typedef typename std::conditional<(selection::size() ==
                                               collection<CLASSIFIER, CLASSIFIERS...>::size()), TYPE, typename common<typename TYPE::type, collection<CLASSIFIER, CLASSIFIERS...>>::type>::type type;
        };


        template<class CLASSIFIER, class ...CLASSIFIERS>
        struct common<collection<CLASSIFIER, CLASSIFIERS...>> : common<typename CLASSIFIER::type, collection<CLASSIFIER, CLASSIFIERS...>> {

        };


        template<class TRANSITION, class EVENT, class TRIGGER>
        struct transition_is_enabled<TRANSITION, EVENT, TRIGGER> {
            typedef typename std::remove_reference<EVENT>::type Event;
            static const bool value = std::is_base_of<typename TRIGGER::event, Event>::value ||
                                      std::is_same<typename TRIGGER::event, Event>::value;
        };

        template<class TRANSITION, class EVENT>
                struct transition_is_enabled<TRANSITION, EVENT, collection<>> : std::false_type {};

        template<class TRANSITION, class EVENT, class TRIGGER, class ...TRIGGERS>
        struct transition_is_enabled<TRANSITION, EVENT, collection<TRIGGER, TRIGGERS...>> {
            static const bool value = transition_is_enabled<TRANSITION, EVENT, TRIGGER>::value || transition_is_enabled<TRANSITION, EVENT, collection<TRIGGERS...>>::value;
        };

        template<class TRANSITION, class EVENT>
        struct transition_is_enabled<TRANSITION, EVENT, void> : std::is_base_of<Psuedostate, typename TRANSITION::source> {
        };

        template<class TRANSITION>
        struct transition_is_enabled<TRANSITION> : std::true_type {
        };

        template<class TRANSITION, class EVENT>
        struct transition_is_enabled<TRANSITION, EVENT>
                : transition_is_enabled<TRANSITION, EVENT, typename TRANSITION::trigger> {

        };


        template<class ...>
        struct transition_all;

        template<class TYPE>
        struct transition_all<TYPE, void> {
            typedef collection<> type;
        };

        template<class VERTEX, class OUTGOING>
        struct transition_all<Type<Vertex>, VERTEX, OUTGOING>
                : std::conditional<std::is_same<typename uml::Collection::type_of<typename VERTEX::outgoing>::type, Type<Collection>>::value, typename VERTEX::outgoing, collection<typename VERTEX::outgoing>> {
        };

        template<class VERTEX>
        struct transition_all<Type<Vertex>, VERTEX, void> {
            typedef collection<> type;
        };
        template<class VERTEX>
        struct transition_all<Type<Vertex>, VERTEX> : transition_all<Type<Vertex>, VERTEX, typename VERTEX::outgoing> {
        };

        template<class PSUEDOSTATE>
        struct transition_all<Type<Psuedostate, Vertex>, PSUEDOSTATE> : transition_all<Type<Vertex>, PSUEDOSTATE> {
        };

        template<class STATE, class SUBMACHINE>
        struct transition_all<State::type, STATE, SUBMACHINE> : transition_all<Type<Vertex>, STATE> {

        };

        template<class STATE>
        struct transition_all<State::type, STATE, void> : Collection::concat<typename transition_all<Type<Vertex>, STATE>::type,
                typename transition_all<typename Classifier::type_of<typename STATE::region>::type, typename STATE::region>::type> {

        };

        template<class STATE>
        struct transition_all<Type<State, Vertex>, STATE>
                : transition_all<State::type, STATE, typename STATE::submachine> {
        };

        template<class REGION>
        struct transition_all<Type<Region>, REGION>
                : Collection::concat<typename transition_all<Type<Classifier>, typename REGION::subvertex>::type, typename REGION::transition> {
        };


        template<class TYPE, class ITEM>
        struct transition_all<TYPE, collection<ITEM>> :

                transition_all<typename ITEM::type, ITEM> {
        };

        template<class TYPE, class ITEM, class ...ITEMS>
        struct transition_all<TYPE, collection<ITEM, ITEMS...>>
                : Collection::concat<typename transition_all<typename ITEM::type, ITEM>::type, typename transition_all<TYPE,
                        collection<ITEMS...>>::type> {

        };

        template<class CLASSIFIER>
        struct transition_all<Type<StateMachine, Behavior>, CLASSIFIER>
                : transition_all<typename Collection::type_of<typename CLASSIFIER::region>::type, typename CLASSIFIER::region> {

        };

        template<class TYPE, class CLASSIFIER>
        struct transition_all<TYPE, CLASSIFIER> : transition_all<typename TYPE::type, CLASSIFIER> {
        };

        template<class CLASSIFIER>
        struct transition_all<CLASSIFIER> : transition_all<typename CLASSIFIER::type, CLASSIFIER> {
        };


        template<class ...>
        struct event_all;

        template<class TRANSITION>
        struct event_all<Transition::type, TRANSITION, void> {
            typedef collection<> type;
        };

        template<class TRANSITION, class TRIGGER>
        struct event_all<Transition::type, TRANSITION, TRIGGER> {
            typedef collection<typename TRIGGER::event> type;
        };

        template<class TRANSITION>
        struct event_all<Transition::type, TRANSITION, collection<>> {
            typedef collection<> type;
        };

        template<class TRANSITION, class TRIGGER, class ...TRIGGERS>
        struct event_all<Transition::type, TRANSITION, collection<TRIGGER, TRIGGERS...>>
                : Collection::concat<typename event_all<Transition::type, TRANSITION, TRIGGER>::type, typename event_all<Transition::type, TRANSITION, collection<TRIGGERS...>>::type> {
        };

        template<class TRANSITION>
        struct event_all<Transition::type, collection<TRANSITION>>
                :

                        event_all<Transition::type, TRANSITION, typename TRANSITION::trigger> {
        };

        template<class TRANSITION, class ...TRANSITIONS>
        struct event_all<Transition::type, collection<TRANSITION, TRANSITIONS...>>
                : Collection::concat<typename event_all<Transition::type, TRANSITION, typename TRANSITION::trigger>::type, typename event_all<Transition::type,
                        collection<TRANSITIONS...>>::type> {
        };

        template<>
        struct event_all<Transition::type, uml::collection<>> {
            typedef uml::collection<> type;
        };

        template<class BEHAVIOR>
        struct event_all<Behavior::type, BEHAVIOR> {
            typedef collection<> type;
        };


        template<class STATEMACHINE>
        struct event_all<StateMachine::type, STATEMACHINE> : event_all<Transition::type, typename Transition::all<STATEMACHINE>::type> {
        };

        template<class CONTEXT>
        struct event_all<CONTEXT> : event_all<typename CONTEXT::type, CONTEXT> {
        };


        template<class STATE>
        struct region_all<Type<State, Vertex>, STATE> : region_all<typename STATE::region> {
        };

        template<class STATE>
        struct region_all<Type<Psuedostate, Vertex>, STATE> {
            typedef collection<> type;
        };

        template<class REGION>
        struct region_all<Type<Region>, REGION>
                : Collection::concat<collection<REGION>, typename region_all<typename REGION::subvertex>::type> {
        };

        template<class ITEM>
        struct region_all<Type<Collection>, collection<ITEM>> : region_all<typename ITEM::type, ITEM> {
        };

        template<class ITEM, class ...ITEMS>
        struct region_all<Type<Collection>, collection<ITEM, ITEMS...>>
                : Collection::concat<typename region_all<typename ITEM::type, ITEM>::type, typename region_all<Type<Collection>, collection<ITEMS...>>::type> {
        };
        template<class CLASSIFIER>
        struct region_all<Type<StateMachine, Behavior>, CLASSIFIER>
                : region_all<typename CLASSIFIER::region::type, typename CLASSIFIER::region> {

        };

        template<class TYPE, class CLASSIFIER>
        struct region_all<TYPE, CLASSIFIER> : region_all<typename TYPE::type, CLASSIFIER> {
        };

        template<class CLASSIFIER>
        struct region_all<CLASSIFIER> : region_all<typename CLASSIFIER::type, CLASSIFIER> {
        };

        template<>
        struct region_all<void> {
            typedef collection<> type;
        };


        template<class BEHAVIOR>
        struct behavior_all<Type<Behavior>, BEHAVIOR> {
            typedef collection<> type;
        };

        template<>
        struct behavior_all<Type<Vertex>, void> {
            typedef collection<> type;
        };

        template<class VERTEX>
        struct behavior_all<Type<Vertex>, VERTEX> : behavior_all<Type<Transition>, typename VERTEX::outgoing> {
        };

        template<class PSUEDOSTATE>
        struct behavior_all<Type<Psuedostate, Vertex>, PSUEDOSTATE> : behavior_all<Type<Vertex>, PSUEDOSTATE> {
        };

        template<class STATE, class SUBMACHINE>
        struct behavior_all<State::type, STATE, SUBMACHINE> {
            typedef typename Collection::concat<collection<SUBMACHINE, typename STATE::doActivity, typename STATE::entry, typename STATE::exit>,
                    typename behavior_all<Type<Vertex>, STATE>::type>::type type;
        };

        template<class STATE>
        struct behavior_all<State::type, STATE, void> {
            typedef typename Collection::concat<collection<typename STATE::doActivity, typename STATE::entry, typename STATE::exit>,
                    typename behavior_all<Type<Vertex>, STATE>::type,
                    typename behavior_all<Type<Region>, typename STATE::region>::type>::type type;
        };

        template<class STATE>
        struct behavior_all<State::type, STATE> : behavior_all<State::type, STATE, typename STATE::submachine> {


        };

        template<class TRANSITION>
        struct behavior_all<Type<Transition>, TRANSITION, std::true_type> {
            typedef collection<> type;
        };

        template<class TRANSITION>
        struct behavior_all<Type<Transition>, TRANSITION, std::false_type> {
            typedef collection<typename TRANSITION::effect> type;
        };

        template<class TRANSITION>
        struct behavior_all<Type<Transition>, TRANSITION>
                : behavior_all<Type<Transition>, TRANSITION, typename std::is_void<TRANSITION>::type> {
        };

        template<>
        struct behavior_all<Type<Transition>, collection<>> {
            typedef collection<> type;
        };

        template<class TRANSITION>
        struct behavior_all<Type<Transition>, collection<TRANSITION>> : behavior_all<Type<Transition>, TRANSITION> {
        };

        template<class TRANSITION, class ...TRANSITIONS>
        struct behavior_all<Type<Transition>, collection<TRANSITION, TRANSITIONS...>>
                : Collection::concat<typename behavior_all<Type<Transition>, TRANSITION>::type, typename behavior_all<
                        Type<Transition>, collection<TRANSITIONS...>>::type> {
        };

        template<class VERTEX>
        struct behavior_all<Type<Vertex>, collection<VERTEX>> :

                behavior_all<typename VERTEX::type, VERTEX> {
        };

        template<class VERTEX, class ...VERTICES>
        struct behavior_all<Type<Vertex>, collection<VERTEX, VERTICES...>>
                : Collection::concat<typename behavior_all<typename VERTEX::type, VERTEX>::type, typename behavior_all<Type<Vertex>,
                        collection<VERTICES...>>::type> {
        };

        template<>
        struct behavior_all<Type<Region>, void> {
            typedef collection<> type;
        };

        template<>
        struct behavior_all<Type<Transition>, void> {
            typedef collection<> type;
        };

        template<class REGION>
        struct behavior_all<Type<Region>, REGION>
                : Collection::concat<typename behavior_all<Type<Vertex>, typename REGION::subvertex>::type, typename behavior_all<
                        Type<Transition>, typename REGION::transition>::type> {

        };

        template<class REGION>
        struct behavior_all<Type<Region>, collection<REGION>> :

                behavior_all<typename REGION::type, REGION> {
        };

        template<class REGION, class ...REGIONS>
        struct behavior_all<Type<Region>, collection<REGION, REGIONS...>>
                : Collection::concat<typename behavior_all<typename REGION::type, REGION>::type, typename behavior_all<Type<Region>,
                        collection<REGIONS...>>::type> {
        };

        template<class STATEMACHINE>
        struct behavior_all<Type<StateMachine, Behavior>, STATEMACHINE> : behavior_all<Type<Region>, typename STATEMACHINE::region> {
        };

        template<class TYPE, class CLASSIFIER>
        struct behavior_all<TYPE, CLASSIFIER> : behavior_all<typename TYPE::type, CLASSIFIER> {

        };

        template<class CONTEXT>
        struct behavior_all<CONTEXT> : Collection::select<typename behavior_all<typename CONTEXT::type, CONTEXT>::type, not_is_void> {
        };

    }
}

#endif //RTSM_UML_H
