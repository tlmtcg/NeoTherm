#include "thermostat.h"
#include "thermostat_internal.h"
#include "../thermal_prediction/thermal_prediction.h"

void thermostat_update_prediction(float *natural_10min, float *heated_10min, bool *prediction_valid)
{
    *prediction_valid = thermal_prediction_is_valid();
    float temperature = s_status.temperature;

    *natural_10min = temperature;
    *heated_10min = temperature;
    float predicted_1h = temperature;

    if (*prediction_valid)
    {
        *natural_10min = thermal_prediction_get_temperature_minutes_state(
            THERMOSTAT_PREDICTION_MINUTES, false);

        *heated_10min = thermal_prediction_get_temperature_minutes_state(
            THERMOSTAT_PREDICTION_MINUTES, true);

        predicted_1h = thermal_prediction_get_temperature_minutes_state(60.0f, true);
    }

    s_status.temp_forecast_1h = predicted_1h;
}
