#include <stdint.h>
#include <string.h>
#include <benchmark/benchmark.h>

#include <ngx_timer.h>

static void BM_timer_add_sametime(benchmark::State& state) {
    ngx_timer_t timer;
    int rc = timer_init(&timer);

    int n = state.range(0);
    int m = state.range(1);

    timer_entry_t *entries = new timer_entry_t[n + m];
    for (int i = 0; i < n; i++) {
        timer_entry_t *te = &entries[i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)i;
        timer_add(&timer, &entries[i], 100L);
    }
    for (int i = 0; i < m; i++) {
        timer_entry_t *te = &entries[n + i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)(n + i);
    }
    int items = 0;
    for (auto _ : state) {
        for (int i = 0; i < m; i++) {
            timer_add(&timer, &entries[n + i], 100L);
        }
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < m; i++) {
            timer_cancel(&timer, &entries[n + i]);
        }
        items += m;
        state.ResumeTiming(); // And resume timers. They are now counting again.
    }

    state.SetItemsProcessed(items);
    delete[] entries;
}

static void BM_timer_add_difftime(benchmark::State& state) {
    ngx_timer_t timer;
    int rc = timer_init(&timer);

    int n = state.range(0);
    int m = state.range(1);

    timer_entry_t *entries = new timer_entry_t[n + m];
    for (int i = 0; i < n; i++) {
        timer_entry_t *te = &entries[i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)i;
        timer_add(&timer, &entries[i], 100L);
    }
    for (int i = 0; i < m; i++) {
        timer_entry_t *te = &entries[n + i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)(n + i);
    }
    int items = 0;
    for (auto _ : state) {
        for (int i = 0; i < m; i++) {
            timer_add(&timer, &entries[n + i], (uint64_t)i);
        }
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < m; i++) {
            timer_cancel(&timer, &entries[n + i]);
        }
        items += m;
        state.ResumeTiming(); // And resume timers. They are now counting again.
    }

    state.SetItemsProcessed(items);
    delete[] entries;
}

static void BM_timer_del(benchmark::State& state) {
    ngx_timer_t timer;
    int rc = timer_init(&timer);

    int n = state.range(0);
    int m = state.range(1);

    timer_entry_t *entries = new timer_entry_t[n + m];
    for (int i = 0; i < n; i++) {
        timer_entry_t *te = &entries[i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)i;
        timer_add(&timer, &entries[i], 100L);
    }
    for (int i = 0; i < m; i++) {
        timer_entry_t *te = &entries[n + i];
        te->timer = {0};
        te->handler = NULL,
        te->data = (void *)(uint64_t)(n + i);
    }
    int items = 0;
    for (auto _ : state) {
        state.PauseTiming(); // Stop timers. They will not count until they are resumed.
        for (int i = 0; i < m; i++) {
            timer_add(&timer, &entries[n + i], (uint64_t)i);
        }
        items += m;
        state.ResumeTiming(); // And resume timers. They are now counting again.

        for (int i = 0; i < m; i++) {
            timer_cancel(&timer, &entries[n + i]);
        }
    }

    state.SetItemsProcessed(items);
    delete[] entries;
}

BENCHMARK(BM_timer_add_sametime)->RangeMultiplier(1<<2)->Ranges({{1<<10, 1<<20}, {64, 64}});
BENCHMARK(BM_timer_add_difftime)->RangeMultiplier(1<<2)->Ranges({{1<<10, 1<<20}, {64, 64}});
BENCHMARK(BM_timer_del)->RangeMultiplier(1<<2)->Ranges({{1<<10, 1<<20}, {64, 64}});

BENCHMARK_MAIN();
