//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN


#include <rtsm/StateMachine.h>
#include <rtsm/log.h>
#include <iostream>
#include <sstream>
#include "doctest/doctest.h"
#include <vector>


namespace test {

    struct Path : std::vector<std::string> {
        bool evaluate(Path::iterator iter) {
            return iter == end();
        }

        template<class ...NAMES>
        bool evaluate(Path::iterator iter, const char *name, NAMES &&...names) {
            if (iter != end()) {
                return *iter == std::string(name) && evaluate(++iter, std::forward<NAMES>(names)...);
            }
            return false;
        }

        template<class ...NAMES>
        bool evaluate(NAMES &&...names) {
            return evaluate(begin(), std::forward<NAMES>(names)...);
        }

        void dump() {
            std::cout << "BEGIN PATH DUMP" << std::endl;
            for (auto i = begin(); i != end(); ++i)
                std::cout << *i << ' ';
            if (size()) {
                std::cout << std::endl;
            }
            std::cout << "END PATH DUMP" << std::endl;
        }
    };


    Path path;


    template<class CLASSIFIER>
    struct Behavior : rtsm::Behavior {
        template<class ...ARGS>
        void execute(ARGS &&...args) {
            std::stringstream path_id;
            path_id << rtsm::log::demangle(typeid(CLASSIFIER).name()) << "::" << name();
            path.push_back(path_id.str());
        }

        virtual std::string name() = 0;

    };


    template<class STATE>
    struct entry : Behavior<STATE> {
        std::string name() override {
            return "entry";
        }
    };

    template<class CLASSIFIER>
    struct exit : Behavior<CLASSIFIER> {
        std::string name() override {
            return "exit";
        }
    };

    template<unsigned int NUMBER>
    struct State : rtsm::State {
        using entry = test::entry<State>;
        using exit = test::exit<State>;
    };

}


struct Event1 : rtsm::Event {
};
struct Event2 : rtsm::Event {
};
struct SelfEvent : rtsm::Event {
};
struct ExternalEvent : rtsm::Event {
};
struct LocalEvent : rtsm::Event {
};
struct IllegalEvent : rtsm::Event {

};
struct TestSM : rtsm::StateMachine<TestSM> {


    struct A1 : rtsm::State {
        using entry = test::entry<A1>;
        using exit = test::exit<A1>;
        struct B1 : rtsm::State {
            using entry = test::entry<B1>;
            using exit = test::exit<B1>;
        };
        struct B2 : rtsm::State {
            using entry = test::entry<B2>;
            using exit = test::exit<B2>;
        };

        struct region1 : rtsm::Region {
            using subvertex = rtsm::collection<B1, B2>;
        };

        struct B3 : rtsm::State {
            using entry = test::entry<B3>;
            using exit = test::exit<B3>;
        };

        using S4 = test::State<4>;

        struct region2 : rtsm::Region {
            using subvertex = rtsm::collection<rtsm::initial<B3>, B3, S4>;
        };
        using region = rtsm::collection<region1, region2>;
    };


    struct A2 : rtsm::State {
        using entry = test::entry<A2>;
        using exit = test::exit<A2>;
    };

    struct A3 : rtsm::State {
        using entry = test::entry<A3>;
        using exit = test::exit<A3>;
    };

    struct A4 : rtsm::State {
        using entry = test::entry<A4>;
        using exit = test::exit<A4>;
        struct ExternalTransition : rtsm::transition<ExternalEvent, A4, A3> {
            struct guard : rtsm::Constraint {
                bool evaluate(TestSM &, ExternalEvent &) {
                    return false;
                }
            };
        };
        using outgoing = rtsm::collection<ExternalTransition>;

    };
    using Initial = rtsm::initial<A1::B1>;

    using MultiTriggerTransition = rtsm::transition<rtsm::collection<Event1, Event2>, A1, A2>;
    using SelfTransition = rtsm::transition<SelfEvent, A1, A1>;
    using LocalTransition1 = rtsm::transition<LocalEvent, A1, A1::B2>;
    using ExternalTransition0 = rtsm::transition<ExternalEvent, A1::B2, A1::B1>;
    using ExternalTransition1 = rtsm::transition<ExternalEvent, A1::B1, A4>;
    using ExternalTransition2 = rtsm::transition<ExternalEvent, A4, A1::B1>;
    using IllegalTransition  = rtsm::transition<IllegalEvent, A1::B1, A1::S4>;
    struct region : rtsm::Region {
        using subvertex = rtsm::collection<Initial, A1, A2, A3, A4>;
        using transition = rtsm::collection<MultiTriggerTransition, LocalTransition1, SelfTransition, ExternalTransition0, ExternalTransition1, ExternalTransition2, IllegalTransition>;

    };
};




TEST_SUITE ("transitions") {
    TEST_CASE ("initial transition") {
        rtsm::Object<TestSM> sm;
        REQUIRE(sm.inState<TestSM::A1::B1>());
        REQUIRE(test::path.evaluate("TestSM::A1::entry", "TestSM::A1::B1::entry", "TestSM::A1::B3::entry"));
        test::path.clear();
        SUBCASE("self transition") {
            sm.dispatch(SelfEvent());
            REQUIRE(test::path.evaluate("TestSM::A1::B1::exit", "TestSM::A1::B3::exit", "TestSM::A1::exit", "TestSM::A1::entry", "TestSM::A1::B3::entry"));
            REQUIRE(sm.inState<TestSM::A1>());
            test::path.clear();
            SUBCASE("local transition") {
                sm.dispatch(LocalEvent());
                REQUIRE(test::path.evaluate("TestSM::A1::B2::entry"));
                test::path.clear();
                SUBCASE("external transition 0") {
                    sm.dispatch(ExternalEvent());
                    REQUIRE(test::path.evaluate("TestSM::A1::B2::exit", "TestSM::A1::B1::entry"));
                    test::path.clear();
                    SUBCASE("external transition 1") {
                        sm.dispatch(ExternalEvent());
                        REQUIRE(test::path.evaluate("TestSM::A1::B1::exit", "TestSM::A1::B3::exit", "TestSM::A1::exit", "TestSM::A4::entry"));
                        test::path.clear();
                        SUBCASE("external transition 2") {
                            sm.dispatch(ExternalEvent());
                            REQUIRE(test::path.evaluate("TestSM::A4::exit", "TestSM::A1::entry", "TestSM::A1::B1::entry", "TestSM::A1::B3::entry"));
                            test::path.clear();
                            SUBCASE("transition with multiple triggers") {
                                SUBCASE("Event1 transition") {
                                    sm.dispatch(Event1());
                                    REQUIRE(test::path.evaluate("TestSM::A1::B1::exit", "TestSM::A1::B3::exit", "TestSM::A1::exit", "TestSM::A2::entry"));
                                    test::path.clear();
                                }
                                SUBCASE("Event2 transition") {
                                    sm.dispatch(Event2());
                                    REQUIRE(test::path.evaluate("TestSM::A1::B1::exit","TestSM::A1::B3::exit", "TestSM::A1::exit", "TestSM::A2::entry"));
                                    test::path.clear();
                                }
                            }
                        }
                        SUBCASE("illegal transition from one Region to another in the same immediate enclosing composite") {
                            sm.dispatch(IllegalEvent());
                        }
                    }
                }
            }
        }
    }
}