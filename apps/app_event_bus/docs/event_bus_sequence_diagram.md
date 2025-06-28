# Event Bus API Usage Sequence Diagram

This sequence diagram shows how the `app_event_bus` application uses the event bus API with the callback mechanism enabled.

```mermaid
sequenceDiagram
    participant Main as Main Thread
    participant EventBus as Event Bus
    participant Sender as Sender Thread
    participant CallbackRx as Callback Receiver
    participant ProcessingQ as Processing Queue
    participant ProcessingTh as Processing Thread
    
    Note over Main, ProcessingTh: Application Initialization
    Main->>+EventBus: event_bus_init()
    EventBus-->>-Main: 0 (success)
    
    Note over CallbackRx: SYS_INIT registers callback handler
    CallbackRx->>+EventBus: event_bus_register_handler(app_event_handler, [EVENT_DOOR_OPENED], 1)
    EventBus-->>-CallbackRx: 0 (success)
    
    Note over Main, ProcessingTh: Runtime Event Processing
    loop Every 3 seconds
        Sender->>Sender: k_msleep(3000)
        Note over Sender: Create event with counter payload
        Sender->>+EventBus: event_bus_post(&msg_event)<br/>{id: EVENT_DOOR_OPENED, payload: counter++}
        
        Note over EventBus: Event bus dispatches to registered handlers
        EventBus->>+CallbackRx: app_event_handler(event)
        
        Note over CallbackRx: Lightweight handler - just enqueue
        CallbackRx->>ProcessingQ: k_msgq_put(&processing_msgq, event, K_NO_WAIT)
        CallbackRx-->>-EventBus: (callback returns)
        EventBus-->>-Sender: 0 (success)
        
        Note over ProcessingTh: Processing thread waits on queue
        ProcessingQ->>+ProcessingTh: k_msgq_get(&processing_msgq, &received_event, K_FOREVER)
        
        Note over ProcessingTh: Heavy processing simulation
        ProcessingTh->>ProcessingTh: k_msleep(500) // Simulate heavy work
        ProcessingTh-->>-ProcessingQ: Processing complete
    end
    
    Note over Main, ProcessingTh: Key API Functions Used:
    Note over Main, ProcessingTh: • event_bus_init() - Initialize the event bus
    Note over Main, ProcessingTh: • event_bus_register_handler() - Register callback for events
    Note over Main, ProcessingTh: • event_bus_post() - Publish events to subscribers
```

## Analysis Summary

The `app_event_bus` application demonstrates the **callback-based** event bus pattern with the following key components:

### Core API Usage:

1. **Initialization**: `event_bus_init()` called once during startup
2. **Registration**: `event_bus_register_handler()` registers callbacks for specific event types
3. **Publishing**: `event_bus_post()` sends events to all registered handlers

### Architecture Pattern:

- **Sender Thread**: Periodically publishes `EVENT_DOOR_OPENED` events with incrementing counter
- **Callback Handler**: Lightweight, non-blocking handler that immediately enqueues events
- **Processing Thread**: Heavy processing happens asynchronously in dedicated thread
- **Message Queue**: Decouples event reception from processing to maintain responsiveness

### Key Benefits:

- **Decoupling**: Publishers don't know about subscribers
- **Responsiveness**: Callback handlers are kept lightweight
- **Scalability**: Heavy processing moved to separate threads
- **Thread Safety**: Uses Zephyr message queues for thread-safe communication

### Configuration:

The application can be configured to use either:
- **Callback mode** (current): `CONFIG_EVENT_BUS_USE_CALLBACK=y`
- **Polling mode**: `CONFIG_EVENT_BUS_USE_POLLING=y`
