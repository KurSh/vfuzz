#include <atomic>

struct MallocFreeTracer {
    void Start(void) {
        Mallocs = 0;
        Frees = 0;
        started = true;
        paused = false;
    }

    void Pause(void) {
        if ( started == false ) {
            return;
        }
        paused = true;
    }

    void Resume(void) {
        if ( started == false ) {
            return;
        }
        paused = false;
    }

    void AddMalloc(void) {
        if ( paused == true ) {
            return;
        }
        Mallocs++;
    }

    void AddFree(void) {
        if ( paused == true ) {
            return;
        }
        Frees++;
    }

    // Returns true if there were more mallocs than frees.
    bool Stop() {
        bool Result = Mallocs > Frees;
        Mallocs = 0;
        Frees = 0;
        started = false;
        return Result;
    }

    bool started, paused;
    std::atomic<size_t> Mallocs;
    std::atomic<size_t> Frees;
};
