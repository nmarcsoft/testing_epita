#include "global.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Enter a file\n");
    return 1;
  }

  struct stat path_stat;

  if (stat(argv[1], &path_stat) == 0) {
    if (!S_ISREG(path_stat.st_mode)) {
      return 1;
    }
  }

  FILE *file = fopen(argv[1], "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file");
    return 1;
  }
  FILE *logFile = fopen("LOG_FW1.tx", "a+");
  if (!logFile) {
    fclose(file);
    perror("Failed to open LOG_FW1.tx");
    return 1;
  }

  fprintf(logFile, "------- (%s): Debug starting -------\n", argv[1]);
  fclose(logFile);
  int res = decode_and_log(file);
  fclose(file);
  logFile = fopen("LOG_FW1.tx", "a+");
  fprintf(logFile, "------- (%s): Exit code = %d -------\n", argv[1], res);

  return res;
}
