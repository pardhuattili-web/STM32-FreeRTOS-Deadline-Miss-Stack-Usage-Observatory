#include "observatory/observatory.h"
#include <limits.h>
#include <string.h>
static observatory_task_t *find_task(observatory_t *o, uint16_t id) { size_t i; if (o == NULL) return NULL; for (i=0U;i<o->task_count;++i) if (o->tasks[i].configured && o->tasks[i].task_id==id) return &o->tasks[i]; return NULL; }
static const observatory_task_t *find_task_const(const observatory_t *o, uint16_t id) { size_t i; if (o == NULL) return NULL; for (i=0U;i<o->task_count;++i) if (o->tasks[i].configured && o->tasks[i].task_id==id) return &o->tasks[i]; return NULL; }
void observatory_init(observatory_t *o) { if (o != NULL) memset(o,0,sizeof(*o)); }
bool observatory_register_task(observatory_t *o, observatory_task_config_t c, uint32_t release) {
 if (o==NULL || o->task_count>=OBSERVATORY_MAX_TASKS || c.period_ticks==0U || c.relative_deadline_ticks==0U || c.relative_deadline_ticks>=0x80000000U || c.execution_budget_ticks==0U || find_task(o,c.task_id)!=NULL) return false;
 observatory_task_t *t=&o->tasks[o->task_count++]; memset(t,0,sizeof(*t)); t->configured=true; t->task_id=c.task_id; t->period_ticks=c.period_ticks; t->relative_deadline_ticks=c.relative_deadline_ticks; t->deadline_ticks=release+c.relative_deadline_ticks; t->release_ticks=release; t->execution_budget_ticks=c.execution_budget_ticks; t->stack_warning_threshold=c.stack_warning_threshold; t->execution_min_ticks=UINT32_MAX; return true;
}
bool observatory_record_job(observatory_t *o,uint16_t id,uint32_t start,uint32_t finish,uint32_t headroom) {
 observatory_task_t *t=find_task(o,id); if (t==NULL) return false; uint32_t duration=finish-start;
 if (duration>=0x80000000U) { t->events|=OBS_EVENT_INVALID_SAMPLE; return false; }
 bool late=(uint32_t)(finish-t->deadline_ticks)<0x80000000U;
 t->last_execution_ticks=duration; if(duration<t->execution_min_ticks)t->execution_min_ticks=duration; if(duration>t->execution_max_ticks)t->execution_max_ticks=duration; t->execution_sum_ticks+=duration; t->sample_count++; t->completed_jobs++;
 if(duration>t->execution_budget_ticks){t->execution_budget_violations++;t->events|=OBS_EVENT_EXEC_BUDGET;}
 if(late){t->deadline_misses++;t->events|=OBS_EVENT_DEADLINE_MISS;}
 t->last_stack_headroom=headroom; if(headroom<t->stack_warning_threshold){t->stack_warnings++;t->events|=OBS_EVENT_STACK_WARNING;}
 do { t->release_ticks+=t->period_ticks; t->deadline_ticks=t->release_ticks+t->relative_deadline_ticks; } while ((uint32_t)(finish-t->deadline_ticks)<0x80000000U);
 return true;
}
bool observatory_record_stack(observatory_t *o,uint16_t id,uint32_t headroom) { observatory_task_t *t=find_task(o,id); if(t==NULL)return false; t->last_stack_headroom=headroom; if(headroom<t->stack_warning_threshold){t->stack_warnings++;t->events|=OBS_EVENT_STACK_WARNING;} return true; }
bool observatory_get_snapshot(const observatory_t *o,uint16_t id,observatory_snapshot_t *s) {
 const observatory_task_t *t=find_task_const(o,id); if(t==NULL||s==NULL)return false; s->task_id=t->task_id;s->completed_jobs=t->completed_jobs;s->deadline_misses=t->deadline_misses;s->execution_budget_violations=t->execution_budget_violations;s->stack_warnings=t->stack_warnings;s->sample_count=t->sample_count;s->execution_min_ticks=t->sample_count==0U?0U:t->execution_min_ticks;s->execution_max_ticks=t->execution_max_ticks;s->execution_average_ticks=t->sample_count==0U?0U:(uint32_t)(t->execution_sum_ticks/t->sample_count);s->last_execution_ticks=t->last_execution_ticks;s->last_stack_headroom=t->last_stack_headroom;s->event_flags=t->events;return true;
}
uint32_t observatory_event_flags(const observatory_t *o,uint16_t id){const observatory_task_t*t=find_task_const(o,id);return t==NULL?OBS_EVENT_INVALID_SAMPLE:t->events;}
void observatory_clear_events(observatory_t *o,uint16_t id,uint32_t mask){observatory_task_t*t=find_task(o,id);if(t!=NULL)t->events&=~mask;}
