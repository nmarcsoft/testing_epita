#include "action.h"

char *content[3];

int action(char result[256]) {
  FILE *logFile = fopen("LOG_FW1.tx", "a+");
  if (!logFile) {
    perror("Failed to open LOG_FW1.tx");
    return 1;
  }

  fprintf(logFile, "%s\n", result);
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    content[0] = result;
    content[1]++;

    if (content[1] == 3) {
      fprintf(logFile, "%s\n", result);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans ke cas d'une alarme (5 fois)
  }
  fclose(logFile);

  return 0;
}
