#include "decode_and_log.h"
#include "action.h"

int decode_and_log(FILE *file) {
  char buffer[33];
  char validLine[33];
  int lineNumber = 0;

  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    } else {
      int ch;
      while ((ch = fgetc(file)) != '\n' && ch != EOF)
        ;
    }

    if (strlen(buffer) == 32) {
      strncpy(validLine, buffer, 32);
      validLine[32] = '\0';
    } else {
      return 1;
    }
    char result[256] = "";
    strcat(result, "TRACE-");

    char substring[4];
    strncpy(substring, validLine, 3);
    substring[3] = '\0';

    if (strcmp(substring, "001") == 0) {
      strcat(result, "ENG");
    } else if (strcmp(substring, "010") == 0) {
      strcat(result, "HYD");
    } else if (strcmp(substring, "011") == 0) {
      strcat(result, "PRES");
    } else if (strcmp(substring, "100") == 0) {
      strcat(result, "LOAD");
    } else if (strcmp(substring, "101") == 0) {
      strcat(result, "CAB");
    } else if (strcmp(substring, "110") == 0) {
      strcat(result, "LG");
    } else if (strcmp(substring, "111") == 0) {
      strcat(result, "FINFO");
    } else {
      strcat(result, "ERROR");
    }

    char actionBits[4];
    strncpy(actionBits, validLine + 3, 3);
    actionBits[3] = '\0';

    strcat(result, "-");

    if (strcmp(actionBits, "001") == 0) {
      strcat(result, "SON");
    } else if (strcmp(actionBits, "010") == 0) {
      strcat(result, "LUM");
    } else if (strcmp(actionBits, "100") == 0) {
      strcat(result, "TEXT");
    } else if (strcmp(actionBits, "011") == 0) {
      strcat(result, "SON & LUM");
    } else if (strcmp(actionBits, "101") == 0) {
      strcat(result, "SON & TEXT");
    } else if (strcmp(actionBits, "110") == 0) {
      strcat(result, "LUM & TEXT");
    } else if (strcmp(actionBits, "111") == 0) {
      strcat(result, "SON, LUM & TEXT");
    } else {
      strcat(result, "ERROR");
    }

    char messageType[3];
    strncpy(messageType, validLine + 6, 2);
    messageType[2] = '\0';

    strcat(result, "-");

    if (strcmp(messageType, "00") == 0) {
      strcat(result, "ERROR");
    } else if (strcmp(messageType, "01") == 0) {
      strcat(result, "INF");
    } else if (strcmp(messageType, "10") == 0) {
      strcat(result, "ALE");
    } else if (strcmp(messageType, "11") == 0) {
      strcat(result, "ALA");
    }

    char asciiBits[25];
    strncpy(asciiBits, validLine + 8, 24);
    asciiBits[24] = '\0';

    char decodedMessage[9];
    for (int i = 0; i < 8; i++) {
      char byte[9];
      strncpy(byte, asciiBits + (i * 8), 8);
      byte[8] = '\0';
      decodedMessage[i] = (char)strtol(byte, NULL, 2);
    }
    decodedMessage[8] = '\0';

    strcat(result, "-");
    strcat(result, decodedMessage);

    printf("%s\n", result);
    action(result);
    lineNumber++;
  }
  return 0;
}
