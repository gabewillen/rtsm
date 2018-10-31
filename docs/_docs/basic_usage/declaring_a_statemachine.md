---
title: Declaring A StateMachine
category: Basic Usage
order: 1
---
Below is an empty StateMachine Classifier declaration
```cpp
namespace light {
    struct Light : rtsm::StateMachine<Light> {
        Light(int watts=0): _watts(watts) {}
    private:
        int _watts;
    }:
}
```
To use the StateMachine you must create an Object.

```cpp
using Light = rtsm::Object<light::Light>;

Light light(100); // instance of Light
```

The Object Classifier automatically forwards arguments to the Light Classifier. A StateMachine is required to have at least one Region that must have an initial transition to a Sate.



