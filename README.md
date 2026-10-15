# STM32 FreeRTOS Deadline-Miss & Stack-Usage Observatory

A portable Embedded C observability layer for real-time firmware. It records task execution-time samples, tracks periodic deadlines, flags stack-headroom warnings, and exposes deterministic fault-injection scenarios. The host build makes the monitoring logic testable without vendor libraries; the STM32/FreeRTOS adapter path is documented separately.

> **Validation status:** The portable C core, host simulation, unit tests, and CI are the verified baseline. This repository does not yet claim a successful STM32CubeIDE cross-build or physical-board timing validation. DWT cycle-counter support and FreeRTOS hooks must be integrated against the exact MCU, clock, and FreeRTOS configuration before claiming hardware measurements.

## Highlights

- Per-task execution-time min/max/average and sample count
- Deadline accounting using rollover-safe unsigned tick arithmetic
- Stack high-water/headroom threshold monitoring
- Event counters and latched diagnostic flags
- Injected over-budget execution and low-stack scenarios
- CMake host build with warnings enabled, unit tests, and GitHub Actions
- Design notes for DWT, FreeRTOS runtime stats, and UART reporting

## Repository structure

```text
include/observatory/      Portable public API
src/                      Portable measurements and policy
simulator/                Deterministic host demonstration
tests/                    Release-safe unit tests
platform/stm32_freertos/  Integration guide and adapter contract
docs/                     Architecture, metrics, validation
.github/workflows/        Host CI
```

## Build and run host tests

Requirements: CMake 3.16+ and a C11 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_WARNINGS_AS_ERRORS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/observatory_demo
```

## Metrics and interpretation

- **Execution duration** is supplied by the platform adapter in abstract counter ticks. Convert to microseconds only when the counter frequency and wrap behaviour are known.
- **Deadline miss** is counted when a completion timestamp is later than the configured absolute deadline. The host core uses 32-bit wrap-safe differences; comparisons assume deadlines are less than half the timer range away.
- **Stack headroom** is supplied by the FreeRTOS adapter, for example using the stack high-water-mark API with its port-specific units interpreted correctly. A low-watermark warning indicates observed headroom was below the configured threshold, not proof of the task's absolute worst-case stack requirement.
- **CPU utilization** is not inferred from execution samples alone. For a target report, configure FreeRTOS runtime statistics or measure idle time with a suitable method and document its overhead.

## STM32 / FreeRTOS integration path

1. Create an STM32CubeIDE project for the exact target board and enable FreeRTOS.
2. Enable the Cortex-M DWT cycle counter where supported, and establish the cycle-counter frequency after final clock configuration.
3. Register each monitored task with a stable task ID, period/deadline policy, and stack-headroom threshold.
4. Wrap each monitored task's work section with execution begin/end calls or capture counters in trace hooks.
5. Poll stack high-water marks from a lower-priority monitor task; do not format UART text or block inside a timing-critical section.
6. Configure FreeRTOS runtime statistics only after implementing the required timer macros in FreeRTOSConfig.h.
7. Export compact diagnostic records over UART and validate timing overhead with GPIO pulses and a logic analyzer.

See [platform/stm32_freertos/README.md](platform/stm32_freertos/README.md) and [docs/validation-plan.md](docs/validation-plan.md).

## Safety and engineering limitations

- This is an observability framework, not a hard real-time certification tool.
- A maximum observed execution time is not a mathematical WCET proof.
- Sampling and reporting introduce overhead; quantify it on the target.
- Avoid dynamic allocation in the monitoring fast path.
- Make reporting non-blocking or use a queue/ring buffer drained by a lower-priority task.
- If a monitored task can be preempted, decide whether the measurement is wall-clock elapsed time or CPU time and document that choice.

## Roadmap

- [x] Portable C monitoring core
- [x] Host simulation and unit tests
- [x] CI with warnings-as-errors
- [x] STM32/FreeRTOS integration and validation guide
- [ ] Actual STM32CubeIDE project and DWT adapter
- [ ] FreeRTOS task/stack/runtime-stat integration
- [ ] Measure monitoring overhead on hardware
- [ ] Publish a real-board trace and demo video

## License

MIT. See [LICENSE](LICENSE).
