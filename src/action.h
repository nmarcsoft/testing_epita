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

extern struct Engine engine;
extern struct Hydraulic hydraulic;
extern struct Pressure pressure;
extern struct Loads loads;
extern struct Cabin cabin;
extern struct Landing_Gear landing_gear;
extern struct Flight_Information flight_information;
extern struct System_Manager system_manager;

int action(char result[256]);
char *replaceWord(const char *s, const char *oldW, const char *newW);
void replaceTRACE(char toChange[256]);

#endif