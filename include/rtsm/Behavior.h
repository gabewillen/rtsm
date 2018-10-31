//
// Created by gwillen on 6/19/18.
//

#ifndef RTSM_BEHAVIOR_H
#define RTSM_BEHAVIOR_H

#if !(__arm__ || NON_CONCURRENT_REGIONS)

#include <atomic>

#endif
#include <iostream>
#include <cstring>
#include "Collection.h"
#include "Classifier.h"
#include "Property.h"
#include "Event.h"
#include "TimeEvent.h"
#include "ChangeEvent.h"
#include "CompletionEvent.h"

namespace rtsm {

//    template<class CONTEXT>
    struct Behavior : uml::StateMachine {

        template<class ...>
        struct prioritize;

        template<class ...EVENTS>
        struct prioritize<rtsm::collection<EVENTS...>> {
            typedef rtsm::collection<EVENTS...> type;
        };


        typedef uml::Behavior::type type;

        template<class CONTEXT, class ...ARGS>
        void execute(CONTEXT &object, ARGS &&...args) {
        }

        template<class CONTEXT>
        void terminate(CONTEXT &object) {
            object.terminate(*this);
        }

    private:
        Behavior *context;

    };

    namespace {


        template<class ...>
        struct context;

        template<class OBJECT>
        struct context<OBJECT> {
            typedef OBJECT type;
        };

        template<class OBJECT, class CONTEXT>
        struct context<OBJECT, CONTEXT> {
            typedef CONTEXT type;
        };
    }

    template<class TYPE, class CONTEXT_CLASSIFIER, class ...REST>
    struct Object<uml::Behavior::type, void, Object<TYPE, CONTEXT_CLASSIFIER, REST...>>
            : Object<uml::Classifier::type, void> {
        typedef Object<TYPE, CONTEXT_CLASSIFIER, REST...> ContextObject;


        template<class CONTEXT, class ...ARGS>
        void execute(CONTEXT &, ARGS &&...args) {}

        void terminate() {}
    };

    template<class CLASSIFIER, class ...REST>
    struct Object<uml::Behavior::type, CLASSIFIER, REST...> : Object<uml::Classifier::type, CLASSIFIER> {
        typedef typename uml::Behavior::all<CLASSIFIER>::type OwnedBehavior;
        typedef Object<uml::Behavior::type, OwnedBehavior, Object> OwnedBehaviorObject;
        typedef Object<uml::Classifier::type, CLASSIFIER> BaseObject;
        typedef Object<typename CLASSIFIER::type, CLASSIFIER, REST...> DerivedObject;
        typedef typename context<Object, REST...>::type ContextObject;

        template<class ...> friend
        class Object;


        typedef typename uml::Event::all<Object>::type events;

        typedef typename CLASSIFIER::template prioritize<events>::type EventPool;


        typedef Object<uml::Event::type, EventPool, DerivedObject> EventPoolObject;

        template<class ...ARGS>
        Object(ARGS &&...args)
                : BaseObject(std::forward<ARGS>(args)...), context(*this), ownedBehavior(*this), pool(static_cast<DerivedObject &>(*this)),
                  processing(0) {
        }

        template<class ContextObject, class ...ARGS>
        Object(ContextObject &object, ARGS &&...args)
                : BaseObject(object, std::forward<ARGS>(args)...), context(object), ownedBehavior(*this),
                  pool(static_cast<DerivedObject &>(*this)),
                  processing(0) {
        }

        template<class EVENT, class ...ARGS>
        inline void call(ARGS &&...args) {
            return static_cast<Object<typename EVENT::type, EVENT, DerivedObject> &>(pool.template get<EVENT>())(std::forward<ARGS>(args)...);
        }


        template<class EVENT, bool IS_SAME = !std::is_same<ContextObject, Object>::value, class POOL=EventPool, int INDEX = uml::Collection::index_of<POOL, EVENT>::value, int COUNT = uml::Collection::count<POOL, EVENT>::value, typename std::enable_if<
                (INDEX >= 0 && !IS_SAME), bool>::type= 0>
        void dispatch(EVENT &event) {
            Object < typename EVENT::type, EVENT, DerivedObject > &object = this->pool.template get<INDEX>();
            if (object.active()) {
                this->template dispatch<EVENT, IS_SAME, POOL, uml::Collection::index_of<POOL, EVENT, INDEX + 1>::value>(event);
                return;
            }
            bool isProcessing = this->processing > 0;
            static_cast<EVENT &>(object) = event;
//            memcpy(static_cast<void *>(&static_cast<EVENT &>(object)), static_cast<void *>(&event), sizeof(EVENT));
            object.active(true);
            this->processing++;
            while (!isProcessing && this->processing > 0) {
                this->process();
            }
        }


        template<class EVENT, bool IS_SAME = !std::is_same<ContextObject, Object>::value, class POOL=EventPool, int INDEX = uml::Collection::index_of<POOL, EVENT>::value, int COUNT = uml::Collection::count<POOL, EVENT>::value, typename std::enable_if<(
                INDEX < 0 && !IS_SAME), bool>::type= 0>
        void dispatch(EVENT &event) {
            ownedBehavior.template dispatch<EVENT, false>(event);
        }

