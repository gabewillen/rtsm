//
// Created by gwillen on 6/20/18.
//

#ifndef RTSM_COLLECTION_H
#define RTSM_COLLECTION_H

#include "Element.h"
#include "utils.h"
#include <tuple>

namespace rtsm {


    template<class ...ITEMS>
    using collection = uml::collection<ITEMS...>;


    template<class ...REST>
    struct Object<uml::Collection::type, collection<>, REST...> : Object<uml::Element::type, void> {};


    template<class ITEM, class ...REST>
    struct Object<uml::Collection::type, uml::collection<ITEM>, REST...> : Object<typename ITEM::type, ITEM, REST...> {
        typedef Object<typename ITEM::type, ITEM, REST...> BaseObject;

        using BaseObject::Object;
//        typedef uml::Collection::type type;

        template<int INDEX = 0, int COUNT = 0, std::size_t SIZE = 1, class CLASSIFIER=ITEM, bool ENABLED =
        INDEX == (SIZE - 1), typename std::enable_if<
                ENABLED, bool>::type= 0>
        Object<typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER, REST...> &get() {
            return *this;
        }

        using BaseObject::get;
//        template<class TARGET>
//        decltype(std::declval<BaseObject>().template get<TARGET>()) &get() {
//            return BaseObject::template get<TARGET>();
//        }

    };


    template<class ITEM, class ...ITEMS, class ...REST>
    struct Object<uml::Collection::type, uml::collection<ITEM, ITEMS...>, REST...>
            : Object<typename ITEM::type, ITEM, REST...> {

//        typedef uml::Collection::type type;
        typedef Object<typename ITEM::type, ITEM, REST...> BaseObject;
        typedef collection<ITEM, ITEMS...> classifiers;
        typedef Object<typename uml::Collection::base<ITEM>::type, collection<ITEMS...>, REST...> NextObject;

        template<class ...ARGS>
        Object(ARGS &&...args): BaseObject(std::forward<ARGS>(args)...), next(std::forward<ARGS>(args)...) {}

        template<int INDEX = 0, int COUNT = 0, std::size_t SIZE = classifiers::size(), class CLASSIFIER=typename uml::Collection::classifier<collection<ITEM, ITEMS...>, INDEX>::type, bool ENABLE =
        INDEX == COUNT, typename std::enable_if<
                INDEX == COUNT, bool>::type= 0>
        Object<typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER, REST...> &get() {
            return *this;
        }

        template<int INDEX = 0, int COUNT = 0, std::size_t SIZE = classifiers::size(), class CLASSIFIER=typename uml::Collection::classifier<collection<ITEM, ITEMS...>, INDEX>::type, typename std::enable_if<
                (INDEX != COUNT), bool>::type= 0>
        Object<typename uml::Element::type_of<CLASSIFIER>::type, CLASSIFIER, REST...> &get() {
            return next.template get<INDEX, COUNT + 1, SIZE, CLASSIFIER>();
        }


        template<class TARGET, class CLASSIFIER= decltype(std::declval<BaseObject>().template get<TARGET>()),
                bool IS_SAME = std::is_same<CLASSIFIER, TARGET &>::value, typename std::enable_if<IS_SAME, bool>::type= 0>
        inline CLASSIFIER get() {
            return BaseObject::template get<TARGET>();
        }

        template<class TARGET, class CLASSIFIER = decltype(std::declval<BaseObject>().template get<TARGET>()),
                bool IS_SAME = std::is_same<CLASSIFIER, TARGET &>::value, typename std::enable_if<!IS_SAME, bool>::type= 0>
        inline decltype(std::declval<NextObject>().template get<TARGET>()) get() {
            return next.template get<TARGET>();
        }

        using BaseObject::active;

        bool active() {
            return BaseObject::active() || next.active();
        }


        NextObject next;
    };


}

#endif //RTSM_COLLECTION_H
