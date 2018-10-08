//
// Created by gwillen on 10/5/18.
//

#define NON_CONCURRENT_REGIONS 1

#include <rtsm/StateMachine.h>

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
        struct entry : rtsm::Behavior {
            void execute(TestSM &self);
        };

        struct B1 : rtsm::State {
        };
        struct B2 : rtsm::State {
        };

        struct region1 : rtsm::Region {
            using subvertex = rtsm::collection<B1, B2>;
        };

        struct B3 : rtsm::State {
        };

        struct B4 : rtsm::State {
        };

        struct region2 : rtsm::Region {
            using subvertex = rtsm::collection<rtsm::initial<B3>, B3, B4>;
        };
        using region = rtsm::collection<region1, region2>;
    };


    struct A2 : rtsm::State {
    };

    struct A3 : rtsm::State {
    };

    struct A4 : rtsm::State {
        struct ExternalTransition : rtsm::transition<ExternalEvent, A4, A3> {
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
    using IllegalTransition  = rtsm::transition<IllegalEvent, A1::B1, A1::B4>;
    struct region : rtsm::Region {
        using subvertex = rtsm::collection<Initial, A1, A2, A3, A4>;
        using transition = rtsm::collection<MultiTriggerTransition, LocalTransition1, SelfTransition, ExternalTransition0, ExternalTransition1, ExternalTransition2, IllegalTransition>;

    };
};


void TestSM::A1::entry::execute(TestSM &self) {
    static int i = 0;
    i++;
}


int main(void) {
    rtsm::Object<TestSM> sm;
    sm.dispatch(SelfEvent());
    sm.execute();
}