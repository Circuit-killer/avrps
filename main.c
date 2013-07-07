#include <avr.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>

FUSES = {
    .low = MASK_MERGE(SUT_ZERO & CKSEL_9MHZ6 & FUSE_CKDIV8,
        SUT_MASK & CKSEL_MASK & FUSE_CKDIV8, LFUSE_DEFAULT),
    .high = HFUSE_DEFAULT,
};
#define F_CPU (9600UL * 1000 / 8)

static uint16_t c2 = 10000;

int main(void) {
    /* Set waveform generation mode (WGM) to fast PWM, with TOP = OCR0A
    ("variable") */
    REG_MOD(TCCR0A, WGM0A_MASK << WGM0A_BIT | COM_MASK << COM0B0,
        WGM0A_PWM_FAST << WGM0A_BIT | COM_PWM_NONINV << COM0B0);
    REG_MOD(TCCR0B, 1 << WGM02 | CS_MASK << CS00,
        WGM02_PWM_VARIABLE << WGM02 | CS_CLK << CS00);
    OCR0A = 24 - 1; /* F_CPU / 50 kHz - 1 */
    OCR0B = 20 - 1; /* 84.5% * 24 */
    TCNT0 = 0;
    sei();
    
    /* Pull-ups on all unused IO pins to avoid floating inputs causing excess
    power consumption */
    DDRB = MASK_MERGE(~0 * DD_INPUT, 1 << OC0B_BIT | 1 << 2, ~0 * DD_OUTPUT);
    REG_MOD(PORTB, ~(1 << OC0B_BIT | 1 << 2), ~0 * PORT_PULLUP);
    
    BIT_MOD(TIMSK0, TOIE0, 1);
    
    while(1) {
        sleep_mode();
    }
}

ISR(TIM0_OVF_vect) {
    if(--c2) {
        return;
    }
    
    if(OCR0B <= 8 - 1) {
        OCR0B = 20 - 1;
        c2 = 800;
    } else {
        OCR0B = 6 - 1;
        c2 = 7000;
    }
}
