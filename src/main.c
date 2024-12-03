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
  FILE *logFile_FW1 = fopen("LOG_FW1.log", "a+");
  if (!logFile_FW1) {
    fclose(file);
    perror("Failed to open LOG_FW1.log");
    return 1;
  }

  fprintf(logFile_FW1, "------- (%s): Debug starting -------\n", argv[1]);
  fclose(logFile_FW1);
  int res = decode_and_log(file);
  fclose(file);
  logFile_FW1 = fopen("LOG_FW1.log", "a+");
  fprintf(logFile_FW1, "------- (%s): Exit code = %d -------\n", argv[1], res);

  return res;
}
