#include "action.h"
#include <string.h>

struct item {
  char content[256];
  int prior;
  int it;
};

int replaceTRACE(char toChange[256]) {
  int len = strlen(toChange);
  while (len != 0) {
    toChange[len] = toChange[len - 1];
    len--;
  }
  toChange[0] = 'A';
  toChange[1] = 'C';
  toChange[2] = 'T';
  toChange[3] = 'I';
  toChange[4] = 'O';
  toChange[5] = 'N';
  return 0;
}

struct item current = {.content = "", .prior = 0, .it = 0};

int action(char result[256]) {

  FILE *logFile = fopen("LOG_FW1.tx", "a+");
  int i = 0;
  if (!logFile) {
    perror("Failed to open LOG_FW1.tx");
    return 1;
  }
  fprintf(logFile, "%s\n", result);
  if (strcmp(current.content, result) != 0) {
    i = 0;
    while (result[i] != '\0') {
      current.content[i] = result[i];
      i++;
    }
    current.content[i] = '\0';
    current.it = 1;
    current.prior = 0;
  } else {
    current.it++;
  }

  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (current.it == 3) {
      replaceTRACE(current.content);
      printf("content = %s, value = %d", current.content, current.it);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans ke cas d'une alarme (5 fois)
    return 0;
  }
  fclose(logFile);

  return 0;
}
