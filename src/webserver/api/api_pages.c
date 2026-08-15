#include "api_pages.h"

#ifdef _WIN32

#include "webserver_http.h"

static void api_pages_send_html(
    SOCKET client_socket,
    const char *html)
{
    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "text/html; charset=utf-8",
        html);
}

/*==========================================================
 * GET /
 *=========================================================*/

void api_pages_handle_index(
    SOCKET client_socket)
{
    static const char html[] =

        "<!DOCTYPE html>"
        "<html lang=\"fr\">"

        "<head>"

        "<meta charset=\"UTF-8\">"

        "<meta name=\"viewport\" "
        "content=\"width=device-width,initial-scale=1.0\">"

        "<title>NeoTherm</title>"

        "<style>"

        "* { box-sizing: border-box; }"

        "body {"
        "  margin: 0;"
        "  font-family: Arial, sans-serif;"
        "  background: #111827;"
        "  color: #f9fafb;"
        "}"

        "header {"
        "  padding: 20px;"
        "  background: #1f2937;"
        "  border-bottom: 1px solid #374151;"
        "}"

        "header h1 {"
        "  margin: 0;"
        "  font-size: 28px;"
        "}"

        "header p {"
        "  margin: 5px 0 0;"
        "  color: #9ca3af;"
        "}"

        ".container {"
        "  max-width: 1200px;"
        "  margin: auto;"
        "  padding: 20px;"
        "}"

        ".grid {"
        "  display: grid;"
        "  grid-template-columns:"
        "    repeat(auto-fit, minmax(250px, 1fr));"
        "  gap: 16px;"
        "}"

        ".card {"
        "  background: #1f2937;"
        "  border-radius: 12px;"
        "  padding: 20px;"
        "  box-shadow: 0 4px 12px rgba(0,0,0,.25);"
        "}"

        ".card h2 {"
        "  margin-top: 0;"
        "  font-size: 18px;"
        "  color: #d1d5db;"
        "}"

        ".temperature {"
        "  font-size: 42px;"
        "  font-weight: bold;"
        "}"

        ".value {"
        "  font-size: 24px;"
        "  font-weight: bold;"
        "}"

        ".label {"
        "  color: #9ca3af;"
        "  font-size: 14px;"
        "}"

        ".row {"
        "  display: flex;"
        "  justify-content: space-between;"
        "  padding: 7px 0;"
        "  border-bottom: 1px solid #374151;"
        "}"

        ".row:last-child {"
        "  border-bottom: none;"
        "}"

        ".on {"
        "  color: #22c55e;"
        "  font-weight: bold;"
        "}"

        ".off {"
        "  color: #9ca3af;"
        "}"

        ".alarm {"
        "  color: #ef4444;"
        "  font-weight: bold;"
        "}"

        ".ok {"
        "  color: #22c55e;"
        "  font-weight: bold;"
        "}"

        ".status-bar {"
        "  margin-top: 16px;"
        "  color: #9ca3af;"
        "  font-size: 13px;"
        "}"

        "button {"
        "  padding: 8px 14px;"
        "  border: none;"
        "  border-radius: 6px;"
        "  cursor: pointer;"
        "}"

        ".nav {"
        "  margin-top: 20px;"
        "}"

        ".nav a {"
        "  color: #60a5fa;"
        "  margin-right: 20px;"
        "  text-decoration: none;"
        "}"

        "</style>"

        "</head>"

        "<body>"

        "<header>"

        "<h1>NeoTherm</h1>"

        "<p>Gestionnaire de chauffage</p>"

        "</header>"

        "<main class=\"container\">"

        "<div class=\"grid\">"

        /*
         * Thermostat
         */

        "<section class=\"card\">"

        "<h2>Thermostat</h2>"

        "<div class=\"temperature\" id=\"temperature\">-- °C</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Mode</span>"
        "<span id=\"mode\">--</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Consigne</span>"
        "<span id=\"setpoint\">-- °C</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Hystérésis</span>"
        "<span id=\"hysteresis\">-- °C</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Demande chauffage</span>"
        "<span id=\"heating\">--</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Relais</span>"
        "<span id=\"relay\">--</span>"
        "</div>"

        "</section>"

        /*
         * Météo
         */

        "<section class=\"card\">"

        "<h2>Météo</h2>"

        "<div class=\"row\">"
        "<span class=\"label\">Température</span>"
        "<span id=\"weather_temperature\">-- °C</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Humidité</span>"
        "<span id=\"humidity\">-- %</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Pression</span>"
        "<span id=\"pressure\">-- hPa</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Vent</span>"
        "<span id=\"wind\">--</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Rafales</span>"
        "<span id=\"gust\">--</span>"
        "</div>"

        "</section>"

        /*
         * Modèle thermique
         */

        "<section class=\"card\">"

        "<h2>Modèle thermique</h2>"

        "<div class=\"row\">"
        "<span class=\"label\">Température extérieure</span>"
        "<span id=\"outside\">-- °C</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Puissance chauffage</span>"
        "<span id=\"heat_power\">--</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Facteur de pertes</span>"
        "<span id=\"loss_factor\">--</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Masse thermique</span>"
        "<span id=\"thermal_mass\">--</span>"
        "</div>"

        "</section>"

        /*
         * Alarmes
         */

        "<section class=\"card\">"

        "<h2>Alarmes</h2>"

        "<div class=\"value\" id=\"alarm_count\">--</div>"

        "<div id=\"alarm_status\">Chargement...</div>"

        "</section>"

        /*
         * Evénements
         */

        "<section class=\"card\">"

        "<h2>Événements</h2>"

        "<div class=\"row\">"
        "<span class=\"label\">Total</span>"
        "<span id=\"event_total\">--</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Dans la queue</span>"
        "<span id=\"event_queue\">--</span>"
        "</div>"

        "<div class=\"row\">"
        "<span class=\"label\">Queue</span>"
        "<span id=\"event_queue_state\">--</span>"
        "</div>"

        "</section>"

        "</div>"

        "<div class=\"status-bar\" id=\"update_status\">"
        "Mise à jour..."
        "</div>"

        "<div class=\"nav\">"

        "<a href=\"/\">Accueil</a>"

        "</div>"

        "</main>"

        "<script>"

        "function setText(id, value) {"
        "  const element = document.getElementById(id);"
        "  if (element) element.textContent = value;"
        "}"

        /*
         * Thermostat
         */

        "async function updateThermostat() {"

        "  try {"

        "    const response = "
        "      await fetch('/api/status');"

        "    if (!response.ok) throw new Error();"

        "    const data = await response.json();"

        "    setText("
        "      'temperature',"
        "      data.temperature.toFixed(2) + ' °C');"

        "    setText('mode', data.mode);"

        "    setText("
        "      'setpoint',"
        "      data.setpoint.toFixed(2) + ' °C');"

        "    setText("
        "      'hysteresis',"
        "      data.hysteresis.toFixed(2) + ' °C');"

        "    setText("
        "      'heating',"
        "      data.heating_request ? 'ON' : 'OFF');"

        "    setText("
        "      'relay',"
        "      data.relay ? 'ON' : 'OFF');"

        "    document.getElementById('heating').className ="
        "      data.heating_request ? 'on' : 'off';"

        "    document.getElementById('relay').className ="
        "      data.relay ? 'on' : 'off';"

        "  }"
        "  catch (error) {"
        "    setText('temperature', '-- °C');"
        "    setText('mode', 'Erreur');"
        "  }"

        "}"

        /*
         * Weather
         */

        "async function updateWeather() {"

        "  try {"

        "    const response = "
        "      await fetch('/api/weather');"

        "    if (!response.ok) throw new Error();"

        "    const data = await response.json();"

        "    setText("
        "      'weather_temperature',"
        "      data.temperature.toFixed(2) + ' °C');"

        "    setText("
        "      'humidity',"
        "      data.humidity.toFixed(1) + ' %');"

        "    setText("
        "      'pressure',"
        "      data.pressure.toFixed(1) + ' hPa');"

        "    setText("
        "      'wind',"
        "      data.wind_speed.toFixed(1) + ' km/h');"

        "    setText("
        "      'gust',"
        "      data.wind_gust.toFixed(1) + ' km/h');"

        "  }"
        "  catch (error) {"
        "    setText('weather_temperature', '--');"
        "  }"

        "}"

        /*
         * Thermal
         */

        "async function updateThermal() {"

        "  try {"

        "    const response = "
        "      await fetch('/api/thermal');"

        "    if (!response.ok) throw new Error();"

        "    const data = await response.json();"

        "    setText("
        "      'outside',"
        "      Number(data.outside_temperature).toFixed(2)"
        "      + ' °C');"

        "    setText("
        "      'heat_power',"
        "      Number(data.heat_power).toFixed(3));"

        "    setText("
        "      'loss_factor',"
        "      Number(data.loss_factor).toFixed(3));"

        "    setText("
        "      'thermal_mass',"
        "      Number(data.mass).toFixed(3));"

        "  }"
        "  catch (error) {"
        "    setText('outside', '--');"
        "  }"

        "}"

        /*
         * Events
         */

        "async function updateEvents() {"

        "  try {"

        "    const response = "
        "      await fetch('/api/events');"

        "    if (!response.ok) throw new Error();"

        "    const data = await response.json();"

        "    setText("
        "      'event_total',"
        "      data.total);"

        "    setText("
        "      'event_queue',"
        "      data.queue_count + ' / ' + data.queue_size);"

        "    const state = "
        "      data.queue_full ? 'PLEINE' : "
        "      data.queue_empty ? 'VIDE' : 'ACTIVE';"

        "    setText('event_queue_state', state);"

        "  }"
        "  catch (error) {"
        "    setText('event_total', '--');"
        "  }"

        "}"

        /*
         * Alarmes
         */

        "async function updateAlarms() {"

        "  try {"

        "    const response = "
        "      await fetch('/api/alarms');"

        "    if (!response.ok) throw new Error();"

        "    const data = await response.json();"

        "    let count = 0;"

        "    if (Array.isArray(data)) {"
        "      count = data.length;"
        "    }"
        "    else if (typeof data.active_count === 'number') {"
        "      count = data.active_count;"
        "    }"

        "    setText('alarm_count', count);"

        "    const status = "
        "      document.getElementById('alarm_status');"

        "    if (count > 0) {"
        "      status.textContent = "
        "        'Alarme(s) active(s)';"
        "      status.className = 'alarm';"
        "    }"
        "    else {"
        "      status.textContent = 'Aucune alarme';"
        "      status.className = 'ok';"
        "    }"

        "  }"
        "  catch (error) {"
        "    setText('alarm_count', '--');"
        "  }"

        "}"

        /*
         * Mise à jour générale
         */

        "async function updateDashboard() {"

        "  await Promise.all(["
        "    updateThermostat(),"
        "    updateWeather(),"
        "    updateThermal(),"
        "    updateEvents(),"
        "    updateAlarms()"
        "  ]);"

        "  setText("
        "    'update_status',"
        "    'Dernière mise à jour : ' + "
        "    new Date().toLocaleTimeString());"

        "}"

        /*
         * Première mise à jour
         */

        "updateDashboard();"

        /*
         * Rafraîchissement automatique
         */

        "setInterval("
        "  updateDashboard,"
        "  1000);"

        "</script>"

        "</body>"
        "</html>";

    api_pages_send_html(
        client_socket,
        html);
}

#endif /* _WIN32 */