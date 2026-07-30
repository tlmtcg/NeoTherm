cmake_minimum_required(VERSION 3.20)

project(
    NeoTherm
    VERSION 0.1.0
    DESCRIPTION "NeoTherm Simulator"
    LANGUAGES C
)

# ==========================================================
# Standard C
# ==========================================================

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Autorise les extensions GNU (__VA_ARGS__, etc.)
# Très proche du mode de compilation utilisé par ESP-IDF.
set(CMAKE_C_EXTENSIONS ON)

# ==========================================================
# Exécutable
# ==========================================================

add_executable(
    NeoTherm

    src/app/main.c
    src/app/app.c

    src/logger/logger.c
    src/config/config.c
    src/debug/debug.c
    src/event/event.c
    src/climate/climate.c
    src/scheduler/scheduler.c
    src/scheduler/scheduler_debug.c
    src/relay/relay.c
    src/thermostat/thermostat.c
    src/storage/storage.c
    src/storage/storage_ini.c
    src/thermal/thermal_model.c
    src/history/history.c
    src/history/history_debug.c
    src/history/history_csv.c
    src/history/history_storage.c
    src/clock/clock.c
    src/program/program.c
    src/program/schedule.c
    src/tests/test_runner.c
    src/tests/test_thermostat.c
    src/tests/test_storage.c
    src/tests/test_scheduler.c
    src/tests/test_program.c
    src/tests/test_history.c
    src/tests/test_clock.c
    src/tests/test_climate.c
    src/tests/test_utils.c
    src/tests/test_relay.c
    src/tests/test_runtime.c
    src/runtime/runtime.c
)

# ==========================================================
# Répertoires d'inclusion
# ==========================================================

target_include_directories(
    NeoTherm
    PRIVATE
        src/app
        src/logger
        src/config
        src/debug
        src/event
        src/climate
        src/scheduler
        src/relay
        src/thermostat
        src/storage
        src/thermal
        src/history
        src/clock
        src/program
        src/tests
        src/runtime
)

# ==========================================================
# Défines
# ==========================================================

target_compile_definitions(
    NeoTherm
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
        DEBUG
)

# ==========================================================
# Options de compilation
# ==========================================================

if (MSVC)

    target_compile_options(
        NeoTherm
        PRIVATE
            /W4
    )

else()

    target_compile_options(
        NeoTherm
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )

endif()
