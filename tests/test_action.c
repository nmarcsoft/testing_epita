#include "../src/decode_and_log.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "../src/action.h"

void log_report(FILE *report_file, const char *format, ...) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(report_file, "[%04d-%02d-%02d %02d:%02d:%02d] ", tm.tm_year + 1900,
          tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  va_list args;
  va_start(args, format);
  vfprintf(report_file, format, args);
  va_end(args);
}

void generic_test(const char *input_data, const char *expected_log_content,
                  const char *test_name) {
  FILE *log_file = fopen("LOG_FW1.log", "w");
  if (log_file != NULL) {
    fclose(log_file);
  } else {
    printf("Error: Could not clear the log file.\n");
    return;
  }

  char buffer[1024];
  strncpy(buffer, input_data, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';
  char *line = strtok(buffer, "\n");

  while (line != NULL) {
    printf("Testing line: %s\n", line);
    int result = action(line);
    CU_ASSERT_EQUAL(result, 0);
    line = strtok(NULL, "\n");
  }

  char actual_log_content[4096] = {0};
  log_file = fopen("LOG_FW1.log", "r");
  if (log_file == NULL) {
    printf("Error: Could not open the log file for reading.\n");
    return;
  }

  size_t total_read = 0;
  while (fgets(actual_log_content + total_read,
               sizeof(actual_log_content) - total_read, log_file)) {
    total_read += strlen(actual_log_content + total_read);
  }
  fclose(log_file);

  FILE *report_log = fopen("report_test.txt", "a+");
  if (report_log == NULL) {
    printf("Error: Could not open the report file.\n");
    return;
  }

  if (!strcmp(actual_log_content, expected_log_content)) {
    char result_message[256];
    snprintf(result_message, sizeof(result_message), "%s succeeded\n",
             test_name);
    log_report(report_log, result_message);
  } else {
    char result_message[256];
    snprintf(result_message, sizeof(result_message), "%s failed\n", test_name);
    log_report(report_log, result_message);
  }

  fclose(report_log);
  CU_ASSERT_STRING_EQUAL(actual_log_content, expected_log_content);
}

void test_action_too_long() {
  char input[257];
  memset(input, 'A', sizeof(input) - 1);
  input[257] = '\0';
  int result = action(input);
  char input2[300];
  memset(input2, 'A', sizeof(input2) - 1);
  input2[300] = '\0';
  int result2 = action(input2);
  CU_ASSERT_EQUAL(result, 1);
  CU_ASSERT_EQUAL(result2, 1);
}

void test_action_engine_alert() {
  const char *input_data = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                                     "ACTION-ENG-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_engine_alert");
}

void test_action_engine_alarm() {
  const char *input_data = "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n";

  const char *expected_log_content = "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                                     "ACTION-ENG-SON, LUM & TEXT-ALA-Eng\n";

  generic_test(input_data, expected_log_content, "test_action_engine_alarm");
}

void test_action_engine_nothing() {
  const char *input_data = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_engine_nothing");
}

void test_action_hydraulic_alert() {
  const char *input_data = "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                                     "ACTION-HYD-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_hydraulic_alert");
}

void test_action_hydraulic_alarm() {
  const char *input_data = "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n";

  const char *expected_log_content = "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALA-Eng\n"
                                     "ACTION-HYD-SON, LUM & TEXT-ALA-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_hydraulic_alarm");
}

void test_action_hydraulic_nothing() {
  const char *input_data = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content,
               "test_action_hydraulic_nothing");
}

void test_action_pressure_alert() {
  const char *input_data = "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n"
                                     "ACTION- PRES-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_pressure_alert");
}

void test_action_pressure_alarm() {
  const char *input_data = "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n";

  const char *expected_log_content = "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-PRES-SON, LUM & TEXT-ALA-Eng\n"
                                     "ACTION-PRES-SON, LUM & TEXT-ALA-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_pressure_alarm");
}

void test_action_pressure_nothing() {
  const char *input_data = "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-PRES-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content,
               "test_action_pressure_nothing");
}

void test_action_loads_alert() {
  const char *input_data = "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n"
                                     "ACTION-LOADS-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_loads_alert");
}

void test_action_loads_alarm() {
  const char *input_data = "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n";

  const char *expected_log_content = "TRACE-LOAD-SSON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-LOADS-SON, LUM & TEXT-ALA-Eng\n"
                                     "ACTION-LOADS-SON, LUM & TEXT-ALA-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_loads_alarm");
}

void test_action_loads_nothing() {
  const char *input_data = "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-LOADS-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_loads_nothing");
}

void test_action_cabin_alert() {
  const char *input_data = "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                                     "ACTION-CAB-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_cabin_alert");
}

void test_action_cabin_alarm() {
  const char *input_data = "TRACE-CAB-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-CAB-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-CAB-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-CAB-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-CAB-SON, LUM & TEXT-ALA-Eng\n";

  const char *expected_log_content = "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                                     "ACTION-CAB-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_cabin_alarm");
}

void test_action_cabin_nothing() {
  const char *input_data = "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-CAB-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_cabin_nothing");
}

