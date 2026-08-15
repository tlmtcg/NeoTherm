console.log("NeoTherm app.js chargé");

let dashboardUpdating = false;

/*==========================================================
 * Initialisation
 *=========================================================*/

document.addEventListener("DOMContentLoaded", () => {
  console.log("NeoTherm web interface started");

  updateDashboard();

  setInterval(updateDashboard, 2000);
});

/*==========================================================
 * Dashboard
 *=========================================================*/

async function updateDashboard() {
  if (dashboardUpdating) {
    console.warn("Dashboard update already running");
    return;
  }

  dashboardUpdating = true;

  try {
    await updateThermostat();
    await updateWeather();
    await updateAlarms();
    await updateClock();

    await updateRelay();
    await updateClimate();
    await updateThermalModel();
    // await updateLearning();
    await updatePrediction();
    await updateRuntime();
    await updateScheduler();
    await updateEvents();
    await updateStorage();
  } finally {
    dashboardUpdating = false;
  }
}

/*==========================================================
 * Thermostat
 *=========================================================*/

async function updateThermostat() {
  try {
    const response = await fetch("/api/status", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-mode", data.mode);

    setText("ui-room-temperature", formatTemperature(data.temperature));

    setText("ui-target-temperature", formatTemperature(data.setpoint));

    setText("ui-hysteresis", formatTemperature(data.hysteresis));

    setText("ui-heating-request", data.heating_request ? "ON" : "OFF");

    setText("ui-heater", data.relay ? "ON" : "OFF");
  } catch (error) {
    console.error("Thermostat:", error);
  }
}

/*==========================================================
 * Météo
 *=========================================================*/

async function updateWeather() {
  try {
    const response = await fetch("/api/weather", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-outdoor-temperature", formatTemperature(data.temperature));

    setText("ui-outdoor-humidity", formatNumber(data.humidity) + " %");

    setText("ui-air-pressure", formatNumber(data.pressure) + " hPa");

    setText("ui-wind-speed", formatNumber(data.wind_speed) + " km/h");

    setText("ui-wind-gust", formatNumber(data.wind_gust) + " km/h");

    setText("ui-wind-direction", formatNumber(data.wind_direction) + " °");

    setText("ui-rain", formatNumber(data.rain) + " mm");
  } catch (error) {
    console.error("Weather:", error);
  }
}

/*==========================================================
 * Alarmes
 *=========================================================*/

async function updateAlarms() {
  try {
    const response = await fetch("/api/alarms", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-active-alarms", data.count);

    const container = document.getElementById("ui-alarm-list");

    if (container === null) {
      return;
    }

    container.innerHTML = "";

    if (!Array.isArray(data.alarms) || data.alarms.length === 0) {
      container.innerHTML = '<p class="alarm-none">' + "Aucune alarme" + "</p>";

      return;
    }

    data.alarms.forEach((alarm) => {
      const element = document.createElement("div");

      element.className = "alarm";

      element.innerHTML = `
          <div class="alarm-name">
            ${escapeHtml(alarm.name)}
          </div>

          <div class="alarm-state">
            ${escapeHtml(alarm.state)}
          </div>

          <div class="alarm-value">
            ${formatNumber(alarm.value)}
          </div>

          <div class="alarm-type">
            ${escapeHtml(alarm.type)}
          </div>
          `;

      container.appendChild(element);
    });
  } catch (error) {
    console.error("Alarms:", error);
  }
}

/*==========================================================
 * Horloge
 *=========================================================*/

async function updateClock() {
  try {
    const response = await fetch("/api/clock", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-current-time", data.datetime);
    setText("ui-header-current-time", data.datetime);
  } catch (error) {
    console.error("Clock:", error);
  }
}

async function updateRelay() {
  try {
    const response = await fetch("/api/relay", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-relay-state", data.state ? "ON" : "OFF");

    setText("ui-relay-switch-count", data.switch_count);

    setText("ui-relay-last-switch", data.last_switch_time);

    setText("ui-relay-min-delay", data.min_switch_delay + " s");

    setText("ui-relay-elapsed", data.elapsed_delay + " s");

    setText("iu-relay-can-switch", data.can_switch);

    setText("iu-relay-remaining", data.remaining_delay);
  } catch (error) {
    console.error("Relay:", error);
  }
}

async function updateClimate() {
  try {
    const response = await fetch("/api/climate", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-climate-temperature", formatTemperature(data.temperature));

    setText("ui-climate-heating", data.heating ? "ON" : "OFF");
  } catch (error) {
    console.error("Climate:", error);
  }
}

async function updateThermalModel() {
  try {
    const response = await fetch("/api/thermal", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText(
      "ui-thermal-outside-temperature",
      formatTemperature(data.outside_temperature),
    );

    setText("ui-thermal-heat-power", formatNumber(data.heat_power));

    setText("ui-thermal-loss-factor", formatNumber(data.loss_factor));

    setText("ui-thermal-mass", formatNumber(data.mass));
  } catch (error) {
    console.error("Thermal model:", error);
  }
}

async function updatePrediction() {
  try {
    const response = await fetch("/api/thermal_prediction", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText(
      "ui-prediction-current",
      formatTemperature(data.current_temperature),
    );

    setText("ui-prediction-next-tick", formatTemperature(data.next_tick));

    setText("ui-prediction-valid", data.valid ? "OUI" : "NON");

    setText(
      "ui-prediction-heated",
      data.heat_rate_valid ? formatNumber(data.heat_rate) + " °C/min" : "--",
    );

    setText(
      "ui-prediction-natural",
      data.cooling_rate_valid
        ? formatNumber(data.cooling_rate) + " °C/min"
        : "Invalid",
    );

    setText("ui-next-tick", data.next_tick);
  } catch (error) {
    console.error("Prediction:", error);
  }
}

async function updateRuntime() {
  try {
    const response = await fetch("/api/runtime", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-runtime-mode", data.mode);

    setText("ui-runtime-setpoint", formatTemperature(data.setpoint));

    setText("ui-runtime-hysteresis", formatTemperature(data.hysteresis));

    setText("ui-runtime-relay-delay", formatNumber(data.relay_delay) + " s");

    setText("ui-runtime-latitude", formatNumber(data.latitude));

    setText("ui-runtime-longitude", formatNumber(data.longitude));

    setText("ui-runtime-weather-provider", data.weather_provider);

    setText(
      "ui-runtime-weather-period",
      formatNumber(data.weather_update_period_sec) + " s",
    );
  } catch (error) {
    console.error("Runtime:", error);
  }
}

async function updateScheduler() {
  try {
    const response = await fetch("/api/scheduler", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-scheduler-count", data.task_count + " / " + data.max_tasks);

    const container = document.getElementById("ui-scheduler-list");

    if (container === null) {
      return;
    }

    if (!Array.isArray(data.tasks) || data.tasks.length === 0) {
      container.innerHTML = "<p>Aucune tâche</p>";

      return;
    }

    let html = `
      <div class="table-container">
        <table class="scheduler-table">
          <thead>
            <tr>
              <th>ID</th>
              <th>Tâche</th>
              <th>Période</th>
              <th>Compteur</th>
              <th>Prochaine</th>
              <th>État</th>
            </tr>
          </thead>

          <tbody>
    `;

    data.tasks.forEach((task) => {
      html += `
        <tr>
          <td>${task.id}</td>

          <td>
            ${escapeHtml(task.name)}
          </td>

          <td>
            ${task.period} s
          </td>

          <td>
            ${task.counter}
          </td>

          <td>
            ${task.next} s
          </td>

          <td class="${task.enabled ? "on" : "off"}">
            ${task.enabled ? "ON" : "OFF"}
          </td>
        </tr>
      `;
    });

    html += `
          </tbody>
        </table>
      </div>
    `;

    container.innerHTML = html;
  } catch (error) {
    console.error("Scheduler:", error);
  }
}

async function updateEvents() {
  try {
    const response = await fetch("/api/events", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-event-total", data.total);

    setText("ui-event-queue-count", data.queue_count);

    setText("ui-event-queue-size", data.queue_size);

    setText("ui-event-queue-empty", data.queue_empty ? "OUI" : "NON");

    setText("ui-event-queue-full", data.queue_full ? "OUI" : "NON");

    const container = document.getElementById("ui-event-list");

    if (container === null) {
      return;
    }

    container.innerHTML = "";

    if (
      data.events === null ||
      typeof data.events !== "object" ||
      Object.keys(data.events).length === 0
    ) {
      container.innerHTML = "<p>Aucun événement</p>";

      return;
    }

    let html = `
      <div class="table-container">
        <table class="scheduler-table">
          <thead>
            <tr>
              <th>Événement</th>
              <th>Nombre</th>
            </tr>
          </thead>

          <tbody>
    `;

    Object.entries(data.events).forEach(([name, count]) => {
      html += `
          <tr>
            <td>${escapeHtml(name)}</td>
            <td>${count}</td>
          </tr>
        `;
    });

    html += `
          </tbody>
        </table>
      </div>
    `;

    container.innerHTML = html;
  } catch (error) {
    console.error("Events:", error);
  }
}

async function updateStorage() {
  try {
    const response = await fetch("/api/storage", {
      cache: "no-store",
    });

    if (!response.ok) {
      throw new Error("HTTP " + response.status);
    }

    const data = await response.json();

    setText("ui-storage-source", data.source);

    setText("ui-storage-mode", data.mode);

    setText("ui-storage-setpoint", formatTemperature(data.setpoint));

    setText("ui-storage-hysteresis", formatTemperature(data.hysteresis));

    setText("ui-storage-relay-delay", formatNumber(data.relay_delay) + " s");

    setText("ui-storage-weather-provider", data.weather_provider);

    setText(
      "ui-storage-weather-period",
      formatNumber(data.weather_update_period) + " s",
    );

    setText("ui-storage-latitude", formatNumber(data.latitude));

    setText("ui-storage-longitude", formatNumber(data.longitude));
  } catch (error) {
    console.error("Storage:", error);
  }
}

/*==========================================================
 * Utilitaires
 *=========================================================*/

function setText(id, value) {
  const element = document.getElementById(id);

  if (element !== null) {
    element.textContent = value;
  }
}

function formatTemperature(value) {
  if (typeof value !== "number") {
    return "--";
  }

  return value.toFixed(1) + " °C";
}

function formatNumber(value) {
  if (typeof value !== "number") {
    return "--";
  }

  return value.toFixed(1);
}

function escapeHtml(value) {
  if (value === null || value === undefined) {
    return "";
  }

  return String(value)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#039;");
}
