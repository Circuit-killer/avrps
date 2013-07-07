#ifndef INCLUDED_avr_h
#define INCLUDED_avr_h

#if 0
#    include <avr/cpufunc.h>
#endif

#define ARRAY_SIZE(a) (sizeof (a) / sizeof (a)[0])

/* Potentially one less operation than naive version. Taken from
https://graphics.stanford.edu/~seander/bithacks.html#MaskedMerge
#define MASK_MERGE(zero, mask, one) ( ((zero) ^ (one)) & (mask) ^ (zero) )
Alas! it does not optimise as well when constants are used. */

#define MASK_MERGE(zero, mask, one) ( ((zero) & ~(mask)) | ((one) & (mask)) )
#define BIT_MERGE(other, pos, val) \
    MASK_MERGE(other, 1 << (pos), (val) << (pos))

#define REG_MOD(reg, mask, val) ( (reg) = MASK_MERGE(reg, mask, val) )
#define FIELD_MOD(reg, field, mask, val) \
    REG_MOD(reg, (mask) << (field), (val) << (field))
#define BIT_MOD(reg, field, val) ( (reg) = BIT_MERGE(reg, field, val) )

/* _MemoryBarrier provided by <avr/cpufunc.h> causes:
    error: expected string literal before ':' token
Including empty assembler text seems to avoid this. */
#undef _MemoryBarrier
#define _MemoryBarrier() asm volatile("" : : : "memory")

#define DD_INPUT 0
#define DD_OUTPUT 1
#define PORT_FLOAT 0
#define PORT_PULLUP 1
#define PIN_TOGGLE 1

#if defined(__AVR_ATtiny2313__)
#    define CKSEL_MASK (~0x0F)
#    define CKSEL_EXT (0x00 | CKSEL_MASK)
#    define CKSEL_4MHZ (0x02 | CKSEL_MASK)
#    define CKSEL_8MHZ (0x04 | CKSEL_MASK)
#    define CKSEL_128KHZ (0x06 | CKSEL_MASK)
#elif defined(__AVR_ATtiny13__)
#    define CKSEL_MASK (~0x03)
#    define CKSEL_EXT (0x00 | CKSEL_MASK)
#    define CKSEL_4MHZ8 (0x01 | CKSEL_MASK)
#    define CKSEL_9MHZ6 (0x02 | CKSEL_MASK)
#    define CKSEL_128KHZ (0x03 | CKSEL_MASK)

#    define SUT_MASK (~0x0C)
#    define SUT_ZERO (0x00 | SUT_MASK)
#    define SUT_4MS (0x04 | SUT_MASK)
#    define SUT_64MS (0x08 | SUT_MASK)
#endif

#define COM_MASK 3 /* Compare output modes for timer-counters */
#define COM_DISCON 0 /* Normal port operation, OC disconnected */
#define COM_TOGGLE 1 /* Toggle OC on compare match */
#define COM_CLEAR 2
#define COM_SET 3
#define COM_PWM_NONINV 2 /* Non-inverted PWM: OC set for TCNT < OCR */
#define COM_PWM_INV 3 /* Inverted PWM: OC set for TCNT > OCR */

#define WGM0_MASK 3 /* Waveform generation modes for Timer-counter 0 */
#define WGM0_NORMAL 0 /* Free-running */
#define WGM0_PWM_PC 1 /* Phase-correct PWM */
#define WGM0_CTC 2 /* Clear timer on compare-match */
#define WGM0_PWM_FAST 3

#define WGM0A_BIT WGM00 /* WGM0 field within TCCR0A */
#define WGM0A_MASK 3
#define WGM02_NORMAL_CTC 0
#    define WGM0A_NORMAL 0
#    define WGM0A_CTC 2
#define WGM02_PWM_FIXED 0
#define WGM02_PWM_VARIABLE 1
#    define WGM0A_PWM_PC 1 /* Phase-correct */
#    define WGM0A_PWM_FAST 3

#define PWM10 WGM10 /* PWM1 a.k.a. WGM1 field in TCCR1A */
#define PWM1_MASK 3
#define WGM1B_BIT WGM12 /* WGM1 field in TCCR1B */
#define WGM1B_MASK 3
#define PWM1_NORMAL_CTC 0
#    define WGM1B_NORMAL 0 /* Free-running */
#    define CTC1 WGM12
#        define WGM13_OCRA 0
#        define WGM13_ICR 1
#define WGM13_PWM_FIXED 0
#    define WGM12_FIXED_PC 0 /* Phase-correct */
#    define WGM12_FIXED_FAST 1
#    define PWM1_8BIT 1
#    define PWM1_9BIT 2
#    define PWM1_10BIT 3
#define WGM13_PWM_VARIABLE 1
#    define WGM12_VAR_CORRECTED 0
#        define PWM11_PHASE_FREQ 0 /* Phase and frequency correct */
#        define PWM11_PHASE 1 /* Phase-correct */
#    define WGM12_VAR_FAST 1
#        define PWM11_FAST 1
#    define PWM10_ICR 0
#    define PWM10_OCRA 1

#define CS_MASK 7 /* Clock selections for timer-counters */
#define CS_NONE 0 /* Stopped */
#define CS_CLK 1 /* No prescaling */
#define CS_CLK_8 2 /* Prescale by 8 */
#define CS_CLK_64 3
#define CS_CLK_256 4
#define CS_CLK_1024 5
#define CS_FALLING 6 /* External input, falling edge */
#define CS_RISING 7 /* External input, rising edge */

/* CTC waveform generation mode options for Timer-counter 1 */
#define WGM12_NORMAL 0
#define WGM12_CTC 1

#define ACIS_MASK 3
#define ACIS_TOGGLE 0
#define ACIS_FALLING 2
#define ACIS_RISING 3

#define MUX_MASK 3

#define WDP_MASK 0x27
#define WDP_16MS 0x00
#define WDP_32MS 0x01
#define WDP_64MS 0x02
#define WDP_0S125 0x03
#define WDP_0S25 0x04
#define WDP_0S5 0x05
#define WDP_1S 0x06
#define WDP_2S 0x07
#define WDP_4S 0x20
#define WDP_8S 0x21

#define OC0A_BIT 0
#define OC0B_BIT 1

#define ADC0_BIT 5
#define ADC1_BIT 2
#define ADC2_BIT 4
#define ADC3_BIT 3

#define AIN0_BIT 0
#define AIN1_BIT 1

#endif
