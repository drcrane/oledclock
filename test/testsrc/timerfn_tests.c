#include <stdio.h>
#include "../../timerfn.h"
#include "minunit.h"

/*
 * gcc -I include/ src/timerfn.c testsrc/timerfn_tests.c -o timerfn_tests
 */

void Timer1_A0_ISR(void);

int flags;

void callback_after_one_tick() {
	flags |= 1;
}

char * timer_callback_after_one_tick_test() {
	timer_initialise();
	flags = 0;
	timer_callback(1, callback_after_one_tick);
	timer_docallbacks();
	mu_assert(flags == 0, "callback should not have been called yet");
	Timer1_A0_ISR();
	timer_docallbacks();
	mu_assert(flags == 1, "callback should have been called by now");
	return NULL;
}

void timer_callback_predictable_one() {
	flags |= 1;
}

void timer_callback_predictable_two() {
	flags |= 2;
}

void timer_callback_add_two_callbacks() {
	timer_callback(0, timer_callback_predictable_one);
	timer_callback(0, timer_callback_predictable_two);
	flags |= 4;
}

char * timer_callback_predictable_test() {
	timer_initialise();
	flags = 0;
	timer_callback(1, timer_callback_add_two_callbacks);
	Timer1_A0_ISR();
	timer_docallbacks();
	debug("%x", flags);
	mu_assert(flags == 7, "all callbacks should now have been called");
	return NULL;
}

char * all_tests() {
	mu_suite_start();
	mu_run_test(timer_callback_after_one_tick_test);
	mu_run_test(timer_callback_predictable_test);
	return NULL;
}

RUN_TESTS(all_tests)