        template<class EVENT, bool PROPOGATE = !std::is_same<ContextObject, Object>::value, typename std::enable_if<PROPOGATE, bool>::type= 0>
        void dispatch(EVENT &event) {
            context.dispatch(event);
        }

        template<class EVENT>
        void dispatch(EVENT &&event) {
            dispatch(static_cast<EVENT &>(event));
        }


        void execute() {
            this->active(true);
            execute(ownedBehavior);
        }

        template<class CONTEXT, class ...ARGS>
        void execute(CONTEXT &object, ARGS &&...args) {
            this->active(true);
            BaseObject::execute(object, std::forward<ARGS>(args)...);
            if (this->active()) {
                this->execute(this->ownedBehavior);
            }
        }


        template<class BEHAVIOR, class ...ARGS,
                int INDEX = uml::Collection::index_of<OwnedBehavior, BEHAVIOR>::value,
                typename std::enable_if<(INDEX >= 0), bool>::type= 0
        >
        void execute(ARGS &&...args) {
            ownedBehavior.template get<INDEX>().execute(*this, std::forward<ARGS>(args)...);
        }

        template<class BEHAVIOR, class ...ARGS,
                int INDEX = uml::Collection::index_of<OwnedBehavior, BEHAVIOR>::value,
                typename std::enable_if<(INDEX < 0), bool>::type= 0
        >
        void execute(ARGS &&...args) {}

        template<class BEHAVIOR, class ...ARGS,
                int INDEX = uml::Collection::index_of<OwnedBehavior, BEHAVIOR>::value,
                typename std::enable_if<(INDEX >= 0), bool>::type= 0
        >
        void terminate() {
            ownedBehavior.template get<INDEX>().terminate(*this);
        }

        template<class BEHAVIOR, class ...ARGS,
                int INDEX = uml::Collection::index_of<OwnedBehavior, BEHAVIOR>::value,
                typename std::enable_if<(INDEX < 0), bool>::type= 0
        >
        void terminate() {}


        void terminate() {
            this->active(false);
        }

        template<class CONTEXT>
        void terminate(CONTEXT &object) {
            if (this->active()) {
                BaseObject::terminate(object);
            }
            this->active(false);
        }

        void terminate(Behavior &object) {
            terminate(ownedBehavior, object);
        }


        template<class TARGET,
                 class BEHAVIOR_TARGET=decltype(std::declval<OwnedBehaviorObject>().template get<TARGET>()),
                 class BASE_TARGET=decltype(std::declval<BaseObject>().template get<TARGET>()),
                bool IS_BEHAVIOR_TARGET = std::is_same<BEHAVIOR_TARGET, TARGET &>::value,
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<IS_BEHAVIOR_TARGET && !IS_BASE_TARGET, bool>::type= 0
        >
        inline BEHAVIOR_TARGET get() {
            return ownedBehavior.template get<TARGET>();
        }

        template<class TARGET,
                 class BEHAVIOR_TARGET=decltype(std::declval<OwnedBehaviorObject>().template get<TARGET>()),
                 class BASE_TARGET=decltype(std::declval<BaseObject>().template get<TARGET>()),
                bool IS_BEHAVIOR_TARGET = std::is_same<BEHAVIOR_TARGET, TARGET &>::value,
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<!IS_BEHAVIOR_TARGET && IS_BASE_TARGET, bool>::type= 0
        >
        inline BASE_TARGET get() {
            return BaseObject::template get<TARGET>();
        }

        using BaseObject::get;
        ContextObject &context;

    private:
        OwnedBehaviorObject ownedBehavior;
        EventPoolObject pool;

#if !(__arm__ || NON_CONCURRENT_REGIONS)
        std::atomic<int> processing;
#else
        int processing;
#endif


        void process() {
            processing = 1;
            process(pool);
            processing--;
        }

        template<class TYPE, class EVENT>
        inline bool process(Object<TYPE, EVENT, DerivedObject> &event) {
            if (event.active()) {
                if (static_cast<DerivedObject *>(this)->process(event)) {
                    event.active(false);
                    return true;
                }
                event.active(false);
            }
            return false;
        }


        template<class EVENT>
        inline bool process(Object<uml::Event::type, uml::collection<EVENT>, DerivedObject>
                            &object) {
            return process(static_cast<Object<typename EVENT::type, EVENT, DerivedObject> &>(object));
        }


        template<class EVENT, class ...EVENTS>
        inline bool process(Object<uml::Event::type, uml::collection<EVENT, EVENTS...>, DerivedObject>
                            &object) {
            if (!process(static_cast<Object<typename EVENT::type, EVENT, DerivedObject> &>(object))) {
                return process(object.next);
            }
            return true;
        }


        template<class ITEM,
                 class OBJECT=Object<typename ITEM::type, ITEM, Object>,
                bool IS_REENTRANT = ITEM::isReentrant, typename std::enable_if<IS_REENTRANT, bool>::type= 0>
        void execute(Object<uml::Behavior::type, uml::collection<ITEM>, Object> &behaviors) {
            OBJECT &object = static_cast<OBJECT &>(behaviors);
            if (object.active()) {
                object.execute(*this);
            }
        }

