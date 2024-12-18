#include "../src/decode_and_log.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <setjmp.h>
#include <signal.h>

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

void report_result(char *actual_log_content, char *expected_log_content,
                   char *test_name) {
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
}

void traceStart() {
  char testTraceStart[256] = "TRACE-OTHER";
  char solutionTraceStart[256] = "ACTION-OTHER";
  replaceTRACE(testTraceStart);
  CU_ASSERT_STRING_EQUAL(testTraceStart, solutionTraceStart);
  report_result(testTraceStart, solutionTraceStart, "traceStart");
  CU_PASS("traceStart");
}

void traceEnd() {
  char testTraceEnd[256] = "OTHER-TRACE";
  char solutionTraceEnd[256] = "OTHER-ACTION";
  replaceTRACE(testTraceEnd);
  CU_ASSERT_STRING_EQUAL(testTraceEnd, solutionTraceEnd);
  report_result(testTraceEnd, solutionTraceEnd, "traceEnd");
  CU_PASS("traceEnd");
}

void traceMiddle() {
  char testTraceMiddle[256] = "OTHER-TRACE-OTHER";
  char solutionTraceMiddle[256] = "OTHER-ACTION-OTHER";
  replaceTRACE(testTraceMiddle);
  CU_ASSERT_STRING_EQUAL(testTraceMiddle, solutionTraceMiddle);
  report_result(testTraceMiddle, solutionTraceMiddle, "traceMiddle");
  CU_PASS("traceMiddle");
}

void traceMissing() {
  char testTraceMissing[256] = "OTHER";
  char solutionTraceMissing[256] = "OTHER";
  replaceTRACE(testTraceMissing);
  CU_ASSERT_STRING_EQUAL(testTraceMissing, solutionTraceMissing);
  report_result(testTraceMissing, solutionTraceMissing, "traceMissing");
  CU_PASS("traceMissing");
}

void traceMultiple() {
  char testTraceMultiple[256] = "OTHER-TRACE-TRACE-OTHER";
  char solutionTraceMultiple[256] = "OTHER-ACTION-ACTION-OTHER";
  replaceTRACE(testTraceMultiple);
  CU_ASSERT_STRING_EQUAL(testTraceMultiple, solutionTraceMultiple);
  report_result(testTraceMultiple, solutionTraceMultiple, "traceMultiple");
  CU_PASS("traceMultiple");
}

void tooLong() {
  char testTooLong[350] =
      "TRACE-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER";
  char solutionTooLong[350] =
      "TRACE-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-OTHER-"
      "OTHER";
  replaceTRACE(testTooLong);
  CU_ASSERT_STRING_EQUAL(testTooLong, solutionTooLong);
  report_result(testTooLong, solutionTooLong, "traceTooLong");
  CU_PASS("tooLong");
}

void tooShort() {
  char testTooShort[100] = "TRACE-OTHER";
  char solutionTooShort[100] = "TRACE-OTHER";
  replaceTRACE(testTooShort);
  CU_ASSERT_STRING_EQUAL(testTooShort, solutionTooShort);
  report_result(testTooShort, solutionTooShort, "traceTooShort");
  CU_PASS("tooShort");
}

void traceLower() {
  char testTraceLower[256] = "trace-OTHER";
  char solutionTraceLower[100] = "trace-OTHER";
  replaceTRACE(testTraceLower);
  CU_ASSERT_STRING_EQUAL(testTraceLower, solutionTraceLower);
  report_result(testTraceLower, solutionTraceLower, "traceLower");
  CU_PASS("traceLower");
}

void traceUpperAndLower() {
  char testTraceUpperAndLower[256] = "TraCE-OTHER";
  char solutionTraceUpperAndLower[256] = "TraCE-OTHER";
  replaceTRACE(testTraceUpperAndLower);
  CU_ASSERT_STRING_EQUAL(testTraceUpperAndLower, solutionTraceUpperAndLower);
  report_result(testTraceUpperAndLower, solutionTraceUpperAndLower,
                "traceUpperAndLower");
  CU_PASS("traceUpperAndLower");
}

void badChars1() {
  char testBadChars1[256] = "\nTRACE-OTHER";
  char solutionBadChars1[256] = "\nACTION-OTHER";
  replaceTRACE(testBadChars1);
  CU_ASSERT_STRING_EQUAL(testBadChars1, solutionBadChars1);
  report_result(testBadChars1, solutionBadChars1, "traceBadChars1");
  CU_PASS("badChars1");
}

void badChars2() {
  char testBadChars2[256] = "TRA\nCE-OTHER";
  char solutionBadChars2[256] = "TRA\nCE-OTHER";
  replaceTRACE(testBadChars2);
  CU_ASSERT_STRING_EQUAL(testBadChars2, solutionBadChars2);
  report_result(testBadChars2, solutionBadChars2, "traceBadChars2");
  CU_PASS("badChars2");
}

void badChars3() {
  char testBadChars3[256] = "TRACE\n-OTHER";
  char solutionBadChars3[256] = "ACTION\n-OTHER";
  replaceTRACE(testBadChars3);
  CU_ASSERT_STRING_EQUAL(testBadChars3, solutionBadChars3);
  report_result(testBadChars3, solutionBadChars3, "traceBadChars3");
  CU_PASS("badChars3");
}

