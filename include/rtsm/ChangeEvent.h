//
// Created by gwillen on 8/16/18.
//

#ifndef RTSM_CHANGEEVENT_H
#define RTSM_CHANGEEVENT_H

#include "Object.h"


namespace rtsm {

    template<class EXPRESSION>
    struct ChangeEvent : uml::ChangeEvent {

        typedef EXPRESSION changeExpression;
    };



    template<class EVENT, class CONTEXT>
    struct Object<uml::ChangeEvent::type, EVENT, CONTEXT> : Object<uml::Event::type, EVENT, CONTEXT> {
        typedef Object<uml::Event::type, EVENT, CONTEXT> BaseObject;
        typedef Object<typename uml::Element::type_of<typename EVENT::changeExpression, uml::Constraint>::type, typename EVENT::changeExpression, CONTEXT> ChangeExpressionObject;

        Object(CONTEXT &object): BaseObject(object), changeExpression(object) {}


        ChangeExpressionObject changeExpression;

        bool active() {
            bool active;
            BaseObject::active((active = changeExpression.evaluate(this->transition.source.containingStateMachine()) || BaseObject::active()));
            return active;
        }

        using BaseObject::active;
//        decltype(PropertyObject::value) &value;

    private:

//        decltype(PropertyObject::value) cachedValue;
    };
}

#endif //RTSM_CHANGEEVENT_H
