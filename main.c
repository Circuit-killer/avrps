#include <avr.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <avr/wdt.h>

static void switch_on(void);

/* ADC2: 1/2 battery voltage */
#define BAT_ADC 2
#define BAT_BIT ADC2_BIT

FUSES = {
    .low = MASK_MERGE(SUT_ZERO & CKSEL_9MHZ6 & FUSE_CKDIV8,
        SUT_MASK & CKSEL_MASK & FUSE_CKDIV8, LFUSE_DEFAULT),
    .high = HFUSE_DEFAULT,
};
#define F_CPU (9600UL * 1000 / 8)

static uint8_t flash_time;
static uint8_t button_time = 0;

static enum {MODE_ON, MODE_SHUTDOWN, MODE_OFF} mode;

int main(void) {
    switch_on();
    
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
    if(mode == MODE_ON && 0 == --flash_time) {
        if(OCR0B <= 13 - 1) {
            OCR0B = 20 - 1;
            flash_time = 2;
        } else {
            OCR0B = 6 - 1;
            flash_time = 10;
        }
    }
    
    if(button_time && 0 == --button_time) {
        switch(mode) {
        case MODE_ON:
            OCR0B = 6 - 1;
            mode = MODE_SHUTDOWN;
            break;
        case MODE_SHUTDOWN:
            OCR0B = 5 - 1;
            mode = MODE_OFF;
            break;
        case MODE_OFF:
            switch_on();
            break;
        }
    }
}

ISR(ANA_COMP_vect) {
    switch(mode) {
    case MODE_ON:
        if(0 == (ACSR >> ACO & 1)) {
            button_time = 80;
        } else {
            button_time = 0;
        }
        break;
    case MODE_SHUTDOWN:
    case MODE_OFF:
        if(1 == (ACSR >> ACO & 1)) {
            button_time = 3;
        } else {
            button_time = 0;
        }
        break;
    }
}

static void switch_on(void) {
    /* Set waveform generation mode (WGM) to fast PWM, with TOP = OCR0A
    ("variable") */
    OCR0A = 24 - 1; /* F_CPU / 50 kHz - 1 */
    OCR0B = 20 - 1; /* 84.5% * 24 */
    TCNT0 = 0;
    TCCR0A = WGM0A_PWM_FAST << WGM0A_BIT | COM_PWM_NONINV << COM0B0;
    TCCR0B = WGM02_PWM_VARIABLE << WGM02 | CS_CLK << CS00;
    
    mode = MODE_ON;
    flash_time = 25;
}
