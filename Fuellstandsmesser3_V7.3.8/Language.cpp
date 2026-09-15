#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "Language.h"

#if FUELLSTANDSMESSER3_LANGUAGE == 1
#include "Language_en.h"
#else
#include "Language_de.h"
#endif

const char *languageCode() {
#if FUELLSTANDSMESSER3_LANGUAGE == 1
  return "en";
#else
  return "de";
#endif
}

const char *languageName() {
#if FUELLSTANDSMESSER3_LANGUAGE == 1
  return "English";
#else
  return "Deutsch";
#endif
}

void localizeWebHtml(String &html) {
#if FUELLSTANDSMESSER3_LANGUAGE == 1
  for (size_t i = 0; LANGUAGE_ENTRIES[i].source; ++i) {
    html.replace(LANGUAGE_ENTRIES[i].source, LANGUAGE_ENTRIES[i].translation);
  }
#else
  (void)html;
#endif
}

#endif
