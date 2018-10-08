//
// Created by gwillen on 8/26/18.
//

#ifndef RTSM_TRIGGER_H
#define RTSM_TRIGGER_H

#include "Object.h"

namespace rtsm {

    using Trigger = uml::Trigger;


    template<class EVENT, class PORT=void>
    struct trigger : Trigger {
        typedef EVENT event;
        typedef PORT port;
    };

}

#endif //RTSM_TRIGGER_H
