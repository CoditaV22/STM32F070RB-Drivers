
#ifndef TIM_H_
#define TIM_H_

void tim3_1Hz_init(void);
void tim3_pa6_Output_Compare(void);
void tim3_pa6_input_capture(void);

#define SR_UIF 	 (1U << 0)
#define SR_CC1IF (1U << 1)
#endif
