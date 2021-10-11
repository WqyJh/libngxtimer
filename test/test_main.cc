#include <cstdint>
#include <thread>
#include <atomic>

#include <gtest/gtest.h>

#include "ngx_timer.h"

void test_expire_handler1(ngx_timer_entry_t *ev, void *ctx) {
    ev->data = (void *)2L;
}

TEST(timer, test_expire) {
    ngx_timer_t timer;
    int rc = ngx_timer_init(&timer);
    EXPECT_EQ(rc, 0);

    ngx_timer_entry_t te = {
        .timer = {0},
        .handler = test_expire_handler1,
        .data = (void *)1L,
    };
    ngx_timer_add(&timer, &te, 100L);

    ngx_timer_tick(&timer, 50L, NULL);
    EXPECT_EQ(te.data, (void *)1L);

    ngx_timer_tick(&timer, 100L, NULL);
    EXPECT_EQ(te.data, (void *)2L);
}
