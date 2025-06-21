---
name: 🛠️ Event Bus TODO Tracker
about: Track enhancements and planned improvements for the Zephyr Event Bus module
title: "[EventBus] Task: <short summary>"
labels: [event-bus, enhancement, todo]
assignees: ''

---

## 🔍 Description

<!-- Brief description of the enhancement, improvement, or bugfix -->

## 📋 Checklist

### 🧩 Functional Enhancements
- [ ] Add ISR-safe event post API (`event_post_from_isr()`)
- [ ] Use `irq_lock()` or `k_spinlock` for shared context
- [ ] Defer ISR events using `k_work` (work queue)
- [ ] Add callback or dispatch hook support

### 🔒 Safety and Robustness
- [ ] Handle backpressure when message queue is full
- [ ] Use `k_is_in_isr()` checks to validate context

### 🧪 Testing
- [ ] Add tests simulating ISR event posts (`irq_offload()`)
- [ ] Add multithreaded concurrency and edge case tests
- [ ] Fill queue and test overflow handling

### 🔍 Logging and Tracing
- [x] Integrate Zephyr logging with log levels
- [ ] Enable runtime log filtering
- [ ] Use `CONFIG_TRACING` to trace post/get operations

### 📚 Documentation
- [ ] Update/Generate README.md
- [ ] Add Doxygen-style comments to APIs

## ✅ Acceptance Criteria

- [ ] Functionality is testable in CI
- [ ] Build and test passes with `twister`
- [ ] Documentation is updated (README and inline)

---

Please edit the checklist to match the scope of your task.
