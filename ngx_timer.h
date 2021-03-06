#ifndef _NGX_TIMER_H_INCLUDED_
#define _NGX_TIMER_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <ngx_rbtree.h>

typedef struct ngx_timer_s ngx_timer_t;
typedef struct timer_entry_s ngx_timer_entry_t;
typedef void (*timer_handler_pt)(ngx_timer_entry_t *ev, void *ctx);

struct timer_entry_s {
    ngx_rbtree_node_t timer;
    void *data;
};
struct ngx_timer_s {
    timer_handler_pt handler;
    ngx_rbtree_t rbtree;
    ngx_rbtree_node_t sentinel;
};

int ngx_timer_init(ngx_timer_t *timer, timer_handler_pt handler);

// 添加定时任务
void ngx_timer_add(ngx_timer_t *timer, ngx_timer_entry_t *te, uint64_t expire_time);

// 取消定时
void ngx_timer_cancel(ngx_timer_t *timer, ngx_timer_entry_t *te);

// 执行到期任务
#define ngx_timer_tick(timer, now, ctx) ngx_timer_tick_limit(timer, now, ctx, 0)

void ngx_timer_tick_limit(ngx_timer_t *timer, uint64_t now, void *ctx, int limit);

#ifdef __cplusplus
}
#endif

#endif /* _NGX_TIMER_H_INCLUDED_ */
