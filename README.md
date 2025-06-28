# Zephyr Labs - Embedded Systems Learning Repository

A comprehensive collection of Zephyr RTOS applications and components demonstrating advanced embedded systems design patterns, real-time programming, and IoT development.

## 🎯 Overview

This repository contains practical examples and reusable components for learning Zephyr RTOS development. Each project demonstrates specific embedded systems concepts while building toward real-world applications.

## 📁 Repository Structure

```
zephyr-labs/
├── apps/                          # Sample applications
│   ├── app_event_bus/            # Event bus demonstration
│   ├── my_gpio_app/              # Basic GPIO operations
│   ├── my_timer_app/             # Timer and timing examples
│   └── washing_machine_sim/      # Advanced FSM-based simulation
├── components/                    # Reusable components
│   └── event_bus/                # Event-driven communication system
├── bootloader/                    # Bootloader configurations
│   └── mcuboot/                  # MCUboot integration
└── scripts/                      # Build and utility scripts
```

## 🚀 Featured Applications

### 🏠 Washing Machine Simulator
*Location: `apps/washing_machine_sim`*

A sophisticated embedded application demonstrating:
- **Hierarchical Finite State Machines** (L1 System + L2 Wash Cycle)
- **Event-driven Architecture** with decoupled communication
- **Multi-threaded Design** with priority-based scheduling
- **Interactive Shell Interface** for testing and debugging
- **Sensor Simulation** for hardware-independent development

**Key Features:**
- Complete wash cycle simulation with 25+ states
- Power management and fault handling
- Configurable wash programs (prewash, heating, steam)
- Real-time event processing with <1ms latency
- Comprehensive test suite and documentation

📖 [View Design Document](apps/washing_machine_sim/docs/washing_machine_design.md)

### 🔄 Event Bus System
*Location: `apps/app_event_bus` + `components/event_bus`*

A robust publish-subscribe communication framework featuring:
- **Dual Operation Modes**: Callback-based and polling-based
- **Thread-safe Communication** across multiple contexts
- **Memory Efficient Design** with configurable resource pools
- **Zephyr Integration** using work queues and message queues

**Use Cases:**
- Decoupled inter-component communication
- Sensor data distribution
- System-wide event notification
- Real-time data streaming

📖 [View Design Document](components/event_bus/docs/design/event_bus_design.md)

### ⚡ GPIO and Timer Examples
*Location: `apps/my_gpio_app`, `apps/my_timer_app`*

Fundamental Zephyr RTOS examples covering:
- GPIO configuration and interrupt handling
- Timer management and periodic tasks
- Device tree integration
- Hardware abstraction layers

## 🧩 Reusable Components

### Event Bus Component
*Location: `components/event_bus`*

Production-ready event communication system with:
- **Configurable Architecture**: Choose between callback or polling modes
- **Scalable Design**: Support for multiple publishers and subscribers
- **Error Handling**: Graceful degradation and resource management
- **Documentation**: Complete API reference and usage examples

**Integration:**
```cmake
# In your CMakeLists.txt
target_link_libraries(app PRIVATE event_bus)
```

```c
// In your application
#include "event_bus.h"

// Initialize
event_bus_init();

// Register handler
event_bus_register_handler(my_handler, events, num_events);

// Post events
const app_event_t event = {.id = MY_EVENT, .payload.s32 = data};
event_bus_post(&event);
```

## 🛠️ Getting Started

### Prerequisites

- **Zephyr SDK** (v0.16.0 or later)
- **West** (Zephyr meta-tool)
- **CMake** (3.20 or later)
- **Python** (3.8 or later)

### Quick Setup

1. **Clone the repository:**
   ```bash
   git clone <repository-url> zephyr-labs
   cd zephyr-labs
   ```

2. **Initialize Zephyr workspace:**
   ```bash
   west init -l .
   west update
   ```