void test_action_landing_alert() {
  const char *input_data = "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n"
                                     "ACTION-LG-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_landing_alert");
}

void test_action_landing_alarm() {
  const char *input_data = "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n"
                           "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n";

  const char *expected_log_content = "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-LG-SON, LUM & TEXT-ALA-Eng\n"
                                     "ACTION-LG-SON, LUM & TEXT-ALA-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_landing_alarm");
}

void test_action_landing_nothing() {
  const char *input_data = "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n";

  const char *expected_log_content = "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-LG-SON, LUM & TEXT-ALE-Eng\n";
  generic_test(input_data, expected_log_content, "test_action_landing_nothing");
}

void test_action_flight_information_alert() {
  const char *input_data = "TRACE-FINFO-SON, LUM & TEXT-ALE-Finfo\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALE-Finfo\n"
                           "TRACE-HYD-SON, LUM & TEXT-ALE-Finfo\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALE-Finfo\n";

  const char *expected_log_content = "TRACE-FINFO-SON, LUM & TEXT-ALE-Finfo\n"
                                     "TRACE-FINFO-SON, LUM & TEXT-ALE-Finfo\n"
                                     "TRACE-HYD-SON, LUM & TEXT-ALE-Finfo\n"
                                     "ACTION-FINFO-SON, LUM & TEXT-ALE-Finfo\n";
  generic_test(input_data, expected_log_content,
               "test_action_flight_management_alert");
}

void test_action_flight_information_alarm() {
  const char *input_data = "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n";

  const char *expected_log_content = "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                                     "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                                     "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                                     "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                                     "ACTION-FINFO-SON, LUM & TEXT-ALA-Finfo\n";
  generic_test(input_data, expected_log_content,
               "test_action_flight_management_alarm");
}

void test_action_flight_information_nothing() {
  const char *input_data = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n"
                           "TRACE-FINFO-SON, LUM & TEXT-ALA-Finfo\n";

  const char *expected_log_content = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n"
                                     "TRACE-FINFO-SON, LUM & TEXT-ALA-Eng\n"
                                     "TRACE-FINFO-SON, LUM & TEXT-ALA-Eng\n";
  generic_test(input_data, expected_log_content,
               "test_action_flight_information_nothing");
}

