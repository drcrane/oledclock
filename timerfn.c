#include <msp430.h>
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
		// start Timer 1
	}
	return res;
}

void timer_set_dummy() {
	timer_ctx.flags |= TIMER_FLAGS_DUMMY;
}

void timer_clear_dummy() {
	timer_ctx.flags &= ~TIMER_FLAGS_DUMMY;
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

void timer_wait_for(int msDelay) {
	int i;
	void (* callback)();
//	timer_callback(msDelay, timer_set_dummy);
//timer_flags_dummy:
	__bis_SR_register(LPM0_bits | GIE);
	i = TIMER_MAX_CALLBACKS;
	do {
		i--;
		if (timer_ctx.callback[i] != NULL && timer_ctx.ticks[i] == 0) {
			callback = timer_ctx.callback[i];
			timer_ctx.callback[i] = NULL;
			callback();
		}
	} while (i != 0);
//	if (timeractx.flags & TIMER_FLAGS_DUMMY) {
//		goto timer_flags_dummy;
//	}
}

void timer_docallbacks() {
	int i;
	int flg;
	void (* callback)();
start_again:
	i = TIMER_MAX_CALLBACKS;
	flg = 0;
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
			flg = 1;
		}
//		__asm__ (" mov  #7, r15\n call #logdebugpos\n" ::: "r15");
	} while (i != 0);
	if (flg == 0) {
		timer_ctx.flags |= TIMER_FLAGS_EMPTY;
		// Stop timer 1
	}
}

/*
int sendChar(int source) {
	UCA0TXBUF = 'T'; while(!(IFG2 & UCA0TXIFG)) { __nop(); }
	timer_callback(1000, sendChar);
}
*/

/*
 * Execution time to enter the ISR is 6 cycles time to exit is 5 cycles
 * At 16MHz = 0.0625uS/cycle
 * 0.6875uS for enter and exit the ISR
 */

// note that we only decrement the ticks here
// a callback will always be called in the context
// of the main thread.
void Timer1_A0_ISR(void) __attribute__((interrupt(TIMER1_A0_VECTOR)));
void Timer1_A0_ISR(void) {
	int i;
	int flg;
	flg = 0;
	if (!(timer_ctx.flags & TIMER_FLAGS_EMPTY)) {
		i = TIMER_MAX_CALLBACKS;
		do {
			i--;
			if (timer_ctx.ticks[i] != 0) {
				timer_ctx.ticks[i] --;
			}
			if (timer_ctx.ticks[i] == 0 &&
					timer_ctx.callback[i] != NULL) {
				flg |= 1;
			}
		} while (i != 0);
	}
	if (flg) {
		__bic_SR_register_on_exit(CPUOFF);
	}
}

