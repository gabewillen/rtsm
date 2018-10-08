# DISCLAIMER
I am not a professional C++ developer.  I taught my self using books and resources from the internet. That being said at my current employer I write software  targeted at embedded devices for automating old industrial machinery.  I welcome criticism as long as its constructive and prefereably accompanied
with a solution.  This library is in extensive development and relies heavily on C++11 templates and SFINAE. I have only tried compiling it with GCC 8.1. Use this at your own risk.

## Synopsis

A state machine implementation that relies on the compiler via C++11 templates and SFINAE to resolve the complex 
transition paths and event dispatching required for execution. Regions can be concurrent or non-current for embedded processor's wthout threading support. In the ladder case events are dispatched in order the regions are declared in the collection. It tries to adhere to the uml specification as closely as possible.

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

## Motivation

I started this mainly for self education.  But I also feel that some of the other state machine implementations out there required to much setup or had verbose api's.


## Installation

Provide code examples and explanations of how to get the project.

## API Reference

Depending on the size of the project, if it is small and simple enough the reference docs can be added to the README. For medium size to larger projects it is important to at least provide a link to where the API reference docs live.

## Tests

Describe and show how to run the tests with code examples.

## Contributors

Let people know how they can dive into the project, include important links to things like issue trackers, irc, twitter accounts if applicable.

## License

A short snippet describing the license (MIT, Apache, etc.)
GOAL
====




