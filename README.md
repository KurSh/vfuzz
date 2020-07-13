# vfuzz

(previously called VrankenFuzz).

I don't claim superiority over other engines in performance or efficiency out of the box, but this does implement some features that I felt where lacking elsewhere.

## Custom generators

Fetch any type of data from anywhere in your harness. The single byte array input provided by AFL/libFuzzer is a crude instrument for constructing complex objects in your harness. Protobufs are used to work around this, but there is no reason to not provide multiple data streams at the engine level.

## Custom sensors

Rather than just code coverage, use any quantifier. Code coverage is quite useful for most purposes, but not always sufficient for exploring specific corner cases.

You can use any ```uint64_t``` value in combination with one of several conditions (value higher than previous, value lower than previous, # of unique values) (which I call Processors) to add inputs to the corpus.

Custom sensors should make it easy to implement fuzzing non-LLVM-based languages, provided that you can extract a coverage signal from that language's interpreter or runtime. libFuzzer's equivalent of custom sensors is "extra counters", and I and several other people used this feature to implement libFuzzer support for [go-fuzz](https://github.com/dvyukov/go-fuzz) (which in turn made Go fuzzing on [OSS-Fuzz](https://github.com/google/oss-fuzz) possible) with minimal effort.

The library also comes bundled with some built-in sensors that are a bridge between instrumentation and system calls and the rest of the library:

### kSensorBuiltinCodeCoverage

Add to corpus if total of unique PC's observed is higher than before.

### kSensorBuiltinStackDepth

Add to corpus if stack pointer observed is lower than before. Useful for finding stack overflows.

### kSensorBuiltinStackUnique

Add to corpus if total of unique stack pointers observed is higher than before. Useful for finding stack overflows.

### kSensorBuiltinIntensity

Add to corpus if total of non-unique PC's observed is higher than before. Useful for finding slow inputs.

### kSensorBuiltinAllocSingleMax

Add to corpus if ```malloc()``` size from a particular location is higher than before. Useful for finding heap exhaustion inputs.

### kSensorBuiltinAllocGlobalMax

Add to corpus if peak concurrent heap usage is higher than before. Useful for finding heap exhaustion inputs.

### kSensorBuiltinAutoCodeIntensity

Add to corpus if the number of times a particular PC is executed during a single run is higher than before. Useful for finding slow inputs.

## Components

The library is split into two components.

### libvfuzz-core

This implements core fuzzer functionality like corpora, dictionaries, mutators, sensors and generators and is agnostic with respect to target-specific interfaces like instrumentation. You could use this to build new fuzzer engines, experiment with features like new mutators, implement fuzzing for other languages etc.

### libfuzzer-runtime

This is a bridge between both the user and the system (including instrumentation) one one hand, and libvfuzz-core on the other hand.

## State of the project

This is alpha-grade software. I can't provide much support but PR's are very welcome. This has only been tested and used on 64 bit Linux.

## Compilation

A recent version of Clang is required.

From the project's top-level directory:

```sh
$ mkdir build/
$ cd build/
$ cmake -DCMAKE_CXX_COMPILER=clang++-8 -DCMAKE_C_COMPILER=clang-8 ..
$ make -j$(nproc)
```

## Quick start guide

```sh
$ mkdir corpus/
$ examples/nlohmann corpus/
20:52:23 [INFO] Running 0/1
20:52:23 [INFO] Corpus size is 1
20:52:26 [PULSE] N 37315 E/s 12438 RSS 5
20:52:29 [PULSE] N 73424 E/s 12237 RSS 5
20:52:32 [PULSE] N 110315 E/s 12257 RSS 5
20:52:35 [PULSE] N 148119 E/s 12343 RSS 5
# Note that no files are added to corpus.
# Let's fix that by enabling the sensor vfuzz::kSensorBuiltinCodeCoverage in examples/nlohmann.cpp
$ emacs ../examples/nlohmann.cpp
$ make
$ examples/nlohmann corpus/
20:57:53 [INFO] Running 0/1
20:57:53 [INFO] Corpus size is 2
20:57:53 [CHANGE] N 5 E/s 5 RSS 4, C 2 -> 3 | code coverage 161 -> 176 (15) < GID #0: InsertDictionary (autodict)(00)
20:57:53 [INFO] Writing corpus/42a629a4d4ef88d7e7cd9749276ca7e2d6c712b7
20:57:53 [CHANGE] N 7 E/s 7 RSS 4, C 3 -> 4 | code coverage 176 -> 178 (2) < GID #0: InsertDictionary (autodict)(00)-ShuffleBytes(00)-NotRanged(00-0)-InsertByte(4B)
20:57:53 [INFO] Writing corpus/8fa6d843356c0c7862a26d29f52103ef5a5c8287
20:57:53 [CHANGE] N 17 E/s 17 RSS 4, C 4 -> 5 | code coverage 178 -> 204 (26) < GID #0: NotRanged(00-1)
20:57:53 [INFO] Writing corpus/a35763d53b92bbf0274e14b1eea7c6a1ab44dce0
20:57:53 [CHANGE] N 28 E/s 28 RSS 4, C 5 -> 6 | code coverage 204 -> 210 (6) < GID #0: Copy(00)-ReverseByte-Or(1C)
# Now experiment with the different sensors listed in examples/nlohmann.cpp - see descriptions above.
```

## License

MIT
