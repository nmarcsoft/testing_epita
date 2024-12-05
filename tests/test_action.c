#include "../src/decode_and_log.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

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

void test_action_simple_engine()
{
    struct Engine myEngine = { "", 0 };
    action("TRACE-ENG-SON, LUM & TEXT-ALE-Eng\n");
    //TODO : vérifier que it s'incrémente correctement, pour des alertes et alarmes
    //Appeler action 3 fois, plus alerte, plus 2 fois de plus, puis alarme
    //Faire cette fct pour tous les types : cabn flight information, loads, landing, etc...
    //Ajouter toutes ces fct dans le main
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
