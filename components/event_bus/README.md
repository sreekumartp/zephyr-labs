# Event Bus Library for Zephyr OS

This library provides a decoupled, asynchronous, publish-subscribe event bus system for Zephyr applications. It is designed to abstract OS-specific communication primitives and allow for modular, scalable application architecture.

## Features

- **OS Abstraction:** Components do not directly use `k_msgq` or other kernel objects for communication.
- **Decoupled Architecture:** Publishers and subscribers do not need to know about each other.
- **Asynchronous Communication:** Components can publish events without blocking.
- **One-to-Many Dispatch:** A single event can be dispatched to multiple subscribers.
- **Thread-Safe:** Subscribing and publishing events are thread-safe operations.

## How to Integrate

1.  Place this `event_bus` directory into your Zephyr project's `modules/lib/` directory.
2.  Enable the library in your application's `prj.conf` file by adding:
    ```
    CONFIG_EVENT_BUS=y
    ```
3.  Include the main header in your application files:
    ```c
    #include <event_bus/event_bus.h>
    ```

## API Usage

See the header files in `include/event_bus/` for a detailed API reference.

1.  **Initialization:** Call `event_bus_init()` once at startup.
2.  **Subscribing:** A component thread creates its own `k_msgq` and calls `event_bus_subscribe()` to listen for specific `event_id_t` values.
3.  **Publishing:** Any component can call `event_bus_publish()` to send an `app_event_t` to the system.

