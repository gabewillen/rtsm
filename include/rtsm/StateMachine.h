//
// Created by gwillen on 6/19/18.
//

#ifndef RTSM_STATEMACHINE_H
#define RTSM_STATEMACHINE_H

#include "uml.h"
#include "Behavior.h"
#include "Region.h"
#include "Transition.h"
#include "State.h"
#include "Psuedostate.h"
#include <type_traits>

namespace rtsm {


    namespace {
        template<class ...>
        struct prioritize;

        template<>
        struct prioritize<uml::collection<>> {
            typedef uml::collection<> type;
        };


        template<class EVENT, class ...EVENTS>
        struct prioritize<uml::collection<EVENT, EVENTS...>> {

            typedef typename prioritize<uml::collection<EVENTS...>>::type events;
            typedef typename std::conditional<std::is_base_of<CompletionEvent, EVENT>::value, typename uml::Collection::concat<uml::collection<EVENT>, events>::type, typename uml::Collection::concat<events, uml::collection<EVENT>>::type>::type type;
        };
    }

    /*!
     *
     * @tparam CONTEXT
     */
    template<class CONTEXT>
    struct StateMachine : Behavior {
        typedef uml::StateMachine::type type;
        typedef Object<uml::StateMachine::type, CONTEXT> DerivedObject;

        template<class ...ARGS>
        using prioritize = prioritize<ARGS...>;

        template<class TARGET>
        TARGET &get() {
            return self.template get<TARGET>();
        }


        template<class PROPERTY, class DATATYPE>
        void setAttribute(DATATYPE &&value) {
            self.template setAttribute<PROPERTY>(value);
        }

        template<class PROPERTY>
        inline typename PROPERTY::dataType getAttribute() {
            return self.template getAttribute<PROPERTY>();
        }


        template<class CLASSIFIER, class OBJECT=typename declobject<DerivedObject, CLASSIFIER>::type>
        bool inState() {
            CLASSIFIER &object = self.template get<CLASSIFIER>();
            return static_cast<OBJECT &>(object).active();
        }

        template<class EVENT>
        void dispatch(EVENT &&event) {
            self.dispatch(static_cast<EVENT &>(event));
        }

        template<class EVENT>
        void dispatch() {
            EVENT temp;
            self.dispatch(temp);
        }

        template<class CLASSIFIER>
        void terminate(CLASSIFIER &object) {
            self.terminate(object);
        }

        static const bool isReentrant = true;

    protected:
        /**
         * StateMachine is abstract
         */
        StateMachine() : self(static_cast<DerivedObject &>(*this)) {}


    private:
        DerivedObject &self;

    };



    template<class CLASSIFIER, class ...REST>
    struct Object<uml::StateMachine::type, CLASSIFIER, REST...> : Object<uml::Behavior::type, CLASSIFIER, REST...> {
        typedef Object<Behavior::type, CLASSIFIER, REST...> BaseObject;
        typedef Object<uml::Region::type, typename CLASSIFIER::region, Object> RegionObject;

        typedef typename uml::Transition::all<Object>::type transitions;



//        typedef Object<uml::Event::type, EventPool, Object> EventPoolObject;

        Object() : region(*this) {
            this->active(true);
            region.activate(*this);
        }

        // State
        template<class CONTEXT>
        Object(CONTEXT &object) : BaseObject(object), region(*this) {}

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class REGION_TARGET = decltype(std::declval<RegionObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                bool IS_REGION_TARGET = std::is_same<REGION_TARGET, TARGET &>::value,
                typename std::enable_if<IS_BASE_TARGET, bool>::type= 0
        >
        inline BASE_TARGET get() {
            return BaseObject::template get<TARGET>();
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class REGION_TARGET = decltype(std::declval<RegionObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                bool IS_REGION_TARGET = std::is_same<REGION_TARGET, TARGET &>::value,
                typename std::enable_if<!IS_BASE_TARGET, bool>::type= 0
        >
        inline REGION_TARGET get() {
            return region.template get<TARGET>();
        }

        void step() {
            BaseObject::process();
            BaseObject::execute();
        }

        void execute() {
            while (this->active()) {
                step();
            }
        }

        template<class CONTEXT>
        void execute(CONTEXT &object) {
            BaseObject::process();
            BaseObject::execute(object);
        }

        template<class TYPE, class EVENT>
        bool process(Object<TYPE, EVENT, Object> &object) {
            return region.process(*this, object);
        }



//        using BaseObject::process;
        using BaseObject::execute;

        RegionObject region;




    };

//    template<class CLASSIFIER, class CONTEXT>
//    struct Object<uml::StateMachine::type, CLASSIFIER, CONTEXT>
//            : Object<uml::StateMachine::type, CLASSIFIER> {
//
//        typedef Object<uml::StateMachine::type, CLASSIFIER> BaseObject;
//
//        Object(CONTEXT &object): BaseObject(object), context(object) {}
//
////        template<class EVENT>
////        void dispatch(EVENT &event) {
////            context.dispatch(event);
////        }
////
//        void execute(CONTEXT &object) {
//            this->Object<uml::Behavior::type, CLASSIFIER>::process();
//            BaseObject::execute(object);
//        }
//
////        CONTEXT &context;
//
//    };

    template<>
    struct Object<uml::StateMachine::type, void> : Object<uml::Classifier::type, void> {
    };

}

#endif //RTSM_STATEMACHINE_H
