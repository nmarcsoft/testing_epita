#ifndef ACTION_H
#define ACTION_H

#include "global.h"

struct Engine {
  char content[256];
  int it;
};

struct Hydraulic {
  char content[256];
  int it;
};

struct Pressure {
  char content[256];
  int it;
};

struct Loads {
  char content[256];
  int it;
};

struct Cabin {
  char content[256];
  int it;
};

struct Landing_Gear {
  char content[256];
  int it;
};

struct Flight_Information {
  char content[256];
  int it;
};

struct System_Manager {
  FILE *FW;
  int error_cpt;
};

int action(char result[256]);
char *replaceWord(const char *s, const char *oldW, const char *newW);

#endif
