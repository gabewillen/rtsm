//
// Created by gwillen on 6/25/18.
//

#ifndef RTSM_VERTEX_H
#define RTSM_VERTEX_H

#include "Collection.h"
//#include "Region.h"

namespace rtsm {


    using Vertex = uml::Vertex;

    template<class REGION, class ...REST>
    struct Object<uml::Vertex::type, void, Object<uml::Region::type, REGION, REST...>> : Object<uml::Element::type, void> {
        typedef Object<uml::Region::type, REGION, REST...> ContainerObject;

        Object(ContainerObject &) {}

        template<class ...ARGS>
        bool process(typename ContainerObject::StateMachineObject &, ARGS &&...args) { return false; }
    };

    template<class CLASSIFIER, class CONTAINER>
    struct Object<uml::Vertex::type, CLASSIFIER, CONTAINER>
            : Object<uml::Element::type, CLASSIFIER> {
        typedef Object<uml::Element::type, CLASSIFIER> BaseObject;
        typedef CONTAINER ContainerObject;
        typedef Object<typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER, ContainerObject> DerivedObject;

        template<class TRANSITION>
        struct is_source {
            static const bool value = std::is_same<typename TRANSITION::source, CLASSIFIER>::value;
        };

        template<class ...>
        struct is_duplicate;

        template<class ...OUTGOING, class TRANSITION>
        struct is_duplicate<uml::collection<OUTGOING...>, TRANSITION> {


            static const bool value = uml::Collection::count<uml::collection<OUTGOING...>, TRANSITION>::value < 1;
        };


        typedef typename uml::Collection::select<typename ContainerObject::StateMachineObject::transitions, is_source>::type Outgoing;
        typedef Object<uml::Transition::type, Outgoing, DerivedObject> OutgoingObject;


        Object(ContainerObject &object) : container(object), outgoing(static_cast<DerivedObject &>(*this)) {}

        template<class EVENT>
        inline bool process(typename ContainerObject::StateMachineObject &object, EVENT &event) {
            return false;
        }

        typename ContainerObject::StateMachineObject &containingStateMachine() {
            return container.containingStateMachine();
        }


        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<!IS_BASE_TARGET, bool>::type= 0
        >
        decltype(std::declval<OutgoingObject>().template get<TARGET>()) get() {
            return outgoing.template get<TARGET>();
        }

        template<class TARGET,
                 class BASE_TARGET = decltype(std::declval<BaseObject>().template get<TARGET>()),
                bool IS_BASE_TARGET = std::is_same<BASE_TARGET, TARGET &>::value,
                typename std::enable_if<IS_BASE_TARGET, bool>::type= 0
        >
        BASE_TARGET get() {
            return BaseObject::template get<TARGET>();
        }

        ContainerObject &container;
        OutgoingObject outgoing;

    };

    template<class ITEM, class CONTAINER>
    struct Object<uml::Vertex::type, uml::collection<
            ITEM>, CONTAINER> : Object<uml::Collection::type,
            uml::collection<ITEM>, CONTAINER> {
        using Object<uml::Collection::type, uml::collection<ITEM>, CONTAINER>
        ::Object;
    };


    template<class ITEM, class ...ITEMS, class CONTAINER>
    struct Object<uml::Vertex::type, uml::collection<ITEM, ITEMS...>, CONTAINER>
            : Object<uml::Collection::type,
                    uml::collection<ITEM, ITEMS...>, CONTAINER> {
        typedef Object<uml::Collection::type, uml::collection<ITEM, ITEMS...>, CONTAINER>
                BaseObject;
        using BaseObject::Object;

        template<class EVENT>
        bool process(typename BaseObject::ContainerObject::StateMachineObject &object, EVENT &event) {
            return BaseObject::process(object, event) || this->next.process(object, event);
        }

    };


}


#endif //RTSM_VERTEX_H
