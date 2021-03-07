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

static void timer_start() {
#ifndef COMPILE_FOR_UNIT_TEST
	TA1CTL |= MC_1;
#endif /* COMPILE_FOR_UNIT_TEST */
}

static void timer_stop() {
#ifndef COMPILE_FOR_UNIT_TEST
	TA1CTL &= ~(MC_1);
#endif /* COMPILE_FOR_UNIT_TEST */
}

/* timerctx.flags is critical. */
int timer_callback(int ticks, void (*callback)()) {
	int i;
	int res;
	res = 1;
	i = TIMER_MAX_CALLBACKS;
	do {
		i--;
		if (timer_ctx.callback[i] == NULL) {
			timer_ctx.ticks[i] = ticks;
			timer_ctx.callback[i] = callback;
			goto finish;
		}
	} while (i != 0);
	res = 0;
finish:
	if (timer_ctx.flags & TIMER_FLAGS_EMPTY) {
		timer_ctx.flags &= ~TIMER_FLAGS_EMPTY;
		//TA1CTL |= MC_1;
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
	void (* callback)();
start_again:
	i = TIMER_MAX_CALLBACKS;
//	__asm__ (" mov  #6, r15\n call #logdebugpos\n" ::: "r15");
	do {
		i--;
		if (timer_ctx.callback[i] != NULL) {
			if (timer_ctx.ticks[i] == 0) {
				callback = timer_ctx.callback[i];
				timer_ctx.callback[i] = NULL;
				callback();
				goto start_again;
			}
		}
//		__asm__ (" mov  #7, r15\n call #logdebugpos\n" ::: "r15");
	} while (i != 0);
}

/*
 * Execution time to enter the ISR is 6 cycles time to exit is 5 cycles
 * At 16MHz = 0.0625uS/cycle
 * 0.6875uS for enter and exit the ISR
 */

// note that we only decrement the ticks here
// a callback will always be called in the context
// of the main thread.
#ifndef COMPILE_FOR_UNIT_TEST
void Timer1_A0_ISR(void) __attribute__((interrupt(TIMER1_A0_VECTOR)));
#endif /* COMPILE_FOR_UNIT_TEST */
void Timer1_A0_ISR(void) {
	int i;
	int flg;
	flg = 0;
	if (!(timer_ctx.flags & TIMER_FLAGS_EMPTY)) {
		i = TIMER_MAX_CALLBACKS;
		do {
			i--;
			if (timer_ctx.callback[i] != NULL) {
				if (timer_ctx.ticks[i] != 0) {
					timer_ctx.ticks[i] --;
				}
				flg |= 2;
				if (timer_ctx.ticks[i] == 0) {
					flg |= 1;
				}
			}
		} while (i != 0);
	}
	if (flg & 1) {
#ifndef COMPILE_FOR_UNIT_TEST
		__bic_SR_register_on_exit(CPUOFF);
#else
		// timer_wake_cpu();
#endif /* COMPILE_FOR_UNIT_TEST */
	}
	if ((flg & 2) == 0) {
		timer_stop();
		timer_ctx.flags |= TIMER_FLAGS_EMPTY;
	}
}

