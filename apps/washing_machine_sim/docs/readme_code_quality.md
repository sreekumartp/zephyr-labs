# 🧼 Code Quality Dashboard - Zephyr Washing Machine Simulation

This project follows continuous improvement practices for code quality, testing, and maintainability. Below is a live snapshot of key metrics tracked through GitHub Actions and tooling.

---

## 📊 Build & Test Health

| Metric         | Status       |
|----------------|--------------|
| CI Build       | ![CI](https://github.com/<your-org>/<your-repo>/actions/workflows/build.yml/badge.svg) |
| Test Pass Rate | ✅ 100% (via Twister) |
| Unit Tests     | `test_fsm`, `test_wm`, ... |

---

## 🧪 Code Coverage

| Module                | Coverage |
|------------------------|----------|
| FSM                   | 95%       |
| Door Sensor Simulator | 100%      |
| Water Level Simulator | 100%      |
| Total                 | 96.5%     |

> Generated via: `west twister --coverage`, `genhtml`

---

## 🔍 Static Analysis

| Tool       | Issues Found |
|------------|--------------|
| cppcheck   | 0 Critical, 2 Style |
| clang-tidy | 0 Warnings   |

> Automated via GitHub Actions for every push/PR.

---

## 🎨 Style Consistency

| Tool         | Status |
|--------------|--------|
| clang-format | ✅     |
| Comment Ratio| TBD    |

> Style config: `.clang-format`

---

## 🧠 Complexity (Lizard)

| Function               | Cyclomatic Complexity |
|------------------------|------------------------|
| `wm_fsm_step()`        | 5                      |
| `door_sensor_sim_init`| 2                      |
| `main()`               | 3                      |

> Target: Keep under 10 where possible.

---

## 🔁 Duplication (jscpd)

| Files Affected | % Duplicated |
|----------------|--------------|
| 0              | 0%           |

---

## 📌 TODO
- [ ] Add comment density and API docs (Doxygen)
- [ ] Add badge for code coverage in README
- [ ] Include `zephyr_lint` stage

---

_Last updated: <!--DATE-->_

