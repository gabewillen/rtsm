//
// Created by gwillen on 8/3/18.
//

#ifndef RTSM_CONSTRAINT_H
#define RTSM_CONSTRAINT_H

#include "Element.h"

namespace rtsm {
    struct Constraint : uml::Constraint {

        template<class ...ARGS>
        bool evaluate(ARGS &&...args) {
            return true;
        }
    };

    template<class CLASSIFIER, class CONTEXT=void, class EVENT=void>
    struct constraint : Constraint {
        bool evaluate(CONTEXT &self, EVENT &event);
    };

    template<class CLASSIFIER, class CONTEXT>
    struct constraint<CLASSIFIER, CONTEXT, void> : Constraint {
        bool evaluate(CONTEXT &self);
    };

    template<class CLASSIFIER>
    struct constraint<CLASSIFIER, void, void> : Constraint {
        template<class CONTEXT>
        bool evaluate(CONTEXT &self);
    };

    template<class CONSTRAINT, class CONTEXT>
    struct Object<uml::Constraint::type, CONSTRAINT, CONTEXT> : Object<uml::Element::type, CONSTRAINT> {

        typedef CONTEXT ContextObject;
        typedef Object<uml::Element::type, CONSTRAINT> BaseObject;

        using BaseObject::Object;

        template<class ...ARGS>
        bool evaluate(ContextObject &object, ARGS &&...args) {
            return BaseObject::evaluate(static_cast<typename CONTEXT::classifier &>(object), std::forward<ARGS>(args)...);
        }
    };

    template<class CONTEXT>
    struct Object<uml::Constraint::type, void, CONTEXT> : Object<uml::Element::type, void> {
        template<class ...ARGS>
        bool evaluate(ARGS &&...args) {
            return true;
        }
    };
}

#endif //RTSM_CONSTRAINT_H
