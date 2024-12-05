#include "../src/decode_and_log.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

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

void test_decode_files_from_list(void) {
  const char *file_list = "tests/input_test.txt";
  FILE *list_file = fopen(file_list, "r");
  FILE *report_file = fopen("report_test.txt", "a+");

  if (list_file == NULL) {
    CU_FAIL("Failed to open file_list.txt");
    return;
  }

  if (report_file == NULL) {
    CU_FAIL("Failed to open report_test.txt");
    fclose(list_file);
    return;
  }

  char filename[256];
  while (fgets(filename, sizeof(filename), list_file)) {
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
      filename[len - 1] = '\0';
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
      log_report(report_file, "Failed to open file: %s\n", filename);
      CU_FAIL("Failed to open one of the listed files");
      continue;
    }

    int result = decode_and_log(file);
    fclose(file);

    if (result == 0) {
      log_report(report_file, "decode_and_log(%s) succeeded\n", filename);
    } else {
      log_report(report_file, "decode_and_log(%s) failed\n", filename);
      CU_FAIL("decode_and_log returned an error for one file");
    }
  }

  fclose(list_file);
  fclose(report_file);
}

void test_decode(const char *name, FILE *report_file) {
  FILE *file = fopen(name, "r");
  if (file == NULL) {
    log_report(report_file, "decode_and_log(%s) failed to open file\n", name);
    CU_FAIL("Failed to open input file");
    return;
  }

  int result = decode_and_log(file);
  fclose(file);

  if (result == 0) {
    log_report(report_file, "decode_and_log(%s) succeeded\n", name);
  } else {
    log_report(report_file, "decode_and_log(%s) failed\n", name);
  }

  CU_ASSERT(result == 0);
}

void test_decode_and_log_for_input_FW(void) {
  FILE *report_file = fopen("report_test.txt", "a+");
  if (report_file == NULL) {
    CU_FAIL("Failed to open report file");
    return;
  }
  test_decode("input_FW.txt", report_file);
  fclose(report_file);
}

void test_decode_and_log_for_input_FW_test(void) {
  FILE *report_file = fopen("report_test.txt", "a+");
  if (report_file == NULL) {
    CU_FAIL("Failed to open report file");
    return;
  }
  test_decode("test/input_test_FW.txt", report_file);
  fclose(report_file);
}

void test_from_input(void) {}

int init_suite(void) { return 0; }

int clean_suite(void) { return 0; }

int main(void) {
  if (CUE_SUCCESS != CU_initialize_registry()) {
    log_report(stderr, "CUnit initialization failed\n");
    return CU_get_error();
  }

  CU_pSuite pSuite =
      CU_add_suite("DecodeAndLog Suite", init_suite, clean_suite);
  if (pSuite == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (CU_add_test(pSuite, "Test decode_and_log with input_FW.txt",
                  test_decode_and_log_for_input_FW) == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (CU_add_test(pSuite, "Test decode_and_log with test/input_FW_test.txt",
                  test_decode_and_log_for_input_FW_test) == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (CU_add_test(pSuite, "Test decode_and_log with file list",
                  test_decode_files_from_list) == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
