#define COMPILE_FOR_UNIT_TEST

#define TIMER_UNIT_TEST_FLAGS_WAKEUP_REQUEST 2
#define TIMER_UNIT_TEST_FLAGS_TIMER_RUNNING 1

int timer_unit_test_flags;

void timer_unit_test_initialise() {
	timer_unit_test_flags = 0;
}

static void timer_wakeup_cpu() {
	timer_unit_test_flags |= TIMER_UNIT_TEST_FLAGS_WAKEUP_REQUEST;
}

static void timer_start() {
	timer_unit_test_flags |= TIMER_UNIT_TEST_FLAGS_TIMER_RUNNING;
}

static void timer_stop() {
	timer_unit_test_flags &= ~TIMER_UNIT_TEST_FLAGS_TIMER_RUNNING;
}

int timer_unit_test_get_flags() {
	return timer_unit_test_flags;
}

#include "../../timerfn.c"

