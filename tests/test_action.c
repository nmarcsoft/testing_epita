#include "../src/decode_and_log.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

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

int main(int argc, char *argv[]) {
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

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
