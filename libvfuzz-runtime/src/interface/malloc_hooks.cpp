#include <sensor/driver.h>
#include <stddef.h>
#include <malloc.h>
#include <util/allocationtracker.h>
#include <memory>
#include <interface/malloc_hooks.h>

bool malloc_hooks_in_lib = false;
//const void* InvalidPtr = (void*)0x12;

static void *malloc_hook_malloc (size_t, const void *);
static void* malloc_hook_realloc (void*, size_t, const void *);
static void malloc_hook_free (void*, const void *);
static void *(*old_malloc_hook) (size_t, const void *) = NULL;
static void *(*old_realloc_hook) (void*, size_t, const void *) = NULL;
static void (*old_free_hook) (void*, const void *) = NULL;

void install_malloc_hooks(void)
{
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    __malloc_hook = malloc_hook_malloc;
    __realloc_hook = malloc_hook_realloc;
    __free_hook = malloc_hook_free;
}

static void* malloc_hook_malloc(size_t size, const void *caller)
{
    if ( size == 0 ) {
        return (void*)0x12;
    }

    void *result;
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __free_hook = old_free_hook;
    result = malloc(size);
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    if ( malloc_hooks_in_lib == false ) {
        vfuzz::sensor::g_sensor_driver->MallocHook((size_t)caller, result, size);
    }
    __malloc_hook = malloc_hook_malloc;
    __realloc_hook = malloc_hook_realloc;
    __free_hook = malloc_hook_free;
    return result;
}

static void* malloc_hook_realloc(void* ptr, size_t size, const void *caller)
{
    void *result;
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __free_hook = old_free_hook;
    result = realloc(ptr, size);
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    //vfuzz::sensor::g_sensor_driver->MallocHook((size_t)caller, result, size);
    __malloc_hook = malloc_hook_malloc;
    __realloc_hook = malloc_hook_realloc;
    __free_hook = malloc_hook_free;
    return result;
}

static void malloc_hook_free(void *ptr, const void *caller)
{
    if ( ptr == (void*)0x12 ) {
        return;
    }
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __free_hook = old_free_hook;
    free(ptr);
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_free_hook = __free_hook;
    if ( malloc_hooks_in_lib == false ) {
        vfuzz::sensor::g_sensor_driver->FreeHook((size_t)caller, ptr);
    }
    __malloc_hook = malloc_hook_malloc;
    __realloc_hook = malloc_hook_realloc;
    __free_hook = malloc_hook_free;
}
