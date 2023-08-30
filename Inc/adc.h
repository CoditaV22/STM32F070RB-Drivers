
#ifndef ADC_H_
#define ADC_H_

void pa1_adc_init(void);
void adc_config(void);
void start_adc_single_conversion_mode(void);
int adc_read(void);

#endif /* ADC_H_ */
