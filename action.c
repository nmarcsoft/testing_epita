#include "action.h"

int action(char result[256]) {
  // TODO

  FILE *logFile = fopen("LOG_FW1.tx", "a+");
  if (!logFile) {
    perror("Failed to open LOG_FW1.tx");
    return 1;
  }

  fprintf(logFile, "%s\n", result);

  fclose(logFile);

  return 0;
}