3. **Build an application:**
   ```bash
   # Build washing machine simulator for native simulation
   west build -b native_sim apps/washing_machine_sim

   # Build for specific hardware (e.g., nRF52840)
   west build -b nrf52840dk_nrf52840 apps/washing_machine_sim
   ```

4. **Run the application:**
   ```bash
   # For native simulation
   west build -t run

   # For hardware (after flashing)
   west flash
   ```

### Build Scripts

Convenient build scripts are provided in the `scripts/` directory:

```bash
# Create and run native simulation
./scripts/create-app-native-sim.sh washing_machine_sim

# Build for specific board
./scripts/build_app.sh washing_machine_sim nrf52840dk_nrf52840

# Clean build artifacts
./scripts/clean_app.sh washing_machine_sim
```

## 🎯 Learning Objectives

This repository demonstrates key embedded systems concepts:

### Real-Time Systems
- **Priority-based Scheduling**: Thread priorities and preemption
- **Timing Constraints**: Meeting real-time deadlines
- **Resource Management**: Memory pools and queue management
- **Interrupt Handling**: Efficient ISR design patterns

### Software Architecture
- **State Machine Design**: Hierarchical and nested FSMs
- **Event-Driven Programming**: Asynchronous communication patterns
- **Component Architecture**: Modular and reusable design
- **Error Handling**: Fault tolerance and recovery strategies

### Zephyr RTOS Features
- **Device Tree**: Hardware abstraction and configuration
- **Kconfig**: Build-time configuration management
- **Work Queues**: Deferred interrupt handling
- **Message Queues**: Inter-thread communication
- **Threading**: Multi-threaded application design

### Testing and Debugging
- **Unit Testing**: Automated test suites with Ztest
- **Simulation**: Hardware-independent development
- **Shell Interface**: Interactive debugging and testing
- **Logging**: Real-time system monitoring

## 📚 Documentation

Each project includes comprehensive documentation:

- **Design Documents**: Architecture and implementation details
- **API Reference**: Function and data structure documentation
- **Sequence Diagrams**: Dynamic behavior visualization
- **State Diagrams**: State machine specifications
- **Build Instructions**: Platform-specific setup guides

## 🧪 Testing

### Unit Tests
```bash
# Run FSM unit tests
west build -b native_sim apps/washing_machine_sim/fsm/tests
west build -t run

# Run event bus tests
west build -b native_sim components/event_bus/tests
west build -t run
```

### Interactive Testing
```bash
# Start washing machine simulator with shell
west build -b native_sim apps/washing_machine_sim
west build -t run

# In the shell:
uart:~$ send_event 4    # Power button pressed
uart:~$ send_event 7    # Cycle selected
uart:~$ start           # Start wash cycle
```

## 🎨 Visual Architecture

The repository includes rich visual documentation:

- **Class Diagrams**: Component relationships and interfaces
- **State Diagrams**: Finite state machine specifications  
- **Sequence Diagrams**: Event flow and interaction patterns
- **Flowcharts**: Build processes and decision logic

All diagrams are created using Mermaid and embedded in documentation.

## 🏗️ Software Engineering Best Practices

This repository demonstrates industry-standard software engineering practices for embedded systems development:

### 🔄 Continuous Integration/Continuous Deployment (CI/CD)

**Automated Build Pipeline:**
- **Multi-Platform Builds**: Automatic builds for multiple target boards (native_sim, nRF52840, STM32)
- **Static Analysis**: Code quality checks with cppcheck and clang-static-analyzer
- **Unit Test Automation**: Automated execution of all test suites on code changes
- **Documentation Generation**: Automatic update of API docs and design diagrams
- **Release Automation**: Automated versioning and release artifact generation

**GitHub Actions Workflow:**
```yaml
# Example CI workflow
name: Zephyr Build and Test
on: [push, pull_request]
jobs:
  build-and-test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        board: [native_sim, nrf52840dk_nrf52840, nucleo_f411re]
        app: [washing_machine_sim, app_event_bus]
    steps:
      - uses: actions/checkout@v3
      - name: Setup Zephyr
        run: |
          west init -l .
          west update
      - name: Build Application
        run: west build -b ${{ matrix.board }} apps/${{ matrix.app }}
      - name: Run Tests
        if: matrix.board == 'native_sim'
        run: west build -t run
```