        template<class ITEM,
                 class OBJECT=Object<typename ITEM::type, ITEM, Object>,
                bool IS_REENTRANT = ITEM::isReentrant, typename std::enable_if<!IS_REENTRANT, bool>::type= 0>
        void execute(Object<uml::Behavior::type, uml::collection<ITEM>, Object> &behaviors) {}

        template<class ITEM, class ...ITEMS,
                 class OBJECT=Object<typename ITEM::type, ITEM, Object>,
                bool IS_REENTRANT = ITEM::isReentrant, typename std::enable_if<IS_REENTRANT, bool>::type= 0>
        void execute(Object<uml::Behavior::type, uml::collection<ITEM, ITEMS...>, Object> &behaviors) {
            OBJECT &object = static_cast<OBJECT &>(behaviors);
            if (object.active()) {
                object.execute(*this);
            }
            execute(behaviors.next);
        }

        template<class ITEM, class ...ITEMS,
                 class OBJECT=Object<typename ITEM::type, ITEM, Object>,
                bool IS_REENTRANT = ITEM::isReentrant, typename std::enable_if<!IS_REENTRANT, bool>::type= 0>
        void execute(Object<uml::Behavior::type, uml::collection<ITEM, ITEMS...>, Object> &behaviors) {
            execute(behaviors.next);
        }

        void execute(Object<uml::Behavior::type, uml::collection<>, Object> &behaviors) {
        }

        template<class ITEM>
        void terminate(Object<uml::Behavior::type, uml::collection<ITEM>, Object> &behavior, Behavior &object) {
            if (static_cast<void *>(&behavior) == static_cast<void *>(&object)) {
                behavior.terminate();
            }
        }

        template<class ITEM, class ...ITEMS>
        void terminate(Object<uml::Behavior::type, uml::collection<ITEM, ITEMS...>, Object> &behavior, Behavior &object) {
            if (static_cast<void *>(&behavior) == static_cast<void *>(&object)) {
                behavior.terminate();
            } else {
                terminate(behavior.next, object);
            }
        }


    };

//    template<class CLASSIFIER, class TYPE, class CONTEXT_CLASSIFIER, class ...REST>
//    struct Object<uml::Behavior::type, CLASSIFIER, Object<TYPE, CONTEXT_CLASSIFIER, REST...>>
//            : Object<uml::Behavior::type, CLASSIFIER> {
//        typedef Object<TYPE, CONTEXT_CLASSIFIER, REST...> ContextObject;
//        typedef Object<uml::Behavior::type, CLASSIFIER> BaseObject;
//        typedef Object<typename CLASSIFIER::type, CLASSIFIER, ContextObject> DerivedObject;
//
//        template<class ...ARGS>
//        Object(ContextObject &object, ARGS &&...args)
//                : BaseObject(object, std::forward<ARGS>(args)...), context(object) {}
//
//
//        template<class EVENT>
//        void dispatch(EVENT &event) {
//            context.dispatch(event);
//        }
//
//
//        ContextObject &context;
//
//    };

    template<class TYPE, class CONTEXT_CLASSIFIER, class ...REST>
    struct Object<uml::Behavior::type, uml::collection<>, Object<TYPE, CONTEXT_CLASSIFIER, REST...>> {
        template<class ...ARGS>
        Object(ARGS &&...args) {}

        template<class EVENT, bool>
        void dispatch(EVENT &event) {}
    };

    template<class ITEM, class TYPE, class CONTEXT_CLASSIFIER, class ...REST>
    struct Object<uml::Behavior::type, uml::collection<ITEM>, Object<TYPE, CONTEXT_CLASSIFIER, REST...>>
            : Object<uml::Collection::type, uml::collection<ITEM>, Object<TYPE, CONTEXT_CLASSIFIER, REST...>> {
        typedef Object<uml::Collection::type, uml::collection<ITEM>, Object<TYPE, CONTEXT_CLASSIFIER, REST...>> BaseObject;

        using BaseObject::Object;
    };

    template<class ITEM, class ...ITEMS, class TYPE, class CONTEXT_CLASSIFIER, class ...REST>
    struct Object<Behavior::type, uml::collection<ITEM, ITEMS...>, Object<TYPE, CONTEXT_CLASSIFIER, REST...>>
            : Object<uml::Collection::type, uml::collection<ITEM, ITEMS...>, Object<TYPE, CONTEXT_CLASSIFIER, REST...>> {
        typedef Object<uml::Collection::type, uml::collection<ITEM, ITEMS...>, Object<TYPE, CONTEXT_CLASSIFIER, REST...>> BaseObject;

        using BaseObject::Object;

        template<class EVENT, bool PROPOGATE>
        void dispatch(EVENT &event) {
            BaseObject::template dispatch<EVENT, PROPOGATE>(event);
            this->next.template dispatch<EVENT, PROPOGATE>(event);
        }

    };
}

#endif //RTSM_BEHAVIOR_H
