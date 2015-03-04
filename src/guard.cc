#include <node.h>
#include <uv.h>
#include <chrono>
#include <thread>

#define SENTRY_DEBUG 0
#define SENTRY_SLEEP_MS 20
#define SENTRY_EXIT_CODE_TIME -1
#define SENTRY_EXIT_CODE_MEMORY -2

using namespace std;
using namespace std::chrono;
using namespace v8;

static thread *sentry_thread = 0;
static bool sentry_thread_active = false;

static bool sentry_memory = false;
static size_t memory_start;
static size_t memory_check;

static bool sentry_time = false;
static time_point<high_resolution_clock> time_check;

void sentry_thread_function() {
    while (sentry_thread_active) {

        if (sentry_memory) {
            size_t memory_now;
            uv_resident_set_memory(&memory_now);

            if (memory_now > memory_check) {
                exit(SENTRY_EXIT_CODE_MEMORY);
            }
        }

        if (sentry_time) {
            time_point<high_resolution_clock> time_now = high_resolution_clock::now();

            if (time_now > time_check) {
                exit(SENTRY_EXIT_CODE_TIME);
            }
        }

        milliseconds duration(SENTRY_SLEEP_MS);
        this_thread::sleep_for(duration);
    }
}

void start(long memory_limit, long time_limit) {
    if (memory_limit > 0) {
        sentry_memory = true;

        // Memory limit takes into account the current memory footprint
        uv_resident_set_memory(&memory_start);
        memory_check = memory_start + memory_limit;
    } else {
        sentry_memory = false;
    }

    if (time_limit > 0) {
        sentry_time = true;

        // Time check starts from now
        time_check = high_resolution_clock::now() + milliseconds(time_limit);;
    } else {
        sentry_time = false;
    }

    // Start the sentry thread
    sentry_thread_active = true;
    sentry_thread = new thread(sentry_thread_function);
}

void stop() {
    if (sentry_thread_active) {
        // Stop the thread and wait for it to exit
        sentry_thread_active = false;
        sentry_thread->join();

        delete sentry_thread;
    }
}

void js_start(const FunctionCallbackInfo<Value>& args) {
    Isolate *isolate = Isolate::GetCurrent();

    if (args.Length() < 2) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
        return;
    }

    int memory_limit_mb = args[0]->IntegerValue();
    int time_limit = args[1]->IntegerValue();

    if (sentry_thread_active) {
        stop();
    }

    start(memory_limit_mb * 1000000, time_limit);

    args.GetReturnValue().Set(Undefined(isolate));
}

void js_stop(const FunctionCallbackInfo<Value>& args) {
    Isolate *isolate = Isolate::GetCurrent();

    stop();

    args.GetReturnValue().Set(Undefined(isolate));
}

void js_memory_usage(const FunctionCallbackInfo<Value>& args) {
    Isolate *isolate = Isolate::GetCurrent();

    size_t memory;
    uv_resident_set_memory(&memory);

    args.GetReturnValue().Set(Integer::New(isolate, memory));
}

void init(Handle<Object> exports) {
    NODE_SET_METHOD(exports, "start", js_start);
    NODE_SET_METHOD(exports, "stop", js_stop);
    NODE_SET_METHOD(exports, "memoryUsage", js_memory_usage);
}

NODE_MODULE(guard, init)