void test_action_simple_engine() {
    engine.it = 0;
    strncpy(engine.content, "", sizeof(engine.content));

    char message[256] = "TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n";
    for (int i = 0; i < 3; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(engine.it, 3);
    CU_ASSERT_STRING_EQUAL(engine.content, "ACTION-ENG-SON, LUM & TEXT-ALE-Eng\n");

    strcpy(message, "TRACE-ENG-SON, LUM & TEXT-ALA-Eng\n");
    for (int i = 0; i < 5; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(engine.it, 5);
    CU_ASSERT_STRING_EQUAL(engine.content, "ACTION-ENG-SON, LUM & TEXT-ALA-Eng\n");
}


void test_action_simple_hydraulic() {
    hydraulic.it = 0;
    strncpy(hydraulic.content, "", sizeof(hydraulic.content));

    char message[256] = "TRACE-HYD-SON, LUM & TEXT-ALE-Hyd\n";
    for (int i = 0; i < 3; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(hydraulic.it, 3);
    CU_ASSERT_STRING_EQUAL(hydraulic.content, "ACTION-HYD-SON, LUM & TEXT-ALE-Hyd\n");

    strncpy(message, "TRACE-HYD-SON, LUM & TEXT-ALA-Hyd\n", sizeof(message));
    for (int i = 0; i < 5; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(hydraulic.it, 5);
    CU_ASSERT_STRING_EQUAL(hydraulic.content, "ACTION-HYD-SON, LUM & TEXT-ALA-Hyd\n");
}


void test_action_simple_pressure() {
    pressure.it = 0;
    strncpy(pressure.content, "", sizeof(pressure.content));

    char message[256] = "TRACE-PRES-SON, LUM & TEXT-ALE-Pres\n";
    for (int i = 0; i < 3; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(pressure.it, 3);
    CU_ASSERT_STRING_EQUAL(pressure.content, "ACTION-PRES-SON, LUM & TEXT-ALE-Pres\n");

    strncpy(message, "TRACE-PRES-SON, LUM & TEXT-ALA-Pres\n", sizeof(message));
    for (int i = 0; i < 5; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(pressure.it, 5);
    CU_ASSERT_STRING_EQUAL(pressure.content, "ACTION-PRES-SON, LUM & TEXT-ALA-Pres\n");
}

void test_action_simple_loads() {
    loads.it = 0;
    strncpy(loads.content, "", sizeof(loads.content));

    char message[256] = "TRACE-LOAD-SON, LUM & TEXT-ALE-Load\n";
    for (int i = 0; i < 3; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(loads.it, 3);
    CU_ASSERT_STRING_EQUAL(loads.content, "ACTION-LOAD-SON, LUM & TEXT-ALE-Load\n");

    strncpy(message, "TRACE-LOAD-SON, LUM & TEXT-ALA-Load\n", sizeof(message));
    for (int i = 0; i < 5; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(loads.it, 5);
    CU_ASSERT_STRING_EQUAL(loads.content, "ACTION-LOAD-SON, LUM & TEXT-ALA-Load\n");
}

void test_action_simple_cabin() {
    cabin.it = 0;
    strncpy(cabin.content, "", sizeof(cabin.content));

    char message[256] = "TRACE-CAB-SON, LUM & TEXT-ALE-Cabin\n";
    for (int i = 0; i < 3; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(cabin.it, 3);
    CU_ASSERT_STRING_EQUAL(cabin.content, "ACTION-CAB-SON, LUM & TEXT-ALE-Cabin\n");

    strncpy(message, "TRACE-CAB-SON, LUM & TEXT-ALA-Cabin\n", sizeof(message));
    for (int i = 0; i < 5; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(cabin.it, 5);
    CU_ASSERT_STRING_EQUAL(cabin.content, "ACTION-CAB-SON, LUM & TEXT-ALA-Cabin\n");
}


void test_action_simple_landing_gear() {
    landing_gear.it = 0;
    strncpy(landing_gear.content, "", sizeof(landing_gear.content));

    char message[256] = "TRACE-LG-SON, LUM & TEXT-ALE-Landing\n";
    for (int i = 0; i < 3; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(landing_gear.it, 3);
    CU_ASSERT_STRING_EQUAL(landing_gear.content, "ACTION-LG-SON, LUM & TEXT-ALE-Landing\n");

    strncpy(message, "TRACE-LG-SON, LUM & TEXT-ALA-Landing\n", sizeof(message));
    for (int i = 0; i < 5; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(landing_gear.it, 5);
    CU_ASSERT_STRING_EQUAL(landing_gear.content, "ACTION-LG-SON, LUM & TEXT-ALA-Landing\n");
}


void test_action_simple_flight_information() {
    flight_information.it = 0;
    strncpy(flight_information.content, "", sizeof(flight_information.content));

    char message[256] = "TRACE-FINFO-SON, LUM & TEXT-ALE-Flight\n";
    for (int i = 0; i < 3; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(flight_information.it, 3);
    CU_ASSERT_STRING_EQUAL(flight_information.content, "ACTION-FINFO-SON, LUM & TEXT-ALE-Flight\n");

    strncpy(message, "TRACE-FINFO-SON, LUM & TEXT-ALA-Flight\n", sizeof(message));
    for (int i = 0; i < 5; i++) {
        action(message);
    }
    CU_ASSERT_EQUAL(flight_information.it, 5);
    CU_ASSERT_STRING_EQUAL(flight_information.content, "ACTION-FINFO-SON, LUM & TEXT-ALA-Flight\n");
}


typedef struct {
  const char *test_name;
  void (*test_function)(void);
} TestCase;

TestCase test_cases[] = {
    {"Test for engine alert", test_action_engine_alert},
    {"Test for engine alarm", test_action_engine_alarm},
    {"Test for engine nothing", test_action_engine_nothing},
    {"Test for hydraulic alert", test_action_hydraulic_alert},
    {"Test for hydraulic alarm", test_action_hydraulic_alarm},
    {"Test for hydraulic nothing", test_action_hydraulic_nothing},
    {"Test for pressure alert", test_action_pressure_alert},
    {"Test for pressure alarm", test_action_pressure_alarm},
    {"Test for pressure nothing", test_action_pressure_nothing},
    {"Test for loads alert", test_action_loads_alert},
    {"Test for loads alarm", test_action_loads_alarm},
    {"Test for loads nothing", test_action_loads_nothing},
    {"Test for cabin alert", test_action_cabin_alert},
    {"Test for cabin alarm", test_action_cabin_alarm},
    {"Test for cabin nothing", test_action_cabin_nothing},
    {"Test for landing alert", test_action_landing_alert},
    {"Test for landing alarm", test_action_landing_alarm},
    {"Test for landing nothing", test_action_landing_nothing},
    {"Test for flight management alert", test_action_flight_information_alert},
    {"Test for flight management alarm", test_action_flight_information_alarm},
    {"Test for flight management nothing",
     test_action_flight_information_nothing},
    {"Test for simple engine", test_action_simple_engine},
    {"Test for simple hydraulic", test_action_simple_hydraulic},
    {"Test for simple pressure", test_action_simple_pressure},
    {"Test for simple loads", test_action_simple_loads},
    {"Test for simple cabin", test_action_simple_cabin},
    {"Test for simple landing gear", test_action_simple_landing_gear},
    {"Test for simple flight information", test_action_simple_flight_information},
};


int main() {
  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  CU_pSuite suite = CU_add_suite("Suite_Action_Tests", NULL, NULL);
  if (suite == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (CU_add_test(suite, "Test for input too long", test_action_too_long) ==
      NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(TestCase); i++) {
    if (CU_add_test(suite, test_cases[i].test_name,
                    test_cases[i].test_function) == NULL) {
      CU_cleanup_registry();
      return CU_get_error();
    }
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}