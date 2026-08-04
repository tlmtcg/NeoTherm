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

    src/app_config/app_config.c
    src/app_config/getters/ini_getters.c
    src/app_config/ini.c
    src/app_config/loaders/app_config_load_climate.c
    src/app_config/loaders/app_config_load_debug.c
    src/app_config/loaders/app_config_load_files.c
    src/app_config/loaders/app_config_load_history.c
    src/app_config/loaders/app_config_load_relay.c
    src/app_config/loaders/app_config_load_schedule.c
    src/app_config/loaders/app_config_load_scheduler.c
    src/app_config/loaders/app_config_logger.c
    src/app_config/loaders/app_config_thermal.c
    src/app_config/parser/ini_parser.c
    src/app_config/setters/ini_setters.c
    src/app_config/storage/ini_storage.c
    src/app_config/utils/ini_utils.c
    src/app/app.c
    src/app/main.c
    src/core/alarm/alarm_runtime.c
    src/core/alarm/alarm.c
    src/core/climate/climate.c
    src/core/history/history_csv.c
    src/core/history/history_debug.c
    src/core/history/history_storage.c
    src/core/history/history.c
    src/core/program/program.c
    src/core/program/schedule.c
    src/core/relay/relay.c
    src/core/thermal/thermal_model.c
    src/core/thermostat/thermostat.c
    src/debug/debug.c
    src/infra/clock/clock.c
    src/infra/console/commands/cmd_alarm.c
    src/infra/console/commands/cmd_clear.c
    src/infra/console/commands/cmd_clock.c
    src/infra/console/commands/cmd_config.c
    src/infra/console/commands/cmd_event.c
    src/infra/console/commands/cmd_history.c
    src/infra/console/commands/cmd_hyst.c
    src/infra/console/commands/cmd_logger.c
    src/infra/console/commands/cmd_mode.c
    src/infra/console/commands/cmd_outside.c
    src/infra/console/commands/cmd_program.c
    src/infra/console/commands/cmd_relay.c
    src/infra/console/commands/cmd_runtime.c
    src/infra/console/commands/cmd_setpoint.c
    src/infra/console/commands/cmd_status.c
    src/infra/console/commands/cmd_storage.c
    src/infra/console/commands/cmd_task.c
    src/infra/console/commands/cmd_temp.c
    src/infra/console/commands/cmd_thermal.c
    src/infra/console/commands/cmd_thermostat.c
    src/infra/console/commands/cmd_tick.c
    src/infra/console/commands/cmd_version.c
    src/infra/console/console_commands.c
    src/infra/console/console_utils.c
    src/infra/console/console.c
    src/infra/event_dispatcher/event_dispatcher.c
    src/infra/event/event.c
    src/infra/logger/logger.c
    src/infra/runtime/runtime.c
    src/infra/scheduler/scheduler_debug.c
    src/infra/scheduler/scheduler.c
    src/infra/storage/storage_ini.c
    src/infra/storage/storage.c
    src/system/system_init.c
    src/tests/test_alarm_runtime.c
    src/tests/test_alarm.c
    src/tests/test_app_config.c
    src/tests/test_climate.c
    src/tests/test_clock.c
    src/tests/test_event.c
    src/tests/test_history.c
    src/tests/test_ini.c
    src/tests/test_logger.c
    src/tests/test_program.c
    src/tests/test_relay.c
    src/tests/test_runner.c
    src/tests/test_runtime.c
    src/tests/test_scheduler.c
    src/tests/test_storage.c
    src/tests/test_thermal_model.c
    src/tests/test_thermostat.c
    src/tests/test_trim.c
    src/tests/test_utils.c
)


# ==========================================================
# Répertoires d'inclusion
# ==========================================================

target_include_directories(
    NeoTherm
    PRIVATE
        src/app
        src/app_config
        src/infra/clock
        src/infra/console
        src/core/alarm
        src/core/climate
        src/core/history
        src/core/program
        src/core/relay
        src/core/thermal
        src/core/thermostat
        src/debug
        src/infra/event
        src/infra/event_dispatcher
        src/infra/logger
        src/infra/runtime
        src/infra/scheduler
        src/infra/storage
        src/system
        src/tests
)

# ==========================================================
# Défines
# ==========================================================

target_compile_definitions(
    NeoTherm
    PRIVATE
        APP_VERSION="${PROJECT_VERSION}"
        UNIT_TEST
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
