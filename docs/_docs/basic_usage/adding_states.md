---
title: Adding States
category: Basic Usage
order: 2
---

Bellow is basic examples of how to declare a State.

```cpp
namespace light {
    
    struct Light : rtsm::StateMachine<Light> {
        struct On : rtsm::State {
        
        };
        
        struct Off : rtsm::State {
            
        };
        
        using Initial = rtsm::Initial<Off>;
        struct region : rtsm::Region {
            using subvertex = rtsm::collection<Initial, On, Off>;
        };
    };

}
```

