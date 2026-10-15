# Validation plan

## Host CI

The GitHub Actions workflow configures CMake, compiles the portable C11 core with warnings-as-errors, runs Release-safe tests, and runs the deterministic demo. Tests cover invalid and duplicate registration, execution min/average/max, deadline/budget/stack flags, event clearing, counter wrap, invalid samples, unknown IDs, and maximum task capacity.

## Target validation

Record board/MCU, core clock, compiler, FreeRTOS version/port, timer source, counter units, and reporting interval.

| Test | Procedure | Evidence |
|---|---|---|
| Counter scale | Compare counter deltas with GPIO pulses or logic-analyzer timing | Measured frequency and overhead |
| Execution duration | Instrument short and long representative jobs | Samples and repeatability |
| Deadline miss | Delay a test task past its deadline | Event flag and count |
| Stack warning | Feed controlled low-headroom samples or use a dedicated isolated test task | Units and threshold response |
| Preemption | Repeat measurement with higher-priority load enabled/disabled | Effect on elapsed time |
| Counter wrap | Test near wrap using host cases | No false oversized duration |
| Reporter overhead | Compare with reporting enabled and disabled | Added latency and CPU use |

Do not deliberately overflow a task stack on a shared board. Host-simulate low headroom or use an isolated test task with stack-overflow checking enabled. No WCET proof or real-time certification is implied.