### 🖥️ Hardware Simulation and Testing

**Native Simulation Environment:**
- **Hardware-Independent Development**: Complete application testing without physical hardware
- **Sensor Simulation**: GPIO emulation for door sensors, water level sensors, and user interfaces
- **Real-Time Simulation**: Accurate timing behavior simulation for embedded applications
- **Integration Testing**: End-to-end system testing in simulated environment

**Simulation Benefits:**
- **Faster Development Cycles**: No need to flash hardware for basic testing
- **Reproducible Testing**: Consistent test environment across all developers
- **Cost-Effective**: Reduced hardware requirements for development teams
- **Parallel Development**: Multiple developers can work simultaneously without hardware conflicts

**Hardware Emulation Features:**
```c
// Example: GPIO emulation for sensor testing
#ifdef CONFIG_BOARD_NATIVE_SIM
    // Use GPIO emulation for simulation
    gpio_emul_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    gpio_emul_flags_set(gpio_emul_dev, PIN_DOOR_SENSOR, GPIO_INPUT);
#else
    // Use real hardware GPIO
    sensor_dev = DEVICE_DT_GET(DT_ALIAS(sensor0));
    gpio_pin_configure_dt(&sensor, GPIO_INPUT);
#endif
```

### 🤖 Automated Code Review with CodeRabbit

**AI-Powered Code Analysis:**
- **Intelligent Review**: CodeRabbit provides context-aware code review suggestions
- **Security Analysis**: Automatic detection of potential security vulnerabilities
- **Performance Optimization**: Suggestions for memory usage and execution efficiency
- **Best Practices Enforcement**: Ensures adherence to Zephyr and embedded systems conventions

**CodeRabbit Integration Features:**
- **Pull Request Reviews**: Automatic analysis of all code changes
- **Learning from Codebase**: AI learns project-specific patterns and standards
- **Multi-Language Support**: Covers C, CMake, Device Tree, and Kconfig files
- **Continuous Learning**: Improves recommendations based on accepted/rejected suggestions

**Review Categories:**
- **Code Quality**: Naming conventions, function complexity, code organization
- **Memory Safety**: Buffer overflow detection, pointer validation, memory leaks
- **Real-Time Considerations**: Interrupt safety, timing constraints, priority inversions
- **Zephyr Best Practices**: Proper use of kernel APIs, device tree patterns, Kconfig usage

### 📊 Quality Metrics and Monitoring

**Code Quality Metrics:**
- **Test Coverage**: Maintain >80% code coverage across all components
- **Cyclomatic Complexity**: Keep functions below complexity threshold (10)
- **Memory Usage**: Monitor RAM and flash usage per application
- **Performance Benchmarks**: Track execution time and response latencies

**Automated Quality Checks:**
```bash
# Static analysis
west build -t cppcheck

# Memory usage analysis
west build -t ram_report
west build -t rom_report

# Test coverage report
west build -t coverage_report
```

### 🔒 Security and Safety Practices

**Secure Development:**
- **MISRA C Compliance**: Adherence to safety-critical coding standards
- **Static Security Analysis**: Regular security vulnerability scanning
- **Memory Protection**: Use of memory protection units and stack guards
- **Secure Boot**: MCUboot integration for verified boot processes

**Safety-Critical Features:**
- **Watchdog Integration**: System health monitoring and recovery
- **Fault Injection Testing**: Deliberate error introduction for robustness testing
- **Error Handling Validation**: Comprehensive error condition testing
- **State Machine Verification**: Formal verification of critical state transitions

## 🚀 Advanced Features

### Multi-Board Support
- **Native Simulation**: Hardware-independent development
- **Nordic nRF Series**: Bluetooth and wireless applications
- **STM32 Boards**: ARM Cortex-M development
- **ESP32**: Wi-Fi and IoT connectivity

