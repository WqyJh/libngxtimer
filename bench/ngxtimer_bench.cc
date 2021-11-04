#include <stdint.h>
#include <string.h>
#include <benchmark/benchmark.h>

#include <ngx_timer.h>

static void BM_timer_add_sametime(benchmark::State& state) {
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

BENCHMARK(BM_timer_add_sametime)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_add_difftime)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);
BENCHMARK(BM_timer_del)->RangeMultiplier(1<<2)->Range(1<<10, 1<<20);

BENCHMARK_MAIN();
