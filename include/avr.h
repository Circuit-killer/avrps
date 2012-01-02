#ifndef INCLUDED_avr_h
#define INCLUDED_avr_h

#if 0
#    include <avr/cpufunc.h>
#endif

#define ARRAY_SIZE(a) (sizeof (a) / sizeof (a)[0])

#define REG_MOD(reg, mask, val) \
    ( (reg) = ((reg) & ~(mask)) | ((val) & (mask)) )
#define FIELD_MOD(reg, field, mask, val) \
    REG_MOD(reg, (mask) << (field), (val) << (field))
#define BIT_MOD(reg, field, val) FIELD_MOD(reg, field, 1, val)

/* _MemoryBarrier provided by <avr/cpufunc.h> causes:
    error: expected string literal before ':' token
Including empty assembler text seems to avoid this. */
#undef _MemoryBarrier
#define _MemoryBarrier() asm volatile("" : : : "memory")

#define CKSEL_MASK 0x0F
#define CKSEL_EXT 0x00
#define CKSEL_4MHZ 0x02
#define CKSEL_8MHZ 0x04
#define CKSEL_128KHZ 0x06

#define COM_MASK 3 /* Compare output modes for timer-counters */
#define COM_DISCON 0 /* Normal port operation, OC disconnected */
#define COM_TOGGLE 1 /* Toggle OC on compare match */

#define WGM0_MASK 3 /* Waveform generation modes for Timer-counter 0 */
#define WGM0_NORMAL 0 /* Free-running */
#define WGM0_PWM_PC 1 /* Phase-correct PWM */
#define WGM0_CTC 2 /* Clear timer on compare-match */
#define WGM0_PWM_FAST 3

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

#endif
