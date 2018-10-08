//
// Created by gwillen on 8/20/18.
//

#ifndef RTSM_CALLEVENT_H
#define RTSM_CALLEVENT_H

#include "Object.h"

namespace rtsm {

    struct CallEvent : uml::CallEvent {
        template<class ...ARGS>
        void operator()(ARGS &&...args) {}
    };

    template<class EVENT, class CONTEXT>
    struct Object<uml::CallEvent::type, EVENT, CONTEXT> : Object<uml::Event::type, EVENT, CONTEXT> {

        typedef Object<uml::Event::type, EVENT, CONTEXT> BaseObject;

        template<class ...ARGS>
        void operator()(ARGS &&...args) {
            BaseObject::operator ()(std::forward<ARGS>(args)...);
            this->active(true);
        }

    };
}

#endif //RTSM_CALLEVENT_H
