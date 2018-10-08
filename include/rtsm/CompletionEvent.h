//
// Created by gwillen on 7/23/18.
//

#ifndef RTSM_COMPLETIONEVENT_H
#define RTSM_COMPLETIONEVENT_H

#include "Event.h"

namespace rtsm {


    struct CompletionEvent : uml::Event {
        typedef uml::Type<CompletionEvent, uml::Event> type;

    };


    template<class CLASSIFIER, class TYPE, class CONTEXT, class ...REST>
    struct Object<CompletionEvent::type, CLASSIFIER, Object<TYPE, CONTEXT, REST...>> : Object<uml::Event::type, CLASSIFIER, Object<TYPE, CONTEXT, REST...>> {
        typedef Object<TYPE, CONTEXT, REST...> ContextObject;

        typedef Object<uml::Event::type, CLASSIFIER, ContextObject> BaseObject;
        

        typedef Object<typename uml::Element::type_of<typename BaseObject::Transition::source::doActivity>::type, typename BaseObject::Transition::source::doActivity, ContextObject> DoActivityObject;


        Object(ContextObject &object) : BaseObject(object), context(object), target(static_cast<typename BaseObject::TargetObject &>(object.template get<typename BaseObject::Transition::target>())) {}

        template<class DO_ACTIVITY=typename BaseObject::Transition::source::doActivity, typename std::enable_if<std::is_void<DO_ACTIVITY>::value, bool>::type= 0>
        bool active() {
            BaseObject::active(this->transition.source.active() && !target.active() &&
                               this->transition.guard.evaluate(this->transition.source.containingStateMachine()));
            return BaseObject::active();
        }

        template<class DO_ACTIVITY=typename BaseObject::Transition::source::doActivity, typename std::enable_if<!std::is_void<DO_ACTIVITY>::value, bool>::type= 0>
        bool active() {
            DoActivityObject &doActivity = static_cast<DoActivityObject &>(context.template get<typename BaseObject::SourceObject::doActivity>());
            BaseObject::active(this->transition.source.active() && !doActivity.active() && !target.active() &&
                               this->transition.guard.evaluate(this->transition.source.containingStateMachine()));
            return BaseObject::active();
        }

        inline void active(bool value) {
            BaseObject::active(value);
        }

        ContextObject &context;
        typename BaseObject::TargetObject &target;
    };
}

#endif //RTSM_COMPLETIONEVENT_H
