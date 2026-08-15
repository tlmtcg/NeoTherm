#ifndef THERMOSTAT_CONTROL_H
#define THERMOSTAT_CONTROL_H

void thermostat_compute_request(float natural_10min, float heated_10min, bool prediction_valid);
void thermostat_apply_relay(void);
void thermostat_update_status(void);

#endif