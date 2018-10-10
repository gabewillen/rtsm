//
// Created by gwillen on 6/25/18.
//

#ifndef RTSM_STATE_H
#define RTSM_STATE_H

#include "Vertex.h"
#include "Behavior.h"

namespace rtsm {


    using State = uml::State;

    namespace state {


        template<class STATE>
        struct entry : rtsm::Behavior {

            template<class CONTEXT>
            void execute(CONTEXT &);

            template<class CONTEXT>
            void terminate(CONTEXT &);
        };

        template<class STATE>
        struct exit : rtsm::Behavior {

            template<class CONTEXT>
            void execute(CONTEXT &);

            template<class CONTEXT>
            void terminate(CONTEXT &);
        };

        template<class STATE, bool REENTRANT = true>
        struct doActivity : rtsm::Behavior {
            template<class CONTEXT>
            void execute(CONTEXT &object);

            template<class CONTEXT>
            void terminate(CONTEXT &object) {}

            static const bool isReentrant = REENTRANT;
        };
    }


    template<class SUBMACHINE>
    struct submachineState : uml::State {
        typedef SUBMACHINE submachine;
        static const bool isSubmachineState = true;

    };

    template<class STATE, class CONTAINER>
    struct Object<uml::State::type, STATE, CONTAINER, void> : Object<uml::Vertex::type, STATE, CONTAINER> {
        typedef Object<uml::Vertex::type, STATE, CONTAINER> BaseObject;
        typedef Object<uml::State::type, STATE, CONTAINER> DerivedObject;
        typedef CONTAINER ContainerObject;

        typedef Object<uml::Region::type, typename STATE::region, DerivedObject> RegionObject;

        Object(ContainerObject &object) : BaseObject(object), region(*static_cast<DerivedObject *>(this)) {}

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class REGION_TARGET = decltype(std::declval<RegionObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<!IS_BASE_TARGET, bool>::type= 0

        >
        REGION_TARGET get() {
            return region.template get<TARGET>();
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class REGION_TARGET = decltype(std::declval<RegionObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<IS_BASE_TARGET, bool>::type= 0
        >
        BASE_TARGET get() {
            return BaseObject::template get<TARGET>();
        }

        void entry(typename ContainerObject::StateMachineObject &object) {}

        void exit(typename ContainerObject::StateMachineObject &object) {}


        template<class TYPE, class EVENT, class ...REST>
        bool process(typename ContainerObject::StateMachineObject &object, Object<TYPE, EVENT, REST...> &event) {
            if (region.process(object, event)) {
                return true;
            }
            return this->outgoing.execute(object, *static_cast<DerivedObject *>(this), event);
        }

        RegionObject region;

    };

    template<class STATE, class CONTAINER, class SUBMACHINE>
    struct Object<uml::State::type, STATE, CONTAINER, SUBMACHINE> : Object<uml::Vertex::type, STATE, CONTAINER> {

        typedef CONTAINER ContainerObject;
        typedef Object<uml::Vertex::type, STATE, CONTAINER> BaseObject;
        typedef Object<uml::StateMachine::type, SUBMACHINE, typename CONTAINER::StateMachineObject> SubmachineObject;
        typedef Object<uml::State::type, STATE, CONTAINER> DerivedObject;
        typedef Object<uml::Region::type, void, DerivedObject> RegionObject;

        Object(ContainerObject &object)
                : BaseObject(object), region(static_cast<DerivedObject &>(*this)),
                  submachine(static_cast<SubmachineObject &>(object.containingStateMachine().template get<SUBMACHINE>())) {}

        void entry(typename ContainerObject::StateMachineObject &object) {
            submachine.active(true);
            submachine.region.activate(submachine);
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                 class SUBMACHINE_TARGET = decltype(std::declval<SubmachineObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<!IS_BASE_TARGET, bool>::type= 0

        >
        SUBMACHINE_TARGET get() {
            return submachine.template get<TARGET>();
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<IS_BASE_TARGET, bool>::type= 0
        >
        BASE_TARGET get() {
            return BaseObject::template get<TARGET>();
        }

        void exit(typename ContainerObject::StateMachineObject &object) {
        }

        template<class TYPE, class EVENT, class ...REST>
        bool process(typename ContainerObject::StateMachineObject &object, Object<TYPE, EVENT, REST...> &event) {
            if (submachine.region.process(submachine, event)) {
                return true;
            }
            return this->outgoing.execute(object, *static_cast<DerivedObject *>(this), event);
        }

        RegionObject region;
        SubmachineObject &submachine;

    };

    template<class STATE, class CONTAINER>
    struct Object<uml::State::type, STATE, CONTAINER> : Object<uml::State::type, STATE, CONTAINER, typename STATE::submachine> {
        typedef Object<uml::State::type, STATE, CONTAINER, typename STATE::submachine> BaseObject;
        using BaseObject::Object;
        typedef CONTAINER ContainerObject;

        void entry(typename ContainerObject::StateMachineObject &object) {
            this->active(true);
            object.template execute<typename STATE::entry>();
            object.template execute<typename STATE::doActivity>();
            BaseObject::entry(object);
        }

        void exit(typename ContainerObject::StateMachineObject &object) {
            object.template terminate<typename STATE::doActivity>();
            object.template execute<typename STATE::exit>();
            BaseObject::exit(object);
            this->active(false);
        }


        template<class TYPE, class EVENT, class ...REST, class DEFERRABLE=typename STATE::deferrableTrigger,
                bool IS_DEFERRED = index_of<DEFERRABLE, EVENT>::value >= 0, typename std::enable_if<IS_DEFERRED, bool>::type= 0>
        bool process(typename ContainerObject::StateMachineObject &object, Object<TYPE, EVENT, REST...> &event) {
            return this->active();
        }

        template<class TYPE, class EVENT, class ...REST, class DEFERRABLE=typename STATE::deferrableTrigger,
                bool IS_DEFERRED = index_of<DEFERRABLE, EVENT>::value >= 0,
                typename std::enable_if<!IS_DEFERRED, bool>::type= 0>
        bool process(typename ContainerObject::StateMachineObject &object, Object<TYPE, EVENT, REST...> &event) {
            if (this->active()) {
                return BaseObject::process(object, event);
            }
            return false;
        }

    };

    template<class CONTAINER>
    struct Object<uml::State::type, void, CONTAINER> : Object<uml::Element::type, void> {
    };

}

#endif //RTSM_STATE_H
