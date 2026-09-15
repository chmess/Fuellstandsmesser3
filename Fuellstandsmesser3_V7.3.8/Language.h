#pragma once

#include <Arduino.h>

// Compile-time language selection:
//   0 = Deutsch (default)
//   1 = English
#ifndef FUELLSTANDSMESSER3_LANGUAGE
#define FUELLSTANDSMESSER3_LANGUAGE 0
#endif

void localizeWebHtml(String &html);
const char *languageCode();
const char *languageName();
