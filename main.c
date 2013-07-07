#include <avr.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <avr/wdt.h>

/* ADC2: 1/2 battery voltage */
#define BAT_ADC 2
#define BAT_BIT ADC2_BIT

FUSES = {
    .low = MASK_MERGE(SUT_ZERO & CKSEL_9MHZ6 & FUSE_CKDIV8,
        SUT_MASK & CKSEL_MASK & FUSE_CKDIV8, LFUSE_DEFAULT),
    .high = HFUSE_DEFAULT,
};
#define F_CPU (9600UL * 1000 / 8)

static uint8_t flash_time = 25;
static uint8_t button_time = 0;

int main(void) {
    /* Set waveform generation mode (WGM) to fast PWM, with TOP = OCR0A
    ("variable") */
    OCR0A = 24 - 1; /* F_CPU / 50 kHz - 1 */
    OCR0B = 20 - 1; /* 84.5% * 24 */
    TCNT0 = 0;
    REG_MOD(TCCR0A, WGM0A_MASK << WGM0A_BIT | COM_MASK << COM0B0,
        WGM0A_PWM_FAST << WGM0A_BIT | COM_PWM_NONINV << COM0B0);
    REG_MOD(TCCR0B, 1 << WGM02 | CS_MASK << CS00,
        WGM02_PWM_VARIABLE << WGM02 | CS_CLK << CS00);
    
    DDRB = MASK_MERGE(~0 * DD_INPUT, 1 << OC0B_BIT | 1 << 2, ~0 * DD_OUTPUT);
    
    /* Unused pins pulled up to avoid floating inputs using excess power.
    Input pins floating. */
    REG_MOD(PORTB, ~(1 << OC0B_BIT | 1 << 2),
        MASK_MERGE(~0 * PORT_PULLUP,
            1 << AIN0_BIT | 1 << BAT_BIT, ~0 * PORT_FLOAT));
    DIDR0 = ~0;
    
    BIT_MOD(ADCSRA, ADEN, 0);
    REG_MOD(ADMUX, MUX_MASK << MUX0, BAT_ADC << MUX0);
    BIT_MOD(ADCSRB, ACME, 1);
    ACSR = 0 << ACD | 0 << ACBG | ACIS_TOGGLE << ACIS0;
    BIT_MOD(ACSR, ACI, 1);
    
    WDTCR = 1 << WDTIE | WDP_16MS;
    
    BIT_MOD(ACSR, ACIE, 1);
    sei();
    
    while(1) {
        sleep_mode();
    }
}

ISR(WDT_vect) {
    if(flash_time && 0 == --flash_time) {
        if(OCR0B <= 13 - 1) {
            OCR0B = 20 - 1;
            flash_time = 2;
        } else {
            OCR0B = 6 - 1;
            flash_time = 10;
        }
    }
    
    if(button_time && 0 == --button_time) {
        OCR0B = 6 - 1;
        flash_time = 0;
    }
}

ISR(ANA_COMP_vect) {
    if(!bit_is_set(ACSR, ACO)) {
        button_time = 80;
    } else {
        button_time = 0;
    }
}
