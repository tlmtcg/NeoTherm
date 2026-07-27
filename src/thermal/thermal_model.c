#include "thermal_model.h"

#include "logger.h"

typedef struct
{
    float outside_temperature;

    float heat_power;

    float loss_factor;

    float thermal_mass;

} thermal_model_t;

static thermal_model_t s_model =
{
    .outside_temperature = 5.0f,  // température extérieure
    .heat_power          = 0.25f, // Gain de température lorsque le chauffage est actif
    .loss_factor         = 0.01f, // Coefficient de pertes thermiques
    .thermal_mass        = 8.0f   // Inertie thermique
};

/*==========================================================
 * Initialisation
 *=========================================================*/

bool thermal_model_init(void)
{
    LOG_INFO("THERMAL",
             "Model : Outside=%.1f C Heat=%.2f C/tick Loss=%.3f Mass=%.1f",
             s_model.outside_temperature,
             s_model.heat_power,
             s_model.loss_factor,
             s_model.thermal_mass);

    return true;
}

/*==========================================================
 * Calcul thermique
 *=========================================================*/

float thermal_model_update(
    float inside_temperature,
    bool heating)
{
    /*
     * Refroidissement naturel
     */
    float delta_loss =
        (s_model.outside_temperature - inside_temperature) *
        s_model.loss_factor;

    /*
     * Chauffage
     */
    float delta_heat = 0.0f;

    if (heating)
    {
        delta_heat = s_model.heat_power;
    }

    /*
     * Variation totale
     */
    float delta =
        (delta_loss + delta_heat) /
        s_model.thermal_mass;

    float new_temperature =
        inside_temperature + delta;

    LOG_INFO("THERMAL",
             "Inside=%.2f Outside=%.2f Loss=%+.3f Heat=%+.3f Delta=%+.3f -> %.2f",
             inside_temperature,
             s_model.outside_temperature,
             delta_loss,
             delta_heat,
             delta,
             new_temperature);

    return new_temperature;
}

/*==========================================================
 * Température extérieure
 *=========================================================*/

void thermal_model_set_outside_temperature(
    float temperature)
{
    s_model.outside_temperature = temperature;
}

float thermal_model_get_outside_temperature(void)
{
    return s_model.outside_temperature;
}

/*==========================================================
 * Paramètres
 *=========================================================*/

void thermal_model_set_heat_power(
    float value)
{
    if (value > 0.0f)
    {
        s_model.heat_power = value;
    }
}

void thermal_model_set_loss_factor(
    float value)
{
    if (value > 0.0f)
    {
        s_model.loss_factor = value;
    }
}

void thermal_model_set_thermal_mass(
    float value)
{
    if (value > 0.1f)
    {
        s_model.thermal_mass = value;
    }
}
