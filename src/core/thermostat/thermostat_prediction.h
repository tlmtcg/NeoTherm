#ifndef THERMOSTAT_PREDICTION_H
#define THERMOSTAT_PREDICTION_H
void thermostat_update_prediction(float *natural_10min, float *heated_10min, bool *prediction_valid);
#endif