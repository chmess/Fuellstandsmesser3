#include "Fuellstandsmesser3_Formatting.h"

String htmlEscape(const String &input) {
  String s;
  s.reserve(input.length() + 16);

  for (size_t i = 0; i < input.length(); i++) {
    switch (input[i]) {
      case '&': s += "&amp;"; break;
      case '<': s += "&lt;"; break;
      case '>': s += "&gt;"; break;
      case '"': s += "&quot;"; break;
      case '\'': s += "&#39;"; break;
      default: s += input[i]; break;
    }
  }

  return s;
}

String jsonEscape(const String &input) {
  String out;
  out.reserve(input.length() + 16);

  for (size_t i = 0; i < input.length(); i++) {
    switch (input[i]) {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: out += input[i]; break;
    }
  }

  return out;
}

String jsonFloat(float value, uint8_t decimals) {
  if (!isfinite(value)) {
    return "null";
  }
  return String(value, (unsigned int)decimals);
}

float calculateDewPointC(float temperatureC, float humidityPercent) {
  if (!isfinite(temperatureC) || !isfinite(humidityPercent) ||
      humidityPercent <= 0.0f) {
    return NAN;
  }
  if (humidityPercent >= 100.0f) {
    return temperatureC;
  }

  const float a = 17.62f;
  const float b = 243.12f;
  const float alpha = log(humidityPercent / 100.0f) +
                      (a * temperatureC) / (b + temperatureC);
  return (b * alpha) / (a - alpha);
}

