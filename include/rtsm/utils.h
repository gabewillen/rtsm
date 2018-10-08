//
// Created by gwillen on 6/21/18.
//

#ifndef RTSM_UTILS_H
#define RTSM_UTILS_H

#include "uml.h"

namespace rtsm {

    template<template<class ...> class EXPR, class ...ARGS>
    struct partial {
        template<class ..._ARGS>
        using type = EXPR<ARGS..., _ARGS...>;
    };
    template<class ...>
    struct concat;

    template<class ...COLLECTION_1_ITEMS, class ...COLLECTION_2_ITEMS>
    struct concat<uml::collection<COLLECTION_1_ITEMS...>, uml::collection<COLLECTION_2_ITEMS...>> {
        typedef uml::collection<COLLECTION_1_ITEMS..., COLLECTION_2_ITEMS...> type;
    };


    template<class ...COLLECTION_1_ITEMS, class ...COLLECTION_2_ITEMS, class ...COLLECTIONS>
    struct concat<uml::collection<COLLECTION_1_ITEMS...>, uml::collection<COLLECTION_2_ITEMS...>, COLLECTIONS...> {
        typedef typename concat<uml::collection<COLLECTION_1_ITEMS..., COLLECTION_2_ITEMS...>, COLLECTIONS...>
        ::type type;
    };


    template<class ...ITEMS>
    struct concat<uml::collection<ITEMS...>, void> {
        typedef uml::collection<ITEMS...> type;
    };

    template<class ...ITEMS>
    struct concat<void, uml::collection<ITEMS...>> {
        typedef uml::collection<ITEMS...> type;
    };


    template<class, template<class ...> class>
    struct map;

    template<class ITEM, template<class ...> class EXPR>
    struct map<uml::collection<ITEM>, EXPR> {
        typedef uml::collection<typename EXPR<ITEM>::type> type;

    };

    template<class ITEM, class ...ITEMS, template<class ...> class EXPR>
    struct map<uml::collection<ITEM, ITEMS...>, EXPR>
            : concat<typename map<uml::collection<ITEM>, EXPR>::type, typename map<uml::collection<ITEMS...>, EXPR>::type> {
    };

    template<template<class ...> class EXPR>
    struct map<uml::collection<>, EXPR> {
        typedef uml::collection<> type;
    };

    template<class, std::size_t>
    struct item;

    template<class ...ITEMS, std::size_t INDEX>
    struct item<uml::collection<ITEMS...>, INDEX> : std::tuple_element<INDEX, std::tuple<ITEMS...>> {
    };

    template<std::size_t INDEX>
    struct item<uml::collection<>, INDEX> {
        typedef void type;
    };


    template<class, template<class ...> class>
    struct filter;

    template<template<class ...> class EXPR>
    struct filter<uml::collection<>, EXPR> {
        typedef uml::collection<> type;
    };


    template<class ITEM, template<class ...> class EXPR>
    struct filter<uml::collection<ITEM>, EXPR>
            : std::conditional<EXPR<ITEM>::value, uml::collection<ITEM>, uml::collection<>> {
    };

    template<class ITEM, class ...ITEMS, template<class ...> class EXPR>
    struct filter<uml::collection<ITEM, ITEMS...>, EXPR>
            : concat<typename filter<uml::collection<ITEM>, EXPR>::type,
                    typename filter<uml::collection<ITEMS...>,
                            EXPR>::type> {
    };

    template<template<class ...> class EXPR>
    struct filter<void, EXPR> {
        typedef uml::collection<> type;
    };

    template<class, class, int= 0, int= 0>
    struct index_of;


    template<class ITEM, class ...ITEMS, class CLASSIFIER>
    struct index_of<uml::collection<ITEM, ITEMS...>, CLASSIFIER> {
        static const int value = std::is_same<ITEM, CLASSIFIER>::value ? 0
                                                                       : index_of<uml::collection<ITEMS...>, CLASSIFIER, 0, 1>::value;

    };

    template<class ITEM, class ...ITEMS, class CLASSIFIER, int START, int INDEX>
    struct index_of<uml::collection<ITEM, ITEMS...>, CLASSIFIER, START, INDEX> {
        static const int value = INDEX > START && std::is_same<CLASSIFIER, ITEM>::value ? INDEX
                                                                                        : index_of<uml::collection<ITEMS...>, CLASSIFIER, START,
                        INDEX + 1>::value;
    };


    template<class CLASSIFIER, int START, int INDEX>
    struct index_of<uml::collection<>, CLASSIFIER, START, INDEX> {
        static const int value = -1;
    };

    template<class CLASSIFIER, int START, int INDEX>
    struct index_of<void, CLASSIFIER, START, INDEX> : std::integral_constant<int, -1> {
    };

    template<class COLLECTION, class CLASSIFIER>
    struct count {
        template<class ITEM>
        using is_same = std::is_same<ITEM, CLASSIFIER>;

        static const std::size_t value = filter<COLLECTION, is_same>::type::size();
    };

    template<class, template<class ...> class, class ...>
    struct reduce;

    template<template<class ...> class EXPR>
    struct reduce<uml::collection<>, EXPR> {
        typedef uml::collection<> type;
    };


    template<class ITEM, template<class ...> class EXPR, class ...REDUCED>
    struct reduce<uml::collection<ITEM>, EXPR, uml::collection<REDUCED...>>
            : std::conditional<EXPR<uml::collection<REDUCED...>, ITEM>::value, uml::collection<REDUCED..., ITEM>, uml::collection<REDUCED...>> {
    };


    template<class ITEM, class ...ITEMS, template<class ...> class EXPR, class ...REDUCED>
    struct reduce<uml::collection<ITEM, ITEMS...>, EXPR, uml::collection<REDUCED...>>
            : reduce<uml::collection<ITEMS...>, EXPR, typename reduce<uml::collection<ITEM>, EXPR, uml::collection<REDUCED...>>::type> {
    };


    template<class ITEM, class ...ITEMS, template<class ...> class EXPR>
    struct reduce<uml::collection<ITEM, ITEMS...>, EXPR> : reduce<uml::collection<ITEM, ITEMS...>, EXPR, uml::collection<>> {
    };


    template <class T, std::size_t = sizeof(T)>
    std::true_type is_complete_impl(T *);

    std::false_type is_complete_impl(...);

    template <class T>
    using is_complete = decltype(is_complete_impl(std::declval<T*>()));
}

#endif //RTSM_UTILS_H
