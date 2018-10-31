//
// Created by gwillen on 6/21/18.
//

#ifndef RTSM_TRANSITION_H
#define RTSM_TRANSITION_H

#include "CompletionEvent.h"
#include "Collection.h"
#include "Constraint.h"
#include <functional>
#include "Region.h"

namespace rtsm {

    struct CompletionTransition : uml::Transition {
        typedef uml::Type<CompletionTransition, uml::Transition> type;
    };


    struct Transition : uml::Transition {

//        template<class ...ARGS>
//        using is_enabled = is_enabled<ARGS...>;
    };

    namespace {

        template<class ...>
        struct is_enabled;
//
//        template<class TRANSITION>
//        struct is_enabled<TRANSITION> : std::conditional<TRANSITION::id > 0, std::integral_value<int, 0>,,
    }

    template<class ...>
    struct transition;

    template<class SOURCE>
    struct transition<uml::Vertex::type, SOURCE> : Transition {
        typedef SOURCE source;

    };


    template<class SOURCE, class TARGET>
    struct transition<uml::Vertex::type, SOURCE, TARGET> : Transition {
        typedef SOURCE source;
        typedef TARGET target;
    };


    template<class SOURCE, class TARGET>
    struct transition<uml::State::type, SOURCE, TARGET> : CompletionTransition {

        typedef SOURCE source;
        typedef TARGET target;


        struct trigger : uml::Trigger {
            typedef transition A_trigger_transition;

            struct event : CompletionEvent {
                typedef trigger A_event_trigger;
            };
        };
    };

    template<class EVENT, class ...ARGS>
    struct transition<uml::Event::type, EVENT, ARGS...> : transition<uml::Vertex::type, ARGS...> {
        struct trigger : uml::Trigger {
            typedef EVENT event;
        };

    };


    template<class ITEM, class ...ITEMS, class ...ARGS>
    struct transition<uml::Collection::type, uml::collection<ITEM, ITEMS...>, ARGS...> : transition<uml::Vertex::type, ARGS...> {


        template<class ...>
                struct to_trigger;


        template<class CLASSIFIER>
                struct to_trigger<std::false_type, CLASSIFIER> {
                    struct type : uml::Trigger {
                        typedef CLASSIFIER event;
                    };
                };

        template<class CLASSIFIER>
                struct to_trigger<std::true_type, CLASSIFIER> {
                    typedef CLASSIFIER type;
                };

        template<class CLASSIFIER>
                struct to_trigger<CLASSIFIER> : to_trigger<typename std::is_base_of<uml::Trigger, CLASSIFIER>::type, CLASSIFIER> {};

        typedef typename map<uml::collection<ITEM, ITEMS...>, to_trigger>::type trigger;
    };

    template<class EVENT, class ...ARGS>
    struct transition<uml::TimeEvent::type, EVENT, ARGS...> : transition<uml::Event::type, EVENT, ARGS...> {
    };

    template<class EVENT, class ...ARGS>
    struct transition<uml::ChangeEvent::type, EVENT, ARGS...> : transition<uml::Event::type, EVENT, ARGS...> {
    };

    template<class SOURCE, class TARGET>
    struct transition<uml::Psuedostate::type, SOURCE, TARGET> : transition<uml::Vertex::type, SOURCE, TARGET> {
    };

    template<class TRIGGER, class ...ARGS>
    struct transition<uml::Trigger::type, TRIGGER, ARGS...> : transition<uml::Vertex::type, ARGS...> {
        typedef TRIGGER trigger;
    };

    template<class ARG, class ...ARGS>
    struct transition<ARG, ARGS...> : transition<typename ARG::type, ARG, ARGS...> {

    };

    template<class SOURCE>
    struct Object<uml::Transition::type, void, SOURCE> {
        Object(SOURCE &) {}

        template<class ...ARGS>
        bool execute(ARGS &&...args) { return false; }
    };