### Development Tools
- **VS Code Integration**: IntelliSense and debugging support
- **West Commands**: Simplified build and flash operations
- **MCUboot Support**: Secure bootloader integration
- **Shell Commands**: Runtime system inspection

### Performance Optimization
- **Memory Profiling**: RAM and flash usage analysis
- **Timing Analysis**: Real-time performance measurement
- **Resource Monitoring**: Thread and queue utilization
- **Power Management**: Low-power design patterns

## 🤝 Contributing

We welcome contributions and maintain high software engineering standards! Please follow these guidelines:

### Development Workflow

1. **Fork the repository** and create a feature branch
2. **Follow coding standards** and best practices outlined below
3. **Write comprehensive tests** for new functionality
4. **Update documentation** including design docs and API references
5. **Ensure CI/CD passes** all automated checks
6. **Submit a pull request** for CodeRabbit review

### Code Quality Standards

**Code Style:**
- Follow Zephyr coding conventions and MISRA C guidelines
- Use meaningful variable and function names
- Include comprehensive comments and API documentation
- Maintain cyclomatic complexity below 10 per function
- Ensure thread-safety in multi-threaded contexts

**Testing Requirements:**
- Write unit tests for all new functions (aim for >80% coverage)
- Include integration tests for component interactions
- Test both success and error conditions
- Validate real-time constraints and timing requirements
- Test on both native simulation and target hardware

**Documentation Standards:**
- Update relevant design documents
- Include Mermaid diagrams for architectural changes
- Document API changes with examples
- Update README if adding new features or dependencies

### Automated Quality Assurance

**Pre-Commit Checks:**
```bash
# Run local quality checks before committing
west build -t cppcheck           # Static analysis
west build -t coverage_report    # Test coverage
west build -t ram_report         # Memory usage check
west build -t rom_report         # Flash usage check
```

**CI/CD Pipeline:**
- All pull requests trigger automated builds for multiple platforms
- Unit tests run automatically on native_sim
- Static analysis and security scanning performed
- CodeRabbit provides AI-powered code review
- Documentation builds verified

**Review Process:**
1. **Automated Analysis**: CI/CD pipeline runs comprehensive checks
2. **AI Review**: CodeRabbit analyzes code for quality and security issues
3. **Human Review**: Maintainers review for design and architectural considerations
4. **Integration Testing**: Final validation on target hardware platforms

## 📄 License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.

## 🔗 Resources

### Zephyr RTOS
- [Official Documentation](https://docs.zephyrproject.org/)
- [Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html)
- [API Reference](https://docs.zephyrproject.org/latest/reference/index.html)

### Development Tools
- [Visual Studio Code](https://code.visualstudio.com/)
- [Zephyr Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.vscode-zephyr)
- [West Meta-Tool](https://docs.zephyrproject.org/latest/guides/west/index.html)

### Software Engineering Tools
- [CodeRabbit](https://coderabbit.ai/) - AI-powered code review
- [GitHub Actions](https://github.com/features/actions) - CI/CD automation
- [Zephyr Static Analysis](https://docs.zephyrproject.org/latest/guides/test/index.html) - Code quality tools
- [MISRA C Guidelines](https://www.misra.org.uk/) - Safety-critical coding standards

### Hardware Platforms
- [Nordic Semiconductor](https://www.nordicsemi.com/Products/Development-hardware)
- [STMicroelectronics](https://www.st.com/en/evaluation-tools/stm32-nucleo-boards.html)
- [Espressif Systems](https://www.espressif.com/en/products/devkits)

## 📞 Support

For questions or support:
- Open an issue on GitHub
- Check existing documentation
- Review Zephyr community resources

---

**Happy Embedded Development!** 🚀

*This repository demonstrates production-quality embedded software design patterns using Zephyr RTOS. Each example is crafted to teach specific concepts while building practical, reusable solutions.*
