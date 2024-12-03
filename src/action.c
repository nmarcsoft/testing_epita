#include "action.h"
#include <stdio.h>
#include <string.h>

void replaceTRACE(char toChange[256]) {
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
}

void copy256string(char s1[256], char s2[256]) {
  for (int i = 0; i < 256; i++) {
    s1[i] = s2[i];
  }
}

struct Engine engine = {.content = "", .it = 0};
struct Hydraulic hydraulic = {.content = "", .it = 0};
struct Pressure pressure = {.content = "", .it = 0};
struct Loads loads = {.content = "", .it = 0};
struct Cabin cabin = {.content = "", .it = 0};
struct Landing_Gear landing_gear = {.content = "", .it = 0};
struct Flight_Information flight_information = {.content = "", .it = 0};

struct System_Manager system_manager = {.FW = NULL, .error_cpt = 0};

void EngineManagment(char result[256], FILE *fw) {
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (engine.it == 3) {
      replaceTRACE(engine.content);
      fprintf(fw, "%s\n", engine.content);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans le cas d'une alarme (5 fois)
    if (engine.it == 5) {
      replaceTRACE(engine.content);
      fprintf(fw, "%s\n", engine.content);
      // printf("content = %s, value = %d", engine.content, engine.it);
    }
  }
}

void HydraulicManagment(char result[256], FILE *fw) {
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (hydraulic.it == 3) {
      replaceTRACE(hydraulic.content);
      fprintf(fw, "%s\n", hydraulic.content);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans le cas d'une alarme (5 fois)
    if (hydraulic.it == 5) {
      replaceTRACE(hydraulic.content);
      fprintf(fw, "%s\n", hydraulic.content);
      // printf("content = %s, value = %d\n", hydraulic.content, hydraulic.it);
    }
  }
}

void PressureManagment(char result[256], FILE *fw) {
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (pressure.it == 3) {
      replaceTRACE(pressure.content);
      fprintf(fw, "%s\n", hydraulic.content);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans le cas d'une alarme (5 fois)
    if (pressure.it == 5) {
      replaceTRACE(pressure.content);
      fprintf(fw, "%s\n", hydraulic.content);
    }
  }
}

void LoadsManagment(char result[256], FILE *fw) {
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (loads.it == 3) {
      replaceTRACE(loads.content);
      fprintf(fw, "%s\n", loads.content);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans le cas d'une alarme (5 fois)
    if (loads.it == 5) {
      replaceTRACE(loads.content);
      fprintf(fw, "%s\n", loads.content);
    }
  }
}

void CabinManagment(char result[256], FILE *fw) {
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (cabin.it == 3) {
      replaceTRACE(cabin.content);
      fprintf(fw, "%s\n", cabin.content);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans le cas d'une alarme (5 fois)
    if (cabin.it == 5) {
      replaceTRACE(cabin.content);
      fprintf(fw, "%s\n", cabin.content);
    }
  }
}

void LandingManagment(char result[256], FILE *fw) {
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (landing_gear.it == 3) {
      replaceTRACE(landing_gear.content);
      fprintf(fw, "%s\n", landing_gear.content);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans le cas d'une alarme (5 fois)
    if (landing_gear.it == 5) {
      replaceTRACE(landing_gear.content);
      fprintf(fw, "%s\n", landing_gear.content);
    }
  }
}

void FlightManagment(char result[256], FILE *fw) {
  if (strstr(result, "ALE") != NULL) {
    // On est dans le cas d'une alerte (3 fois)
    if (flight_information.it == 3) {
      replaceTRACE(flight_information.content);
      fprintf(fw, "%s\n", flight_information.content);
    }
  } else if (strstr(result, "ALA") != NULL) {
    // On est dans le cas d'une alarme (5 fois)
    if (flight_information.it == 5) {
      replaceTRACE(flight_information.content);
      fprintf(fw, "%s\n", flight_information.content);
    }
  }
}

int action(char result[256]) {

  if (strlen(result) > 256)
    return 1;

  FILE *logFile_FW1 = fopen("LOG_FW1.log", "a+");
  FILE *logFile_FW2 = fopen("LOG_FW2.log", "a+");
  if (system_manager.error_cpt >= 10) {
    system_manager.FW = logFile_FW2;
  } else if (system_manager.error_cpt == 20) {
    return -1;
  } else {
    system_manager.FW = logFile_FW1;
  }
  if (!logFile_FW1) {
    perror("Failed to open LOG_FW1.log");
    return 1;
  }
  if (!logFile_FW2) {
    perror("Failed to open LOG_FW2.log");
    return 1;
  }
  fprintf(logFile_FW1, "%s\n", result);
  fprintf(logFile_FW2, "%s\n", result);

  if (strstr(result, "ENG")) {
    if (strcmp(engine.content, result) == 0) {
      engine.it++;
      EngineManagment(result, system_manager.FW);
    } else {
      copy256string(engine.content, result);
      engine.it = 1;
    }
  }

  if (strstr(result, "HYD")) {
    if (strcmp(hydraulic.content, result) == 0) {
      hydraulic.it++;
      HydraulicManagment(result, system_manager.FW);
    } else {
      copy256string(hydraulic.content, result);
      hydraulic.it = 1;
    }
  }

  if (strstr(result, "PRES")) {
    if (strcmp(pressure.content, result) == 0) {
      pressure.it++;
      PressureManagment(result, system_manager.FW);
    } else {
      copy256string(pressure.content, result);
      pressure.it = 1;
    }
  }

  if (strstr(result, "LOAD")) {
    if (strcmp(loads.content, result) == 0) {
      loads.it++;
      LoadsManagment(result, system_manager.FW);
    } else {
      copy256string(loads.content, result);
      loads.it = 1;
    }
  }

  if (strstr(result, "CAB")) {
    if (strcmp(cabin.content, result) == 0) {
      cabin.it++;
      CabinManagment(result, system_manager.FW);
    } else {
      copy256string(cabin.content, result);
      cabin.it = 1;
    }
  }

  if (strstr(result, "LG")) {
    if (strcmp(landing_gear.content, result) == 0) {
      landing_gear.it++;
      LandingManagment(result, system_manager.FW);
    } else {
      copy256string(landing_gear.content, result);
      landing_gear.it = 1;
    }
  }

  if (strstr(result, "FINFO")) {
    if (strcmp(flight_information.content, result) == 0) {
      flight_information.it++;
      FlightManagment(result, system_manager.FW);
    } else {
      copy256string(flight_information.content, result);
      flight_information.it = 1;
    }
  }

  if (strstr(result, "ERROR")) {
    system_manager.error_cpt++;
  }

  fclose(logFile_FW1);
  fclose(logFile_FW2);

  return 0;
}
