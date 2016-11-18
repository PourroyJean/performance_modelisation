//
// Created by Jean Pourroy on 18/11/2016.
//

#include <stdio.h>
#include <check.h>

#include "Student.h"


START_TEST (test_str)
{
    fail_if(1, "Could not allocate s");

}
END_TEST

Suite * str_suite (void) {
    Suite *suite = suite_create("isti_str");
    TCase *tcase = tcase_create("case");
    tcase_add_test(tcase, test_str);
    suite_add_tcase(suite, tcase);
    return suite;
}

int main (int argc, char *argv[]) {
    int number_failed;
    Suite *suite = str_suite();
    SRunner *runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return number_failed;
}