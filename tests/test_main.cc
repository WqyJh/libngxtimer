#include <cstdint>
#include <stdint.h>
#include <thread>
#include <atomic>

#include <gtest/gtest.h>

#include "ngx_timer.h"

// data += 1
void test_expire_handler1(ngx_timer_entry_t *ev, void *ctx) {
    ev->data = (void *)((uint64_t)(ev->data) + 1L);
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

    // Not expire
    ngx_timer_tick(&timer, 50L, NULL);
    EXPECT_EQ(te.data, (void *)1L);

    // Not expire
    ngx_timer_tick(&timer, 80L, NULL);
    EXPECT_EQ(te.data, (void *)1L);

    // Expired and excuted
    ngx_timer_tick(&timer, 100L, NULL);
    EXPECT_EQ(te.data, (void *)2L);

    // Expired and won't be excuted
    ngx_timer_tick(&timer, 200L, NULL);
    EXPECT_EQ(te.data, (void *)2L);
}

TEST(timer, test_cancel) {
    ngx_timer_t timer;
    int rc = ngx_timer_init(&timer);
    EXPECT_EQ(rc, 0);

    ngx_timer_entry_t te = {
        .timer = {0},
        .handler = test_expire_handler1,
        .data = (void *)1L,
    };
    ngx_timer_add(&timer, &te, 100L);

    // Not expire
    ngx_timer_tick(&timer, 50L, NULL);
    EXPECT_EQ(te.data, (void *)1L);

    // Cancel
    ngx_timer_cancel(&timer, &te);

    // Not expire
    ngx_timer_tick(&timer, 80L, NULL);
    EXPECT_EQ(te.data, (void *)1L);

    // Expired and won't be excuted
    ngx_timer_tick(&timer, 100L, NULL);
    EXPECT_EQ(te.data, (void *)1L);

    // Expired and won't be excuted
    ngx_timer_tick(&timer, 200L, NULL);
    EXPECT_EQ(te.data, (void *)1L);
}

void test_handler2(ngx_timer_entry_t *ev, void *ctx) {
    ev->data = (void *)((uint64_t)(ev->data) + 1L);
    *((uint64_t *)ctx) += 1;
}

TEST(timer, test_ctx) {
    ngx_timer_t timer;
    int rc = ngx_timer_init(&timer);
    EXPECT_EQ(rc, 0);

    ngx_timer_entry_t te = {
        .timer = {0},
        .handler = test_handler2,
        .data = (void *)1L,
    };
    ngx_timer_add(&timer, &te, 100L);

    uint64_t ctx = 100L;

    // Not expire
    ngx_timer_tick(&timer, 50L, &ctx);
    EXPECT_EQ(te.data, (void *)1L);
    EXPECT_EQ(ctx, 100L);

    // Expired and excuted
    ngx_timer_tick(&timer, 100L, &ctx);
    EXPECT_EQ(te.data, (void *)2L);
    EXPECT_EQ(ctx, 101L);
}
