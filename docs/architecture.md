# Architecture

The portable C core is independent of STM32 HAL and FreeRTOS. A platform adapter captures a task job's start/finish counter values plus measured stack headroom; the core updates execution statistics, deadline misses, execution-budget violations, and stack warnings. A lower-priority reporting task reads snapshots and exports telemetry.

## Event policy

- `OBS_EVENT_DEADLINE_MISS`: completion later than the configured absolute deadline.
- `OBS_EVENT_EXEC_BUDGET`: measured duration exceeds its budget.
- `OBS_EVENT_STACK_WARNING`: observed unused stack is below a configured threshold.
- `OBS_EVENT_INVALID_SAMPLE`: sample duration is outside the unambiguous supported range.

Event flags stay latched until explicitly cleared; counters are cumulative.

## Timing model

The counters are unsigned 32-bit ticks. Subtraction handles one wrap when elapsed durations/deadline windows are less than half the counter range. This baseline assumes periodic jobs and advances each task deadline by whole periods after a completion. If the application must count releases that never execute, add a scheduler/monitor tick hook.

## Limitations

Observed maxima do not prove WCET. Stack high-water marks describe observed unused stack, not every possible call path. Wall-clock task time includes preemption unless CPU-time accounting is specifically added. Target-side measurement and reporting overhead must be quantified.