void badChars4() {
  char testBadChars4[256] = "TRACE-OTHER\n";
  char solutionBadChars4[256] = "ACTION-OTHER\n";
  replaceTRACE(testBadChars4);
  CU_ASSERT_STRING_EQUAL(testBadChars4, solutionBadChars4);
  report_result(testBadChars4, solutionBadChars4, "traceBadChars4");
  CU_PASS("badChars4");
}

void badChars5() {
  char testBadChars5[256] = "\0TRACE-OTHER";
  char solutionBadChars5[256] = "\0TRACE-OTHER";
  replaceTRACE(testBadChars5);
  CU_ASSERT_STRING_EQUAL(testBadChars5, solutionBadChars5);
  report_result(testBadChars5, solutionBadChars5, "traceBadChars5");
  CU_PASS("badChars5");
}

void badChars6() {
  char testBadChars6[256] = "TRA\0CE-OTHER";
  char solutionBadChars6[256] = "TRA\0CE-OTHER";
  replaceTRACE(testBadChars6);
  CU_ASSERT_STRING_EQUAL(testBadChars6, solutionBadChars6);
  report_result(testBadChars6, solutionBadChars6, "traceBadChars6");
  CU_PASS("badChars6");
}

void badChars7() {
  char testBadChars7[256] = "TRACE\0-OTHER";
  char solutionBadChars7[256] = "ACTION\0-OTHER";
  replaceTRACE(testBadChars7);
  CU_ASSERT_STRING_EQUAL(testBadChars7, solutionBadChars7);
  report_result(testBadChars7, solutionBadChars7, "traceBadChars7");
  CU_PASS("badChars7");
}

void badChars8() {
  char testBadChars8[256] = "TRACE-OTHER\0";
  char solutionBadChars8[256] = "ACTION-OTHER\0";
  replaceTRACE(testBadChars8);
  CU_ASSERT_STRING_EQUAL(testBadChars8, solutionBadChars8);
  report_result(testBadChars8, solutionBadChars8, "traceBadChars8");
  CU_PASS("badChars8");
}

static jmp_buf jump_buffer;

void signal_handler(int sig) {
  printf("Crash détecté : Signal %d reçu.\n", sig);
  longjmp(jump_buffer, 1);
}

void badType1() {
  signal(SIGSEGV, signal_handler);

  if (setjmp(jump_buffer) == 0) {
    int testBadType1 = 123;
    replaceTRACE(testBadType1);
    CU_PASS();
    report_result("", "", "badType1");
  } else {
    CU_FAIL("SIGSEV détecté");
    report_result("", "0", "badType1");
  }
}

void badType2() {
  signal(SIGSEGV, signal_handler);
  if (setjmp(jump_buffer) == 0) {
    int testBadType1 = 123;
    replaceTRACE(NULL);
    CU_PASS();
    report_result("", "", "badType2");
  } else {
    CU_FAIL("SIGSEV détecté");
    report_result("", "0", "badType2");
  }
}

void badType3() {
  signal(SIGSEGV, signal_handler);

  if (setjmp(jump_buffer) == 0) {
    int testBadType1 = 123;
    CU_PASS();
    report_result("","", "badType3");
  } else {
    CU_FAIL("SIGSEV détecté");
    void *testBadType3 = "TRACE-OTHER";
    void *solutionBadType3 = "ACTION-OTHER";
    replaceTRACE(testBadType3);
    CU_ASSERT_STRING_EQUAL(testBadType3, solutionBadType3);
    report_result("", "0", "badType3");
  }
}

int main() {
  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  CU_pSuite suite = CU_add_suite("Test Suite for replaceTRACE function", 0, 0);
  if (suite == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if (CU_add_test(suite, "traceStart", traceStart) == NULL ||
      CU_add_test(suite, "traceEnd", traceEnd) == NULL ||
      CU_add_test(suite, "traceMiddle", traceMiddle) == NULL ||
      CU_add_test(suite, "traceMissing", traceMissing) == NULL ||
      CU_add_test(suite, "traceMultiple", traceMultiple) == NULL ||
      CU_add_test(suite, "tooLong", tooLong) == NULL ||
      CU_add_test(suite, "tooShort", tooShort) == NULL ||
      CU_add_test(suite, "traceLower", traceLower) == NULL ||
      CU_add_test(suite, "traceUpperAndLower", traceUpperAndLower) == NULL ||
      CU_add_test(suite, "badChars1", badChars1) == NULL ||
      CU_add_test(suite, "badChars2", badChars2) == NULL ||
      CU_add_test(suite, "badChars3", badChars3) == NULL ||
      CU_add_test(suite, "badChars4", badChars4) == NULL ||
      CU_add_test(suite, "badChars5", badChars5) == NULL ||
      CU_add_test(suite, "badChars6", badChars6) == NULL ||
      CU_add_test(suite, "badChars7", badChars7) == NULL ||
      CU_add_test(suite, "badChars8", badChars8) == NULL ||
      CU_add_test(suite, "badType1", badType1) == NULL ||
      CU_add_test(suite, "badType2", badType2) == NULL ||
      CU_add_test(suite, "badType3", badType3) == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();

  CU_cleanup_registry();
  return CU_get_error();
}
