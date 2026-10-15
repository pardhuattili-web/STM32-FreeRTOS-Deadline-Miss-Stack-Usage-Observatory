#ifndef OBSERVATORY_H
#define OBSERVATORY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define OBSERVATORY_MAX_TASKS 16U
typedef enum { OBS_EVENT_NONE=0, OBS_EVENT_DEADLINE_MISS=1U<<0, OBS_EVENT_STACK_WARNING=1U<<1, OBS_EVENT_EXEC_BUDGET=1U<<2, OBS_EVENT_INVALID_SAMPLE=1U<<3 } observatory_event_t;
typedef struct { uint16_t task_id; uint32_t period_ticks; uint32_t relative_deadline_ticks; uint32_t execution_budget_ticks; uint32_t stack_warning_threshold; } observatory_task_config_t;
typedef struct { bool configured; uint16_t task_id; uint32_t period_ticks; uint32_t relative_deadline_ticks; uint32_t deadline_ticks; uint32_t release_ticks; uint32_t execution_budget_ticks; uint32_t stack_warning_threshold; uint32_t completed_jobs; uint32_t deadline_misses; uint32_t execution_budget_violations; uint32_t stack_warnings; uint32_t sample_count; uint32_t execution_min_ticks; uint32_t execution_max_ticks; uint64_t execution_sum_ticks; uint32_t last_execution_ticks; uint32_t last_stack_headroom; uint32_t events; } observatory_task_t;
typedef struct { observatory_task_t tasks[OBSERVATORY_MAX_TASKS]; size_t task_count; } observatory_t;
typedef struct { uint16_t task_id; uint32_t completed_jobs; uint32_t deadline_misses; uint32_t execution_budget_violations; uint32_t stack_warnings; uint32_t sample_count; uint32_t execution_min_ticks; uint32_t execution_max_ticks; uint32_t execution_average_ticks; uint32_t last_execution_ticks; uint32_t last_stack_headroom; uint32_t event_flags; } observatory_snapshot_t;
void observatory_init(observatory_t *observatory);
bool observatory_register_task(observatory_t *observatory, observatory_task_config_t config, uint32_t first_release_ticks);
bool observatory_record_job(observatory_t *observatory, uint16_t task_id, uint32_t start_ticks, uint32_t finish_ticks, uint32_t stack_headroom);
bool observatory_record_stack(observatory_t *observatory, uint16_t task_id, uint32_t stack_headroom);
bool observatory_get_snapshot(const observatory_t *observatory, uint16_t task_id, observatory_snapshot_t *snapshot);
uint32_t observatory_event_flags(const observatory_t *observatory, uint16_t task_id);
void observatory_clear_events(observatory_t *observatory, uint16_t task_id, uint32_t event_mask);
#endif
