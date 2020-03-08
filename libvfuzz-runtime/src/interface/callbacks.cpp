#include <sensor/driver.h>
#include <dictionary/autodict.h>
#include <sanitizer/coverage_interface.h>
#include <interface/malloc_hooks.h>

extern bool malloc_hooks_in_lib;
namespace vfuzz {
size_t NumGuards = 0;

template <class T>
void sensor::Driver::HandleCmp(const uintptr_t PC, const T Arg1, const T Arg2) {
    if ( g_sensor_driver == nullptr ) {
        return;
    }
    g_sensor_driver->builtin_HandleCmp(PC, Arg1, Arg2);
}

} /* namespace vfuzz */

extern "C" void __sanitizer_cov_trace_pc_guard_init(uint32_t *start, uint32_t *stop) {
    static uint64_t N;
    if (start == stop || *start) return;
    for (uint32_t *x = start; x < stop; x++) {
        *x = ++N;  // Guards should start from 1.
        vfuzz::NumGuards++;
    }
}

extern "C" void __sanitizer_cov_trace_pc_guard(uint32_t *guard) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        /* Shouldn't happen */
        return;
    }

    if (!*guard) return;
    malloc_hooks_in_lib = true;
    //void *PC = __builtin_return_address(0);
    //vfuzz::sensor::g_sensor_driver->builtin_AddPC((size_t)PC);
    vfuzz::sensor::g_sensor_driver->builtin_AddPC((size_t)*guard);
    vfuzz::sensor::g_sensor_driver->builtin_AddStack();

    vfuzz::sensor::g_sensor_driver->builtin_HandleStacktrace(reinterpret_cast<uintptr_t>(__builtin_return_address(0)));
    malloc_hooks_in_lib = false;
}

extern "C" void __sanitizer_cov_trace_pc_indir(uintptr_t Callee) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }
    uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    vfuzz::sensor::g_sensor_driver->HandleCallerCallee(PC, Callee);
}

template <class T> static void generic_cmp_handler(const uintptr_t PC, const T A, const T B)
{
    malloc_hooks_in_lib = true;
    vfuzz::sensor::Driver::HandleCmp(PC, A, B);
    vfuzz::dictionary::AutoDict::InsertTorc(A, B, sizeof(A));
    malloc_hooks_in_lib = false;
}

#define DEFINE_TRACE_CMP(suffix, type) \
    extern "C" void __sanitizer_cov_trace_##suffix(const type A, const type B) { \
        const uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0)); \
        generic_cmp_handler(PC, A, B); \
    }

DEFINE_TRACE_CMP(cmp8, uint64_t)
DEFINE_TRACE_CMP(const_cmp8, uint64_t)
DEFINE_TRACE_CMP(cmp4, uint32_t)
DEFINE_TRACE_CMP(const_cmp4, uint32_t)
DEFINE_TRACE_CMP(cmp2, uint16_t)
DEFINE_TRACE_CMP(const_cmp2, uint16_t)
DEFINE_TRACE_CMP(cmp1, uint8_t)
DEFINE_TRACE_CMP(const_cmp1, uint8_t)

#undef DEFINE_TRACE_CMP

extern "C" void __sanitizer_cov_trace_switch(uint64_t Val, uint64_t *Cases) {
    uint64_t N = Cases[0];
    uint64_t ValSizeInBits = Cases[1];
    uint64_t *Vals = Cases + 2;
    // Skip the most common and the most boring case.
    if (Vals[N - 1]  < 256 && Val < 256)
        return;
    uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    size_t i;
    uint64_t Token = 0;
    for (i = 0; i < N; i++) {
        Token = Val ^ Vals[i];
        if (Val < Vals[i])
            break;
    }

    if (ValSizeInBits == 16)
        vfuzz::sensor::Driver::HandleCmp(PC + i, static_cast<uint16_t>(Token), (uint16_t)(0));
    else if (ValSizeInBits == 32)
        vfuzz::sensor::Driver::HandleCmp(PC + i, static_cast<uint32_t>(Token), (uint32_t)(0));
    else
        vfuzz::sensor::Driver::HandleCmp(PC + i, Token, (uint64_t)(0));
}

extern "C" void __sanitizer_cov_trace_div4(uint32_t Val) {
    uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    vfuzz::sensor::Driver::HandleCmp(PC, Val, (uint32_t)0);
}

extern "C" void __sanitizer_cov_trace_div8(uint64_t Val) {
    uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    vfuzz::sensor::Driver::HandleCmp(PC, Val, (uint64_t)0);
}

extern "C" void __sanitizer_cov_trace_gep(uintptr_t Idx) {
    uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
    vfuzz::sensor::Driver::HandleCmp(PC, Idx, (uintptr_t)0);
}

extern "C" void __sanitizer_weak_hook_memcmp(void *caller_pc, const void *s1,
        const void *s2, size_t n, int result) {
    if (result == 0) return;  // No reason to mutate.
    if (n <= 1) return;  // Not interesting.
    vfuzz::sensor::g_sensor_driver->AddValueForMemcmp(caller_pc, s1, s2, n, /*StopAtZero*/false);
}

static size_t InternalStrnlen(const char *S, const size_t MaxLen) {
    size_t Len = 0;
    for (; Len < MaxLen && S[Len]; Len++) {}
    return Len;
}

// Finds min of (strlen(S1), strlen(S2)).
// Needed bacause one of these strings may actually be non-zero terminated.
static size_t InternalStrnlen2(const char *S1, const char *S2) {
    size_t Len = 0;
    for (; S1[Len] && S2[Len]; Len++)  {}
    return Len;
}

extern "C" void __sanitizer_weak_hook_strncmp(void *caller_pc, const char *s1,
        const char *s2, size_t n, int result) {
    if (result == 0) return;  // No reason to mutate.
    size_t Len1 = InternalStrnlen(s1, n);
    size_t Len2 = InternalStrnlen(s2, n);
    n = std::min(n, Len1);
    n = std::min(n, Len2);
    if (n <= 1) return;  // Not interesting.
    vfuzz::sensor::g_sensor_driver->AddValueForMemcmp(caller_pc, s1, s2, n, /*StopAtZero*/true);
}

extern "C" void __sanitizer_weak_hook_strcmp(void *caller_pc, const char *s1,
        const char *s2, int result) {
    if (result == 0) return;  // No reason to mutate.
    size_t N = InternalStrnlen2(s1, s2);
    if (N <= 1) return;  // Not interesting.
    vfuzz::sensor::g_sensor_driver->AddValueForMemcmp(caller_pc, s1, s2, N, /*StopAtZero*/true);
}

extern "C" void __cyg_profile_func_enter(void* func, void* caller) {
    if ( vfuzz::sensor::g_sensor_driver == nullptr ) {
        return;
    }

    vfuzz::sensor::g_sensor_driver->builtin_AddFuncEnter((size_t)func, (size_t)caller);
}

extern "C" void __cyg_profile_func_exit(void* func, void* caller) {
    /* TODO */
    (void)func;
    (void)caller;
    return;
}

extern "C" void __sanitizer_cov_8bit_counters_init(uint8_t *Start, uint8_t *Stop) {
    /* TODO */
    (void)Start;
    (void)Stop;
}
