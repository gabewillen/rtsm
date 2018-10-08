//
// Created by gwillen on 6/19/18.
//

#ifndef RTSM_CLASSIFIER_H
#define RTSM_CLASSIFIER_H

#include "Element.h"

namespace rtsm {

    using Classifier = uml::Classifier;

    template<>
    struct Object<uml::Property::type, void> : Object<uml::Element::type, void> {
    };

    template<>
    struct Object<uml::Classifier::type, void> : Object<uml::Element::type, void> {
    };




    template<class CLASSIFIER>
    struct Object<uml::Classifier::type, CLASSIFIER> : Object<uml::Element::type, CLASSIFIER> {

        typedef CLASSIFIER classifier;
        typedef Object<typename CLASSIFIER::type, CLASSIFIER> DerivedObject;
        typedef Object<uml::Property::type, typename CLASSIFIER::attribute> PropertyObject;
        typedef Object<uml::Element::type, CLASSIFIER> BaseObject;

        using BaseObject::Object;


        template<class PROPERTY, class DATATYPE>
        void setAttribute(DATATYPE &value) {
            static_cast<Object<typename PROPERTY::type, PROPERTY> &>(attribute.template get<PROPERTY>()).value = value;
        }

        template<class PROPERTY>
        inline typename PROPERTY::dataType &getAttribute() {
            return static_cast<Object<typename PROPERTY::type, PROPERTY> &>(attribute.template get<PROPERTY>()).value;
        }


        PropertyObject attribute;

    };

}

#endif //RTSM_CLASSIFIER_H
