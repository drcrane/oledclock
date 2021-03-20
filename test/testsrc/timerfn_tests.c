#include <stdio.h>
#include "../../timerfn.h"
#include "minunit.h"

/*
 * gcc -I include/ src/timerfn.c testsrc/timerfn_tests.c -o timerfn_tests
 */

#define TIMER_UNIT_TEST_FLAGS_WAKEUP_REQUEST 2
#define TIMER_UNIT_TEST_FLAGS_TIMER_RUNNING 1
void timer_unit_test_initialise();
int timer_unit_test_get_flags();

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

void timer_callback_set_bit_0() {
	flags |= 1;
}

void timer_callback_set_bit_1() {
	flags |= 2;
}

void timer_callback_add_two_callbacks() {
	timer_callback(0, timer_callback_set_bit_0);
	timer_callback(0, timer_callback_set_bit_1);
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

char * timer_stopped_when_no_callbacks_present_test() {
	timer_unit_test_initialise();
	flags = 0;
	timer_initialise();
	mu_assert(timer_unit_test_get_flags() == 0, "Timer should be off");
	timer_callback(1, timer_callback_set_bit_0);
	timer_callback(2, timer_callback_set_bit_1);
	Timer1_A0_ISR();
	mu_assert(timer_unit_test_get_flags() & TIMER_UNIT_TEST_FLAGS_WAKEUP_REQUEST,
		"Wakeup should be requested");
	timer_docallbacks();
	mu_assert(flags == 0x1, "Bit 0 should be set");
	mu_assert(timer_unit_test_get_flags() & TIMER_UNIT_TEST_FLAGS_TIMER_RUNNING,
		"Timer should still be running");
	Timer1_A0_ISR();
	timer_docallbacks();
	mu_assert(flags & 0x2 == 0x2, "Bit 1 should be set");
	debug("flags %x", flags);
	//timer_docallbacks();
	mu_assert((timer_unit_test_get_flags() & TIMER_UNIT_TEST_FLAGS_TIMER_RUNNING) == 0,
		"Timer should be stopped");
	Timer1_A0_ISR();
	Timer1_A0_ISR();
	//
	flags = 0;
	return NULL;
}

char * all_tests() {
	mu_suite_start();
	mu_run_test(timer_callback_after_one_tick_test);
	mu_run_test(timer_callback_predictable_test);
	mu_run_test(timer_stopped_when_no_callbacks_present_test);
	return NULL;
}

RUN_TESTS(all_tests)

