//
// Created by gwillen on 8/16/18.
//

#ifndef RTSM_PROPERTY_H
#define RTSM_PROPERTY_H

#include "Collection.h"

namespace rtsm {


    template<class DATATYPE, std::size_t ID = 0>
    struct Property : uml::Property {

        typedef DATATYPE dataType;
        static constexpr DATATYPE defaultValue = DATATYPE(0);
    };

    template<std::size_t ID>
    struct Property<std::string, ID> : uml::Property {
        typedef std::string dataType;
        static const std::string defaultValue;
    };

    template<std::size_t ID>
    const std::string Property<std::string, ID>::defaultValue;

    template<class PROPERTY>
    struct PropertyChangeEvent : uml::ChangeEvent {


        struct changeExpression : uml::Constraint {


            template<class CONTEXT>
            changeExpression(CONTEXT &object) : value(object.template getAttribute<PROPERTY>()), cachedValue(value) {}

            template<class CONTEXT>
            bool evaluate(CONTEXT &object) {
                if (value != cachedValue) {
                    cachedValue = value;
                    return true;
                }
                return false;
            }


            typename PROPERTY::dataType &value;
            typename PROPERTY::dataType cachedValue;

        };

    };


    template<class PROPERTY>
    struct Object<uml::Property::type, PROPERTY> : Object<uml::Element::type, PROPERTY> {

        Object(): value(PROPERTY::defaultValue) {}

        typename PROPERTY::dataType value;
    };

    template<class PROPERTY>
    struct Object<uml::Property::type, uml::collection<PROPERTY>> : Object<uml::Collection::type, uml::collection<PROPERTY>> {
        typedef Object<uml::Collection::type, uml::collection<PROPERTY>> BaseObject;
        using BaseObject::Object;


    };

    template<class PROPERTY, class ...PROPERTIES>
    struct Object<uml::Property::type, uml::collection<PROPERTY, PROPERTIES...>>
            : Object<uml::Collection::type, uml::collection<PROPERTY, PROPERTIES...>> {
        typedef Object<uml::Collection::type, uml::collection<PROPERTY, PROPERTIES...>> BaseObject;
        using BaseObject::Object;
    };

}


#define Property(TYPE) rtsm::Property<TYPE, __LINE__>

#endif //RTSM_PROPERTY_H
