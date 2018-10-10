//
// Created by gwillen on 10/8/18.
//

#ifndef RTSM_FINALSTATE_H
#define RTSM_FINALSTATE_H

#include "State.h"

namespace rtsm {


    struct FinalState : uml::FinalState {

    };

    template<class CLASSIFIER, class CONTAINER>
    struct Object<uml::FinalState::type, CLASSIFIER, CONTAINER> : Object<uml::State::type, CLASSIFIER, CONTAINER, void> {

        typedef Object<uml::State::type, CLASSIFIER, CONTAINER, void> BaseObject;
        typedef CONTAINER ContainerObject;


        using BaseObject::Object;

        static_assert(std::is_void<typename CLASSIFIER::exit>::value, "A FinalState has no exit Behavior");
        static_assert(BaseObject::Outgoing::size() == 0, "A FinalState cannot have any outgoing Transitions");
        static_assert(std::is_void<typename CLASSIFIER::region>::value, "A FinalState cannot have Regions");
        static_assert(std::is_void<typename CLASSIFIER::submachine>::value, "A FinalState cannot reference a submachine");
        static_assert(std::is_void<typename CLASSIFIER::entry>::value, "A FinalState has no entry Behavior");
        static_assert(std::is_void<typename CLASSIFIER::doActivity>::value, "A FinalState has no state (doActivity) Behavior");

    };

}

#endif //RTSM_FINALSTATE_H
