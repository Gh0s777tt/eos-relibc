// Regression test for the relibc static-TLS layout bug fixed in this branch
// (E-OS R-402a): on aarch64 every thread crashed on exit, and on x86_64 every
// thread whose executable had an awkwardly-aligned TLS segment crashed on exit,
// because the static-TLS block was placed where the access model does not read
// it. Two observable consequences, both covered here:
//
//   1. Thread-local *initializers* (.tdata) were not seen by spawned threads:
//      read as zero on aarch64 (start- vs end-relative Master::offset), and
//      shifted on x86_64 (distance-from-end ignored PT_TLS alignment).
//   2. The pthread cleanup-stack and key-destructor walks that run at thread
//      exit read TLS-resident bookkeeping (e.g. CLEANUP_LL_HEAD); with the block
//      misplaced they walked a garbage list and faulted near-null.
//
// A correct libc passes (prints OK, exits 0). A libc with the bug either fails
// an initializer assertion or crashes when a worker thread exits.

#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialized thread-locals (.tdata). These must survive, unchanged, into a
// freshly spawned thread's TLS image.
static _Thread_local int tl_int = 0x12345678;
static _Thread_local unsigned long long tl_u64 = 0xCAFEF00DDEADBEEFULL;
// A high alignment forces the module's PT_TLS p_align above the size of the
// preceding members, the exact placement condition that displaced .tdata on
// x86_64. The runtime address must honour the requested alignment.
static _Thread_local _Alignas(64) char tl_buf[24] = "tls-regression-canary!!";

#define NTHREADS 8

static pthread_key_t key;
static atomic_int dtor_runs = 0;
static atomic_int cleanup_runs = 0;

static void key_dtor(void *v) {
    assert(v == (void *)0x5151);
    atomic_fetch_add(&dtor_runs, 1);
}

static void cleanup_handler(void *v) {
    assert(v == (void *)0x4242);
    atomic_fetch_add(&cleanup_runs, 1);
}

static void *worker(void *arg) {
    long id = (long)arg;

    // (1) .tdata initializers must be intact in this thread.
    assert(tl_int == 0x12345678);
    assert(tl_u64 == 0xCAFEF00DDEADBEEFULL);
    assert(memcmp(tl_buf, "tls-regression-canary!!", 24) == 0);
    assert(((uintptr_t)tl_buf % 64) == 0);

    // Each thread owns its copy.
    tl_int += (int)id;
    assert(tl_int == 0x12345678 + (int)id);

    // (2) Exercise the cleanup-stack and key-destructor machinery that runs at
    // thread exit -- the path that used to fault on a misplaced TLS block.
    assert(pthread_setspecific(key, (void *)0x5151) == 0);
    pthread_cleanup_push(cleanup_handler, (void *)0x4242);
    pthread_cleanup_pop(1); // run it now

    return NULL; // returning triggers the key destructor + cleanup-stack walk
}

int main(void) {
    assert(pthread_key_create(&key, key_dtor) == 0);

    pthread_t t[NTHREADS];
    for (long i = 0; i < NTHREADS; i++) {
        assert(pthread_create(&t[i], NULL, worker, (void *)i) == 0);
    }
    for (int i = 0; i < NTHREADS; i++) {
        assert(pthread_join(t[i], NULL) == 0);
    }

    // The main thread's TLS is independent and still holds its initializer.
    assert(tl_int == 0x12345678);
    assert(tl_u64 == 0xCAFEF00DDEADBEEFULL);
    assert(atomic_load(&cleanup_runs) == NTHREADS);
    assert(atomic_load(&dtor_runs) == NTHREADS);
    assert(pthread_key_delete(key) == 0);

    printf("tls_initexit: %d threads ok, %d cleanups, %d destructors\n",
           NTHREADS, atomic_load(&cleanup_runs), atomic_load(&dtor_runs));
    return EXIT_SUCCESS;
}
