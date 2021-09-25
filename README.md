# DISCLAIMER
This is highly experimental and ambandoned. It's here for historical purporses. Use at your own risk. This was re-written and used internally closed source for my current employer.

## Synopsis

A state machine implementation that relies on the compiler via C++11 templates and SFINAE to resolve the complex 
transition paths and event dispatching required for execution. Regions can be concurrent or non-current for embedded processor's wthout threading support. In the ladder case events are dispatched in the order regions are declared in the collection. It tries to adhere to the uml specification as closely as possible.

## Code Example
Below is a bare minimum example
```cpp
#include <rtsm/StateMachine.h>

namespace light {
    struct Light : rtsm::StateMachine<Light> {

        struct OnEvent : rtsm::Event {
        };
        struct OffEvent : rtsm::Event {
        };

        struct On : rtsm::State {
            struct entry : rtsm::Behavior {
                void execute(Light &light) {
                    std::cout << "Light is on" << std::endl;
                }
            };

            struct exit : rtsm::Behavior {
                void execute(Light &light) {
                    std::cout << "Light is turning off" << std::endl;
                }
            };
        };

        struct Off : rtsm::State {
            struct entry : rtsm::Behavior {
                void execute(Light &light) {
                    std::cout << "Light is off" << std::endl;
                }
            };

            struct exit : rtsm::Behavior {
                void execute(Light &light) {
                    std::cout << "Light is turning on" << std::endl;
                }
            };
        };


        using Initial = rtsm::initial<Off>;

        using OnTransition = rtsm::transition<OnEvent, Off, On>; // External transition
        using OffTransition = rtsm::transition<OffEvent, On, Off>; // External transition


        struct region : rtsm::Region {

            using subvertex = rtsm::collection<Initial, On, Off>
            using transition = rtsm::collection<OnTransition, OffTransition, BlinkTransition>;
        };
    };

}

using Light = rtsm::Object<light::Light>;

int main() {
    Light light;
    light.dispatch(Light::OnEvent());
    light.dispatch(Light::OffEvent());
    light.execute(); // Returns when the light is deactivated or when a FinalState is reached
}

```



