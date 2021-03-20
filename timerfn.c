#ifndef COMPILE_FOR_UNIT_TEST
#include <msp430.h>
#endif /* COMPILE_FOR_UNIT_TEST */
#include <stdint.h>
#include <stddef.h>

#include "timerfn.h"

struct timer_context timer_ctx;

void timer_initialise() {
	int i;
	i = TIMER_MAX_CALLBACKS;
	do {
		i--;
		timer_ctx.callback[i] = NULL;
		timer_ctx.ticks[i] = 0;
	} while (i != 0);
	timer_ctx.flags = TIMER_FLAGS_EMPTY;
#ifndef COMPILE_FOR_UNIT_TEST
	TA1CTL = TASSEL_1 | ID_0 | TACLR;
	TA1CCR0 = 32767;
	TA1CCTL0 = CCIE;
#endif /* COMPILE_FOR_UNIT_TEST */
}

#ifndef COMPILE_FOR_UNIT_TEST
static void timer_start() {
	TA1CTL |= MC_1;
}

static void timer_stop() {
	TA1CTL &= ~(MC_1);
}
#endif /* COMPILE_FOR_UNIT_TEST */

/* timerctx.flags is critical. */
int timer_callback(int ticks, void (*callback)()) {
	int i;
	int res;
	res = 1;
	i = TIMER_MAX_CALLBACKS;
	do {
		i--;
		if (timer_ctx.callback[i] == NULL) {
			timer_ctx.ticks[i] = timer_ctx.sys_ticks + ticks;
			timer_ctx.callback[i] = callback;
			goto finish;
		}
	} while (i != 0);
	res = 0;
finish:
	if (timer_ctx.flags & TIMER_FLAGS_EMPTY) {
		timer_ctx.flags &= ~TIMER_FLAGS_EMPTY;
		timer_ctx.soonest_ticks = timer_ctx.ticks[i] - 1;
		timer_start();
	}
	return res;
}

int timer_is_present(void (*callback)()) {
	int i;
	i = TIMER_MAX_CALLBACKS;
	do {
		i--;
		if (timer_ctx.callback[i] == callback) {
			return 1;
		}
	} while (i != 0);
	return 0;
}

int timer_is_present_remove(void (*callback)()) {
	int i;
	i = TIMER_MAX_CALLBACKS;
	do {
		i--;
		if (timer_ctx.callback[i] == callback) {
			timer_ctx.callback[i] = NULL;
			return 1;
		}
	} while (i != 0);
	return 0;
}

void timer_docallbacks() {
	int i;
	void (* callback)() = NULL;
	int sys_ticks = timer_ctx.sys_ticks;
	int soonest_ticks = 0x7fff;
	int remaining;
start_again:
	i = TIMER_MAX_CALLBACKS;
	callback = NULL;
//	__asm__ (" mov  #6, r15\n call #logdebugpos\n" ::: "r15");
	do {
		i--;
		if (timer_ctx.callback[i] != NULL) {
			callback = timer_ctx.callback[i];
			remaining = timer_ctx.ticks[i] - sys_ticks;
			remaining --;
			if (remaining & 0x8000) {
				timer_ctx.callback[i] = NULL;
				callback();
				goto start_again;
			}
			if (remaining < soonest_ticks) {
				soonest_ticks = remaining;
			}
		}
//		__asm__ (" mov  #7, r15\n call #logdebugpos\n" ::: "r15");
	} while (i != 0);
	timer_ctx.soonest_ticks = soonest_ticks;
	if (callback == NULL) {
		timer_ctx.flags |= TIMER_FLAGS_EMPTY;
		timer_stop();
	}
}

/*
 * Execution time to enter the ISR is 6 cycles time to exit is 5 cycles
 * At 16MHz = 0.0625us/cycle
 * 0.6875us for enter and exit the ISR
 */

// note that we only decrement the ticks here
// a callback will always be called in the context
// of the main thread.
#ifndef COMPILE_FOR_UNIT_TEST
void Timer1_A0_ISR(void) __attribute__((interrupt(TIMER1_A0_VECTOR)));
#endif /* COMPILE_FOR_UNIT_TEST */
void Timer1_A0_ISR(void) {
	int sys_ticks;
	sys_ticks = timer_ctx.sys_ticks;
	sys_ticks++;
	timer_ctx.sys_ticks = sys_ticks;
	sys_ticks = timer_ctx.soonest_ticks - sys_ticks;
	if (sys_ticks & 0x8000) {
#ifndef COMPILE_FOR_UNIT_TEST
		__bic_SR_register_on_exit(CPUOFF);
#else
		timer_wakeup_cpu();
#endif /* COMPILE_FOR_UNIT_TEST */
	}
}