    template<class CLASSIFIER, class SOURCE_TYPE, class SOURCE, class CONTAINER>
    struct Object<uml::Transition::type, CLASSIFIER, Object<SOURCE_TYPE, SOURCE, CONTAINER>, void>
            : Object<uml::Element::type, CLASSIFIER> {

        typedef Object<SOURCE_TYPE, SOURCE, CONTAINER> SourceObject;
        typedef typename uml::StateMachine::lca<typename CONTAINER::StateMachineObject::classifier, typename CLASSIFIER::source, typename CLASSIFIER::target>::type LeastCommonAncestor;
        typedef Object<uml::Transition::kind<CLASSIFIER>, CLASSIFIER, SourceObject> DerivedObject;
        typedef Object<uml::Element::type, CLASSIFIER> BaseObject;
        typedef Object<typename uml::Element::type_of<typename CLASSIFIER::guard, uml::Constraint>::type, typename CLASSIFIER::guard, typename CONTAINER::StateMachineObject> GuardObject;
        typedef typename declobject<typename CONTAINER::StateMachineObject, typename CLASSIFIER::target>::type TargetObject;
        typedef CONTAINER ContainerObject;

        Object(SourceObject &object) : source(object) {}

        template<class CONTEXT, class CURRENT, class ...ARGS, bool ENABLED = Transition::is_enabled<CLASSIFIER, ARGS...>::value, typename std::enable_if<ENABLED, bool>::type= 0>
        bool execute(CONTEXT &object, CURRENT &vertex, ARGS &&...args) {
            if (guard.evaluate(object, std::forward<ARGS>(args)...)) {
                static_cast<DerivedObject *>(this)->traverse(object, vertex, std::forward<ARGS>(args)...);
                return true;
            }
            return false;
        }


        template<class CONTEXT, class CURRENT, class ...ARGS, bool ENABLED = Transition::is_enabled<CLASSIFIER, ARGS...>::value, typename std::enable_if<!ENABLED, bool>::type= 0>
        bool execute(CONTEXT &object, CURRENT &vertex, ARGS &&...args) {
            return false;
        }


        template<class CONTEXT, class KIND, class PSUEDOSTATE, class _CONTAINER, class ...ARGS>
        static void enter(CONTEXT &object, Object<uml::Psuedostate::type, KIND, PSUEDOSTATE, _CONTAINER> &current,
                   ARGS &&...args) {
            current.enter(object, std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class STATE, class _CONTAINER, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                bool IS_DESCENDANT = uml::Classifier::is_descendant<TARGET, STATE>::value,
                bool IS_SAME = std::is_same<TARGET, STATE>::value,
                typename std::enable_if<IS_SAME && !IS_DESCENDANT && !STATE::isSubmachineState, bool>::type= 0
        >
        static void enter(CONTEXT &object, Object<uml::State::type, STATE, _CONTAINER> &current,
                   ARGS &&...args) {
            current.entry(object);
            current.region.activate(object);
        }

        template<class CONTEXT, class STATE, class _CONTAINER, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                typename std::enable_if<STATE::isSubmachineState, bool>::type= 0
        >
        static void enter(CONTEXT &object, Object<uml::State::type, STATE, _CONTAINER> &current,
                   ARGS &&...args) {
            current.entry(object);
        }

        template<class CONTEXT, class STATE, class _CONTAINER, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                bool IS_DESCENDANT = uml::Classifier::is_descendant<TARGET, STATE>::value,
                bool IS_SAME = std::is_same<TARGET, STATE>::value,
                typename std::enable_if<!IS_SAME && IS_DESCENDANT && !STATE::isSubmachineState, bool>::type= 0
        >
        static void enter(CONTEXT &object, Object<uml::State::type, STATE, _CONTAINER> &current,
                   ARGS &&...args) {
            if (!current.active()) {
                current.entry(object);
            }
            enter(object, current.region, std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class STATE, class _CONTAINER, class ...ARGS, class TARGET=typename CLASSIFIER::target, bool IS_SAME = std::is_same<TARGET, STATE>::value,
                typename std::enable_if<IS_SAME, bool>::type= 0
        >
        static void enter(CONTEXT &object, Object<uml::FinalState::type, STATE, _CONTAINER> &current, ARGS &&...args) {
                exit(object, current.container, std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class CURRENT, class ...REST, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                bool IS_DESCENDANT = uml::Classifier::is_descendant<TARGET, CURRENT>::value,
                bool IS_COLLECTION = (CURRENT::size() > 1),
                typename std::enable_if<!IS_DESCENDANT && !IS_COLLECTION, bool>::type= 0>
        static void enter(CONTEXT &object, Object<Region::type, CURRENT, REST...> &current, ARGS &&...args) {
            if (!current.active()) {
                current.activate(object);
            }
        }


        template<class CONTEXT, class CURRENT, class ...REST, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                bool IS_DESCENDANT = uml::Classifier::is_descendant<TARGET, CURRENT>::value,
                bool IS_COLLECTION = (CURRENT::size() > 1),
                typename std::enable_if<IS_DESCENDANT && !IS_COLLECTION, bool>::type= 0>
        static void enter(CONTEXT &object, Object<Region::type, CURRENT, REST...> &current, ARGS &&...args) {
            current.active(true);
            enter(object, current.subvertex, std::forward<ARGS>(args)...);
        }


        template<class CONTEXT, class ITEM1, class ITEM2, class ...ITEMS, class ...REST, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                 class CURRENT=Object<typename ITEM1::type, ITEM1, REST...>>
        static void enter(CONTEXT &object, Object<Region::type, uml::collection<ITEM1, ITEM2, ITEMS...>, REST...> &current,
              ARGS &&...args) {
            enter(object, static_cast<CURRENT &>(current), std::forward<ARGS>(args)...);
            enter(object, current.next, std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class ITEM1, class ...REST, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                 class CURRENT=Object<typename ITEM1::type, ITEM1, REST...>
        >
        static void enter(CONTEXT &object, Object<uml::Vertex::type, uml::collection<ITEM1>, REST...> &current,
              ARGS &&...args) {
            enter(object, static_cast<CURRENT &>(current), std::forward<ARGS>(args)...);
        }


        template<class CONTEXT, class ITEM1, class ITEM2, class ...ITEMS, class ...REST, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                 class CURRENT=Object<typename ITEM1::type, ITEM1, REST...>,
                bool IS_DESCENDANT = uml::Classifier::is_descendant<TARGET, ITEM1>::value,
                bool IS_SAME = std::is_same<TARGET, ITEM1>::value,
                typename std::enable_if<IS_SAME || IS_DESCENDANT, bool>::type= 0
        >
        static void enter(CONTEXT &object, Object<uml::Vertex::type, uml::collection<ITEM1, ITEM2, ITEMS...>, REST...> &current,
              ARGS &&...args) {
            enter(object, static_cast<CURRENT &>(current), std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class ITEM1, class ITEM2, class ...ITEMS, class ...REST, class ...ARGS,
                 class TARGET=typename CLASSIFIER::target,
                 class CURRENT=Object<typename ITEM1::type, ITEM1, REST...>,
                bool IS_DESCENDANT = uml::Classifier::is_descendant<TARGET, ITEM1>::value,
                bool IS_SAME = std::is_same<TARGET, ITEM1>::value,
                typename std::enable_if<!IS_SAME && !IS_DESCENDANT, bool>::type= 0
        >
        static void enter(CONTEXT &object, Object<uml::Vertex::type, uml::collection<ITEM1, ITEM2, ITEMS...>, REST...> &current,
              ARGS &&...args) {
            enter(object, current.next, std::forward<ARGS>(args)...);
        }


        template<class CONTEXT, class STATE, class _CONTAINER, class ...ARGS>
        static void exit(CONTEXT &object, Object<uml::State::type, STATE, _CONTAINER> &current, ARGS &&...args) {
            if (current.active()) {
                exit(object, current.region, std::forward<ARGS>(args)...);
                current.exit(object);
            }
        }

        template<class CONTEXT, class ...REST, class ...ARGS>
        static void exit(CONTEXT &object, Object<Region::type, void, REST...> &current, ARGS &&...args) {
        }

        template<class CONTEXT, class REGION, class ...REST, class ...ARGS>
        static void exit(CONTEXT &object, Object<Region::type, REGION, REST...> &current, ARGS &&...args) {
            if (current.active()) {
                exit(object, current.subvertex, std::forward<ARGS>(args)...);
                current.active(false);
            }
        }

        template<class CONTEXT,  class STATE, class _CONTAINER, class ...ARGS>
        static void exit(CONTEXT &object, Object<uml::FinalState::type, STATE, _CONTAINER> &current, ARGS &&...args) {}

        template<class CONTEXT, class KIND, class PSUEDOSTATE, class _CONTAINER, class ...ARGS>
        static void exit(CONTEXT &object, Object<uml::Psuedostate::type, KIND, PSUEDOSTATE, _CONTAINER> &current, ARGS &&...args) {}

        template<class CONTEXT, class ITEM, class ...REST, class ...ARGS>
        static void exit(CONTEXT &object, Object<Region::type, collection<ITEM>, REST...

        > &current,
                  ARGS &&...args
        ) {
            exit(object, static_cast<Object<Region::type, ITEM, REST...> &>(current), std::forward<ARGS>(args)...);
        }

        template<class CONTEXT,  class ITEM, class ...ITEMS, class ...REST, class ...ARGS>
        static void exit(CONTEXT &object, Object<Region::type, collection<ITEM, ITEMS...>, REST...

        > &current,
             ARGS &&...args
        ) {
            exit(object, static_cast<Object<Region::type, ITEM, REST...> &>(current), std::forward<ARGS>(args)...);
            exit(object, current.next, std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class ITEM, class ...REST, class ...ARGS>
        static void exit(CONTEXT &object, Object<Vertex::type, collection<ITEM>, REST...

        > &current,
                  ARGS &&...args
        ) {
            exit(object, static_cast<Object<typename ITEM::type, ITEM, REST...> &>(current), std::forward<ARGS>(args)...);
        }

        template<class CONTEXT,  class ITEM, class ...ITEMS, class ...REST, class ...ARGS>
        static void exit(CONTEXT &object, Object<Vertex::type, collection<ITEM, ITEMS...>, REST...

        > &current,
             ARGS &&...args
        ) {
            exit(object, static_cast<Object<typename ITEM::type, ITEM, REST...> &>(current), std::forward<ARGS>(args)...);
            exit(object, current.next, std::forward<ARGS>(args)...);
        }




        template<class LCA, class CONTEXT, class KIND, class PSUEDOSTATE, class _CONTAINER, class ...ARGS, typename std::enable_if<std::is_same<_CONTAINER, LCA>::value, bool>::type= 0>
        static LCA &exit(CONTEXT &object, Object<uml::Psuedostate::type, KIND, PSUEDOSTATE, _CONTAINER> &current, ARGS &&...args) {
            return current.container;
        }

        template<class LCA, class CONTEXT, class STATE, class _CONTAINER, class ...ARGS, typename std::enable_if<std::is_same<_CONTAINER, LCA>::value, bool>::type= 0>
        static LCA &exit(CONTEXT &object, Object<uml::State::type, STATE, _CONTAINER> &current, ARGS &&...args) {
            exit(object, current, std::forward<ARGS>(args)...);
            return current.container;
        }

        template<class LCA, class CONTEXT, class STATE, class _CONTAINER, class ...ARGS, typename std::enable_if<!std::is_same<_CONTAINER, LCA>::value, bool>::type= 0>
        static LCA &exit(CONTEXT &object, Object<uml::State::type, STATE, _CONTAINER> &current, ARGS &&...args) {
            exit(object, current, std::forward<ARGS>(args)...);
            return exit<LCA>(object, current.container.state, std::forward<ARGS>(args)...);
        }


        SourceObject &source;
        GuardObject guard;

    };


    template<class CLASSIFIER, class SOURCE, class TRIGGER>
    struct Object<uml::Transition::type, CLASSIFIER, SOURCE, TRIGGER>
            : Object<uml::Transition::type, CLASSIFIER, SOURCE, void> {

        using Object<uml::Transition::type, CLASSIFIER, SOURCE, void>::Object;
    };


    template<class TRANSITION, class SOURCE>
    struct Object<uml::TransitionKind::external, TRANSITION, SOURCE>
            : Object<uml::Transition::type, TRANSITION, SOURCE, typename TRANSITION::trigger> {

        using Object<uml::Transition::type, TRANSITION, SOURCE, typename TRANSITION::trigger>::Object;


        template<class CONTEXT, class CURRENT, class ...ARGS,
                 class LCA=typename declobject<CONTEXT, typename uml::StateMachine::lca<typename CONTEXT::classifier, typename TRANSITION::source, typename TRANSITION::target>::type>::type>
        void traverse(CONTEXT &object, CURRENT &current, ARGS &&...args) {
            LCA &lca = this->template exit<LCA>(object, current, std::forward<ARGS>(args)...);
            object.template execute<typename TRANSITION::effect>(std::forward<ARGS>(args)...);
            this->enter(object, lca.subvertex, std::forward<ARGS>(args)...);
        }

    };

    template<class TRANSITION, class SOURCE>
    struct Object<uml::TransitionKind::local, TRANSITION, SOURCE>
            : Object<uml::TransitionKind::external, TRANSITION, SOURCE> {

        typedef Object<uml::TransitionKind::external, TRANSITION, SOURCE> BaseObject;
        using BaseObject::Object;


        template<class CONTEXT, class CURRENT, class ...ARGS>
        void traverse(CONTEXT &object, CURRENT &current, ARGS &&...args) {
            static_assert(std::is_same<CURRENT, typename BaseObject::SourceObject>::value, "Local transition error");
            object.template execute<typename TRANSITION::effect>(std::forward<ARGS>(args)...);
            this->enter(object, current.region, std::forward<ARGS>(args)...);
        }
    };

    template<class TRANSITION, class SOURCE>
    struct Object<uml::TransitionKind::internal, TRANSITION, SOURCE>
            : Object<uml::Transition::type, TRANSITION, SOURCE, typename TRANSITION::trigger> {

        using Object<uml::Transition::type, TRANSITION, SOURCE, typename TRANSITION::trigger>::Object;

        template<class CONTEXT, class CURRENT, class ...ARGS>
        void traverse(CONTEXT &object, CURRENT &current, ARGS &&...args) {
            object.template execute<typename TRANSITION::effect>(std::forward<ARGS>(args)...);
        }
    };

    template<class TRANSITION, class SOURCE>
    struct Object<uml::Transition::type, TRANSITION, SOURCE>
            : Object<uml::Transition::kind<TRANSITION>, TRANSITION, SOURCE> {
        typedef Object<uml::Transition::kind<TRANSITION>, TRANSITION, SOURCE> BaseObject;

        typedef uml::Transition::kind<TRANSITION> kind;

        bool enabled() {
            return this->source.active() && !this->target.active() && this->guard.evaluate(this->source.containingStateMachine());
        }


        using BaseObject::Object;

    };

    template<class TRANSITION, class SOURCE>
    struct Object<CompletionTransition::type, TRANSITION, SOURCE>
            : Object<uml::Transition::type, TRANSITION, SOURCE> {
        typedef Object<uml::Transition::type, TRANSITION, SOURCE> BaseObject;
        using BaseObject::Object;

        template<class CONTEXT, class CURRENT, class ...ARGS, bool ENABLED = Transition::is_enabled<TRANSITION, ARGS...>::value, typename std::enable_if<ENABLED, bool>::type= 0>
        bool execute(CONTEXT &object, CURRENT &vertex, ARGS &&...args) {
            BaseObject::traverse(object, vertex);
            return true;
        }

        template<class CONTEXT, class CURRENT, class ...ARGS, bool ENABLED = Transition::is_enabled<TRANSITION, ARGS...>::value, typename std::enable_if<!ENABLED, bool>::type= 0>
        bool execute(CONTEXT &object, CURRENT &vertex, ARGS &&...args) {
            return false;
        }


    };


//    template<class CONTEXT, class CURRENT, class EVENT>
//    bool execute(CONTEXT &object, CURRENT &vertex, Object<CompletionEvent::type, EVENT, CONTEXT> &) {
//        LOG_FN;
////            if (this->guard(object, std::forward<ARGS>(args)...)) {
//        static_cast<DerivedObject *>(this)->traverse(object, vertex);
//        return true;
////            }
////            return false;
//    }


    template<class SOURCE>
    struct Object<uml::Transition::type, uml::collection<>, SOURCE> : Object<uml::Transition::type, void, SOURCE> {
        using Object<uml::Transition::type, void, SOURCE>::Object;

        template<class TARGET>
        void get() {}
    };

    template<class TRANSITION, class SOURCE>
    struct Object<uml::Transition::type, uml::collection<TRANSITION>, SOURCE>
            : Object<uml::Collection::type, collection<TRANSITION>, SOURCE> {
        typedef Object<uml::Collection::type, collection<TRANSITION>, SOURCE>
                BaseObject;
        using BaseObject::Object;


    };


    template<class TRANSITION, class ...TRANSITIONS, class SOURCE>
    struct Object<uml::Transition::type, uml::collection<TRANSITION, TRANSITIONS...>, SOURCE>
            : Object<uml::Collection::type, uml::collection<TRANSITION, TRANSITIONS...>, SOURCE> {
        typedef Object<uml::Collection::type, uml::collection<TRANSITION, TRANSITIONS...>, SOURCE>
                BaseObject;
        using BaseObject::Object;

        template<class CONTEXT, class CURRENT, class ...ARGS, bool ENABLED = Transition::is_enabled<TRANSITION, ARGS...>::value, typename std::enable_if<!ENABLED, bool>::type= 00>
        bool execute(CONTEXT &context, CURRENT &current, ARGS &&...args) {
            return this->next.execute(context, current, std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class CURRENT, class ...ARGS, bool ENABLED = Transition::is_enabled<TRANSITION, ARGS...>::value, typename std::enable_if<ENABLED, bool>::type= 0>
        bool execute(CONTEXT &context, CURRENT &current, ARGS &&...args) {
            return BaseObject::execute(context, current, std::forward<ARGS>(args)...) ||
                   this->next.execute(context, current, std::forward<ARGS>(args)...);
        }

        template<class CONTEXT, class CURRENT, class ...REST, class ...ARGS>
        bool
        execute(CONTEXT &context, Object<uml::Psuedostate::type, uml::PsuedostateKind::fork, CURRENT, REST...> &current, ARGS &&...args) {
            return BaseObject::execute(context, current, std::forward<ARGS>(args)...) &&
                   this->next.execute(context, current, std::forward<ARGS>(args)...);
        }
    };


}

#endif //RTSM_TRANSITION_H
