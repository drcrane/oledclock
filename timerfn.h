#ifndef __TIMER_H__
#define __TIMER_H__

#define TIMER_MAX_CALLBACKS 10
#define TIMER_FLAGS_EMPTY    1

struct timer_context {
	int flags;
	int ticks[TIMER_MAX_CALLBACKS];
	void (*callback[TIMER_MAX_CALLBACKS])();
};

extern struct timer_context timer_ctx;

void timer_initialise();
int timer_callback(int ticks, void (*callback)());
void timer_docallbacks();
int timer_is_present(void (*callback)());
int timer_is_present_remove(void (*callback)());

#endif /* __TIMER_H__ */

