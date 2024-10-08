# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(QT_BUILD_WASM_BATCHED_TESTS) # not all tests currently work in WebAssembly
    add_subdirectory(qatomicint)
    add_subdirectory(qatomicinteger)
    add_subdirectory(qatomicpointer)
    add_subdirectory(qfuturesynchronizer)
    add_subdirectory(qmutexlocker)
    add_subdirectory(qreadlocker)
    add_subdirectory(qresultstore)
    add_subdirectory(qwritelocker)
    return()
endif()

if(QT_FEATURE_thread)
    add_subdirectory(qatomicint)
    add_subdirectory(qatomicinteger)
    add_subdirectory(qatomicpointer)
    if(QT_FEATURE_future)
        if(QT_FEATURE_concurrent AND NOT INTEGRITY)
            add_subdirectory(qfuture)
        endif()
        add_subdirectory(qresultstore)
        add_subdirectory(qfuturesynchronizer)
        if(NOT INTEGRITY)
            add_subdirectory(qpromise)
        endif()
        # QTBUG-87431
        if(TARGET Qt::Concurrent AND NOT INTEGRITY)
            add_subdirectory(qfuturewatcher)
        endif()
    endif()
    add_subdirectory(qmutex)
    add_subdirectory(qmutexlocker)
    add_subdirectory(qreadlocker)
    add_subdirectory(qreadwritelock)
    add_subdirectory(qsemaphore)
    # QTBUG-85364
    if(NOT CMAKE_CROSSCOMPILING)
        add_subdirectory(qthread)
    endif()
    add_subdirectory(qthreadonce)
    add_subdirectory(qthreadpool)
    add_subdirectory(qthreadstorage)
    add_subdirectory(qwaitcondition)
    add_subdirectory(qwritelocker)
endif()

