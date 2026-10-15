# STM32 / FreeRTOS adapter contract

The portable core is ready for host testing; a board-specific adapter is not yet supplied because DWT, runtime-statistics, and stack API details depend on the exact Cortex-M target and FreeRTOS port.

## Integration outline

- Use DWT CYCCNT on supported Cortex-M cores or a free-running hardware timer. Configure it after final clock setup and document tick frequency and wrap behaviour.
- Wrap a well-defined task job, capture counter start/finish, and call `observatory_record_job()` with the job duration and stack headroom. This measurement is wall-clock elapsed time if the task may be preempted.
- Query stack high-water marks from a low-priority monitor task. Confirm whether the selected FreeRTOS port reports words or bytes and normalize units.
- Configure the FreeRTOS runtime-statistics timer macros only after testing the counter source. Do not calculate CPU utilization from job duration alone.
- Copy snapshots to a queue or ring buffer; format UART output in a lower-priority reporter, never inside a timing-critical hook.

Pseudocode:

```c
uint32_t start = platform_counter_now();
run_control_step();
uint32_t finish = platform_counter_now();
uint32_t headroom = platform_stack_headroom(control_task_handle);
(void)observatory_record_job(&monitor, TASK_ID_CONTROL, start, finish, headroom);
```

Implement and validate `platform_counter_now()` and `platform_stack_headroom()` for your specific target before compiling this sketch.
