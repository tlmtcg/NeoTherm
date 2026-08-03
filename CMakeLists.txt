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
    src/system/system_init.c
    src/logger/logger.c
    src/app_config/app_config.c
    src/app_config/ini.c
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
    src/tests/test_ini.c
    src/tests/test_app_config.c
    src/tests/test_event.c
    src/tests/test_logger.c
    src/tests/test_thermal_model.c
    src/runtime/runtime.c
    src/event_dispatcher/event_dispatcher.c
    src/app_config/loaders/app_config_thermal.c
    src/app_config/loaders/app_config_logger.c
    src/app_config/loaders/app_config_load_relay.c
    src/app_config/loaders/app_config_load_scheduler.c
    src/app_config/loaders/app_config_load_files.c
    src/app_config/loaders/app_config_load_history.c
    src/app_config/loaders/app_config_load_schedule.c
    src/app_config/loaders/app_config_load_climate.c
    src/app_config/loaders/app_config_load_debug.c
    src/app_config/parser/ini_parser.c
    src/app_config/setters/ini_setters.c
    src/app_config/getters/ini_getters.c
    src/app_config/utils/ini_utils.c
    src/app_config/storage/ini_storage.c
    src/alarm/alarm.c
    src/alarm/alarm_runtime.c
    src/tests/test_alarm.c
    src/tests/test_alarm_runtime.c
    src/tests/test_trim.c
    src/console/console.c
    src/console/console_commands.c
    src/console/commands/cmd_time.c
    src/console/commands/cmd_alarm.c
    src/console/commands/cmd_status.c
    src/console/console_utils.c
    src/console/commands/cmd_temp.c
    src/console/commands/cmd_mode.c
    src/console/commands/cmd_relay.c
    src/console/commands/cmd_logger.c
    src/console/commands/cmd_outside.c
    src/console/commands/cmd_setpoint.c
       src/console/commands/cmd_hyst.c
)


# ==========================================================
# Répertoires d'inclusion
# ==========================================================

target_include_directories(
    NeoTherm
    PRIVATE
        src/app
        src/logger
        src/app_config
        src/debug
        src/event
        src/event_dispatcher
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
        src/system
        src/alarm
        src/console
)

# ==========================================================
# Défines
# ==========================================================

target_compile_definitions(
    NeoTherm
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
        # UNIT_TEST
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
