#include <ngx_timer.h>

// 初始化
ngx_inline int ngx_timer_init(ngx_timer_t *timer) {
    ngx_rbtree_init(&timer->rbtree, &timer->sentinel,
                    ngx_rbtree_insert_timer_value);
    return 0;
}

// 添加定时任务
ngx_inline void ngx_timer_add(ngx_timer_t *timer, ngx_timer_entry_t *te, uint64_t expire_time) {
    te->timer.key = expire_time;
    ngx_rbtree_insert(&timer->rbtree, &te->timer);
}

// 取消定时
ngx_inline void ngx_timer_cancel(ngx_timer_t *timer, ngx_timer_entry_t *te) {
    ngx_rbtree_delete(&timer->rbtree, &te->timer);
}

// 依次执行到期任务
ngx_inline void ngx_timer_tick(ngx_timer_t *timer, uint64_t now, void *ctx) {
    ngx_timer_entry_t *te;
    ngx_rbtree_node_t *node, *root, *sentinel;
    sentinel = timer->rbtree.sentinel;

    for (;;) {
        root = timer->rbtree.root;
        if (root == sentinel) {
            return;
        }
        node = ngx_rbtree_min(root, sentinel);
        if ((ngx_rbtree_key_int_t)(node->key - now) > 0) {
            return;
        }
        te = ngx_rbtree_data(node, ngx_timer_entry_t, timer);
        ngx_rbtree_delete(&timer->rbtree, &te->timer);
        te->handler(te, ctx);
        // free(te);
    }
}
