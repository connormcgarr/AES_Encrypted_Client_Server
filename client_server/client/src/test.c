#ifdef TEST
#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include"../hdr/client.h"

#include <stdio.h>  // for printf

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/



void client_test_1(void) {
  CU_ASSERT_EQUAL( client(1,2), 2);
  CU_ASSERT_EQUAL( client(2,1), 2);
}

void client_test_2(void) {
  CU_ASSERT_EQUAL( client(2,2), 2);
  CU_ASSERT_EQUAL( client(0,0), 0);
  CU_ASSERT_EQUAL( client(-1,-1), -1);
}

void client_test_3(void) {
  CU_ASSERT_EQUAL( client(-1,-2), -1);
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite pSuite = NULL;
    //unsigned int ERROR = 0xFF;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite( "client_test_suite", init_suite, clean_suite );
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(pSuite, "client_test_1", client_test_1)) ||
        (NULL == CU_add_test(pSuite, "client_test_2", client_test_2)) ||
        (NULL == CU_add_test(pSuite, "client_test_3", client_test_3))
      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");
   /* Clean up registry and return */
   CU_cleanup_registry();
   return CU_get_error();

 }
#endif
