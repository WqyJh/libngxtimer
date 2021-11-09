#include <stdint.h>
#include <string.h>
#include <benchmark/benchmark.h>

#include <ngx_timer.h>

#define EXPECT_TRUE(v1) assert(v1)
#define EXPECT_EQ(v1, v2) assert((v1) == (v2))

// #define ITERATIONS 1000000

static void BM_timer_add_sametime(benchmark::State& state) {
    ngx_timer_t timer;
    int rc = ngx_timer_init(&timer);

    const int n = state.range(0);

    ngx_timer_entry_t *entries = new ngx_timer_entry_t[n];
    for (int i = 0; i < n; i++) {
        ngx_timer_entry_t *te = &entries[i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)i;
    }
    int items = 0;
    while (state.KeepRunningBatch(n)) {
        for (int i = 0; i < n; i++) {
            ngx_timer_add(&timer, &entries[i], 100L);
        }
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < n; i++) {
            ngx_timer_cancel(&timer, &entries[i]);
        }
        items += n;
        state.ResumeTiming(); // And resume timers. They are now counting again.
    }

    state.SetItemsProcessed(items);
    delete[] entries;
}

static void BM_timer_add_difftime(benchmark::State& state) {
    ngx_timer_t timer;
    int rc = ngx_timer_init(&timer);

    int n = state.range(0);

    ngx_timer_entry_t *entries = new ngx_timer_entry_t[n];
    for (int i = 0; i < n; i++) {
        ngx_timer_entry_t *te = &entries[i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)i;
    }
    int items = 0;
    while (state.KeepRunningBatch(n)) {
        for (int i = 0; i < n; i++) {
            ngx_timer_add(&timer, &entries[i], (uint64_t)i);
        }
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < n; i++) {
            ngx_timer_cancel(&timer, &entries[i]);
        }
        items += n;
        state.ResumeTiming(); // And resume timers. They are now counting again.
    }

    state.SetItemsProcessed(items);
    delete[] entries;
}

static void BM_timer_del(benchmark::State& state) {
    ngx_timer_t timer;
    int rc = ngx_timer_init(&timer);

    int n = state.range(0);

    ngx_timer_entry_t *entries = new ngx_timer_entry_t[n];
    for (int i = 0; i < n; i++) {
        ngx_timer_entry_t *te = &entries[i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)i;
    }

    int items = 0;
    while (state.KeepRunningBatch(n)) {
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < n; i++) {
            ngx_timer_add(&timer, &entries[i], (uint64_t)i);
        }
        items += n;
        state.ResumeTiming(); // And resume timers. They are now counting again.

        for (int i = 0; i < n; i++) {
            ngx_timer_cancel(&timer, &entries[i]);
        }
    }

    state.SetItemsProcessed(items);
    delete[] entries;
}

void expire_handler(ngx_timer_entry_t *ev, void *ctx) {
    EXPECT_EQ(ev->data, ctx);
}

static void BM_timer_expire(benchmark::State& state) {
    ngx_timer_t timer;
    int rc = ngx_timer_init(&timer);

    int n = state.range(0);

    ngx_timer_entry_t *entries = new ngx_timer_entry_t[n];
    for (int i = 0; i < n; i++) {
        ngx_timer_entry_t *te = &entries[i];
        te->timer = {0};
        te->handler = expire_handler,
        te->data = te;
    }

    int items = 0;
    while (state.KeepRunningBatch(n)) {
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < n; i++) {
            ngx_timer_add(&timer, &entries[i], (uint64_t)i);
        }
        items += n;
        state.ResumeTiming(); // And resume timers. They are now counting again.

        for (int i = 0; i < n; i++) {
            ngx_timer_tick(&timer, i, &entries[i]);
        }
    }

    state.SetItemsProcessed(items);
    delete[] entries;
}

// BENCHMARK(BM_timer_add_sametime)->Arg(ITERATIONS);
// BENCHMARK(BM_timer_add_difftime)->Arg(ITERATIONS);
// BENCHMARK(BM_timer_del)->Arg(ITERATIONS);

BENCHMARK(BM_timer_add_sametime)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_add_difftime)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_del)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_expire)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);

BENCHMARK_MAIN();
