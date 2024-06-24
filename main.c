#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Define.h"
#include "Ultrasone_sensor.h"

#define DEBOUNCE _delay_ms(10)


typedef enum {START,
              SINGAAL,
              AUTOLR,
              RIJDEN,
              BOCHT_1,
              BOCHT_2,
              BOCHT_3,
              BOCHT_4,
              BOCHT_5,
              NOODSTOP,
              VOLG,
              STOPKNOP,
              BOOMSTOP,
              RESET,
              BOOMRESET} AGV;

// ISR voor PWM motor L
ISR(TIMER4_OVF_vect)
{
    PORTH ^= (1 << ENB);
}

ISR(TIMER4_COMPA_vect)
{
    PORTH |= (1 << ENB);
}

// ISR voor PWM motor R
ISR(TIMER1_OVF_vect)
{
    PORTH ^= (1 << ENA);
}

ISR(TIMER1_COMPA_vect)
{
    PORTH |= (1 << ENA);
}

void MOTORL(signed char percentage)
{
    if (percentage >= 0 && percentage <= 100)
    {
        OCR1A = (65536 * percentage) / 100;
    }
}

void MOTORR(signed char percentage)
{
    if (percentage >= 0 && percentage <= 100)
    {
        OCR4A = (65536 * percentage) / 100;
    }
}

void Rechtdoor()
{
    PORTB |=  (1 << IN1);
    PORTB &= ~(1 << IN2);
    PORTB &= ~(1 << IN3);
    PORTB |=  (1 << IN4);
}

void HBRUG_UIT()
{
    PORTB &= ~(1 << IN1);
    PORTB &= ~(1 << IN2);
    PORTB &= ~(1 << IN3);
    PORTB &= ~(1 << IN4);

}

void init()
{
    cli();  // Interrupts uitschakelen tijdens setup

    // Timer PWM motor L
    TCCR4A = (1 << WGM41);  // Fast PWM, TOP=ICR3
    TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS40);  // Prescaler = 1
    ICR4 = 65535;  // Set TOP value for 16-bit timer
    TIMSK4 = (1 << OCIE4A) | (1 << TOIE4);
    OCR4A = 0;

    // Timer PWM motor R
    TCCR1A = (1 << WGM11);  // Fast PWM, TOP=ICR1
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // Prescaler = 1
    ICR1 = 65535;  // Set TOP value for 16-bit timer
    TIMSK1 = (1 << OCIE1A) | (1 << TOIE1);
    OCR1A = 0;

    // Invoer pinnen
    DDRK &= ~((1 << START_knop) | (1 << stop_knop) | (1 << LM_switch) | (1 << RM_switch));

    // Uitvoer pinnen
    DDRH |= (1 << LED_TEST) | (1 << LED_TEST2);
    DDRL |= (1 << LED_L_GEEL) | (1 << LED_R_GEEL);
    DDRH |= (1 << ENA) | (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4) | (1 << ENB);

    // Zet uitvoer pinnen uit
    PORTH &= ~((1 << LED_TEST) | (1 << LED_TEST2));
    PORTL &= ~((1 << LED_L_GEEL) | (1 << LED_R_GEEL));

    // H-brug pinnen LOW
    PORTH &= ~((1 << ENA) | (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4) | (1 << ENB));

    agv_ultrasoon_init();

    sei();  // Interrupts inschakelen na setup
}

int main(void)
{
    AGV toestand = START;
    init();
    int baan = 0;
    int VOLGAAN = 0;
    int BOOM = 0;
    int LOM = 0;
    int ROM = 0;
    int boomR = 0;
    int boomL = 0;

    while (1)
    {
        if ((PINK & (1 << stop_knop)) != 0)
        {
            PORTB &= ~(1<<IN1);
            PORTB &= ~(1<<IN2);
            PORTB &= ~(1<<IN3);
            PORTB &= ~(1<<IN4);
            MOTORL(99);
            MOTORR(99);
            PORTH |= (1 << LED_TEST);
            PORTH |= (1 << LED_TEST2);
            PORTL |= (1 << LED_L_GEEL);
            PORTL |= (1 << LED_R_GEEL);
            toestand = NOODSTOP;
        }

        switch (toestand)
        {
            case NOODSTOP:
                if ((PINK & (1 << toestand_knop)) == 0) //knop op auto
                {
                    if ((PINK & (1 << START_knop)) != 0)
                    {
                        DEBOUNCE;
                        if ((PINK & (1 << START_knop)) == 0)
                        {
                            DEBOUNCE;
                            PORTH &= ~(1 << LED_TEST);
                            PORTH &= ~(1 << LED_TEST2);
                            PORTL &= ~(1 << LED_L_GEEL);
                            PORTL &= ~(1 << LED_R_GEEL);
                            toestand = SINGAAL;
                        }
                    }
                }
                if ((PINK & (1 << toestand_knop)) != 0) //knop op volg
                {
                    if ((PINK & (1 << START_knop)) != 0)
                    {
                        DEBOUNCE;
                        if ((PINK & (1 << START_knop)) == 0)
                        {
                            DEBOUNCE;
                            PORTH &= ~(1 << LED_TEST);
                            PORTH &= ~(1 << LED_TEST2);
                            PORTL &= ~(1 << LED_L_GEEL);
                            PORTL &= ~(1 << LED_R_GEEL);
                            toestand = SINGAAL;
                        }
                    }
                }
                if ((PINK &(1<<stop_knop))!=0)
                {
                    HBRUG_UIT();
                    baan = 0;
                    VOLGAAN = 0;
                    toestand = START;
                }

                break;

            case START:
                if ((PINK & (1 << toestand_knop)) == 0) //knop op auto
                {
                    if ((PINK & (1 << START_knop)) != 0)
                    {
                        DEBOUNCE;
                        PORTH |= (1 << LED_TEST); // aan
                        if ((PINK & (1 << START_knop)) == 0)
                        {
                            DEBOUNCE;
                            PORTH &= ~(1 << LED_TEST); // uit
                            toestand = SINGAAL;
                        }
                    }
                }
                if ((PINK & (1 << toestand_knop)) != 0) //knop op volg
                {
                    if ((PINK & (1 << START_knop)) != 0)
                    {
                        DEBOUNCE;
                        PORTH |= (1 << LED_TEST2); // aan
                        if ((PINK & (1 << START_knop)) == 0)
                        {
                            DEBOUNCE;
                            PORTH &= ~(1 << LED_TEST2); // uit
                            toestand = SINGAAL;
                        }
                    }
                }
                break;

            case SINGAAL:
                if ((PINK &( 1 << toestand_knop))==0)
                {
                PORTH |= (1 << LED_TEST); // AAN uit AAN uit
                _delay_ms(500);
                PORTH &= ~(1 << LED_TEST);
                _delay_ms(100);
                PORTH |= (1 << LED_TEST);
                _delay_ms(500);
                PORTH &= ~(1 << LED_TEST);
                _delay_ms(100);
                MOTORL(snelheidrechtdoor);
                MOTORR(snelheidrechtdoor);
                Rechtdoor();
                toestand = AUTOLR;
                }

                if ((PINK &(1<<toestand_knop))!=0)
                {
                PORTH |= (1 << LED_TEST2); // aan UIT aan UIT
                _delay_ms(100);
                PORTH &= ~(1 << LED_TEST2);
                _delay_ms(500);
                PORTH |= (1 << LED_TEST2);
                _delay_ms(100);
                PORTH &= ~(1 << LED_TEST2);
                _delay_ms(500);
                VOLGAAN += 1;
                toestand = VOLG;
                }
                break;

            case AUTOLR:
                if ((PINF & (1 << IR_R)) == 0) // muur dichtbij
                {
                    MOTORR(snelheidrechtdoor);
                }
                if ((PINF & (1 << IR_R)) != 0) // muur weg dus bijsturen andere kant
                {
                    MOTORR(snelheidbijsturen);
                }

                if ((PINF & (1 << IR_L)) == 0) // muur dichtbij
                {
                    MOTORL(snelheidrechtdoor);
                }
                if ((PINF & (1 << IR_L)) != 0) // muur weg dus bijsturen andere kant
                {
                    MOTORL(snelheidbijsturen);
                }

                if ((PINF & (1 << IR_V)) == 0) // Detectie & te ver
                {
                    PORTH |= (1 << LED_TEST); //aan
                    PORTH |= (1 << LED_TEST2); //aan
                    HBRUG_UIT();
                }

                if ((PINF & (1 << IR_V)) != 0) // geen detectie & in bereik
                {
                    PORTH &= ~(1 << LED_TEST); //uit
                    PORTH &= ~(1 << LED_TEST2); //uit
                    Rechtdoor();
                    MOTORL(snelheidrechtdoor);
                    MOTORR(snelheidrechtdoor);
                }
                if (((PINF & (1 << IR_R)) && (PINF & (1<< IR_L))) != 0) // Voorbij beide balken
                {
                    MOTORL(40);
                    MOTORR(40);
                    toestand = BOCHT_1;
                }


                if ((1<<IR_BOOM_rechts) != 0) // boomgedetecteerd
                  {
                    HBRUG_UIT();
                    toestand = BOOMSTOP;
                  }
          
                if ((1<<IR_BOOM_links) != 0)
                 {
                    HBRUG_UIT();
                    toestand = BOOMSTOP;
                  }
                break;

            case BOOMSTOP:
              if ((1<<IR_BOOM_rechts) != 0)
              {
                PORTL |= (1 << LED_R_GEEL);
                _delay_ms(500);
                PORTL &= ~(1<<LED_R_GEEL);
                _delay_ms(500);
                PORTL |= (1 << LED_R_GEEL);
                _delay_ms(500);
                Rechtdoor();
                MOTORL(snelheidrechtdoor);
                MOTORR(snelheidrechtdoor);
                toestand = BOOMRESET;
              }
              else if ((1<<IR_BOOM_links) != 0)
              {
                PORTL |= (1 << LED_R_GEEL);
                _delay_ms(500);
                PORTL &= ~(1<<LED_R_GEEL);
                _delay_ms(500);
                PORTL |= (1 << LED_R_GEEL);
                _delay_ms(500);
                Rechtdoor();
                MOTORL(snelheidrechtdoor);
                MOTORR(snelheidrechtdoor);
                toestand = BOOMRESET;
              } 
              
                


                break;
            case BOCHT_1:
//                for (int i = 0; i < 100; i++) {
                   _delay_ms(1000);
                   MOTORL(99);
                   MOTORR(40);
                   _delay_ms(700);//
                    MOTORL(snelheidrechtdoor);
                    MOTORR(snelheidrechtdoor);
                   _delay_ms(1000); // stukje rechtdoor
                   MOTORL(99);
                   MOTORR(40);
                   _delay_ms(800);//
                   MOTORL(snelheidrechtdoor);
                   MOTORR(snelheidrechtdoor);
                   _delay_ms(1000);
                    toestand = RIJDEN;
//                }
                break;
            case BOCHT_2:
                _delay_ms(1000);
                   MOTORL(40);
                   MOTORR(99);
                   _delay_ms(700);//
                    MOTORL(snelheidrechtdoor);
                    MOTORR(snelheidrechtdoor);
                   _delay_ms(1000); // stukje rechtdoor
                   MOTORL(40);
                   MOTORR(99);
                   _delay_ms(800);//
                   MOTORL(snelheidrechtdoor);
                   MOTORR(snelheidrechtdoor);
                   _delay_ms(1000);
                    toestand = AUTOLR;
                break;

            case RIJDEN:
               if ((PINF & (1 << IR_R)) == 0) // muur dichtbij
                {
                    MOTORR(snelheidrechtdoor);
                }
                if ((PINF & (1 << IR_R)) != 0) // muur weg dus bijsturen andere kant
                {
                    MOTORR(snelheidbijsturen);
                }

                if ((PINF & (1 << IR_L)) == 0) // muur dichtbij
                {
                    MOTORL(snelheidrechtdoor);
                }
                if ((PINF & (1 << IR_L)) != 0) // muur weg dus bijsturen andere kant
                {
                    MOTORL(snelheidbijsturen);
                }

                if ((PINF & (1 << IR_V)) == 0) // Detectie & te ver
                {
                    PORTH |= (1 << LED_TEST); //aan
                    PORTH |= (1 << LED_TEST2); //aan
                    HBRUG_UIT();
                }
                 if ((PINF & (1 << IR_V)) != 0) // geen detectie & in bereik
                {
                    PORTH &= ~(1 << LED_TEST); //uit
                    PORTH &= ~(1 << LED_TEST2); //uit
                    Rechtdoor();
                    MOTORL(snelheidrechtdoor);
                    MOTORR(snelheidrechtdoor);
                }
                if (((PINF & (1 << IR_R)) && (PINF & (1<< IR_L))) != 0) // Voorbij beide balken
                {
                    HBRUG_UIT();
                    MOTORR(99);
                    MOTORL(99);
                    toestand = BOCHT_2;
                }
                break;

            case VOLG:
                if (((PINF & (1 << IR_V)) != 0) && (agv_ultrasoon_voor_midden < 24)) // geen detectie & in bereik
                {
                    PORTH &= ~(1 << LED_TEST); //uit
                    PORTH &= ~(1 << LED_TEST2); //uit
                    Rechtdoor();
                    MOTORL(snelheidrechtdoor);
                    MOTORR(snelheidrechtdoor);
                }

//                if (((PINF & (1 << IR_V)) == 0) && (agv_ultrasoon_voor_midden > 20)) // Detectie & te ver
                else
                {
                    PORTH |= (1 << LED_TEST); //aan
                    PORTH |= (1 << LED_TEST2); //aan
                    HBRUG_UIT();
                }

                if ((PINF & (1 << IR_R)) == 0) // muur R dichtbij
                {
                    MOTORR(snelheidrechtdoor);
                }
                if ((PINF & (1 << IR_R)) != 0) // muur weg dus bijsturen andere kant
                {
                    MOTORR(snelheidbijsturen);
                }

                if ((PINF & (1 << IR_L)) == 0) // muur L dichtbij
                {
                    MOTORL(snelheidrechtdoor);
                }
                if ((PINF & (1 << IR_L)) != 0) // muur weg dus bijsturen andere kant
                {
                    MOTORL(snelheidbijsturen);
                }
                break;

            case BOOMRESET:
                    if(((1<< IR_BOOM_rechts) == 0) && ((1<<IR_BOOM_links) == 0))
                    {
                    toestand = AUTOLR;
                    }
                if ((PINF & (1 << IR_V)) == 0) // Detectie & te ver
                {
                    PORTH |= (1 << LED_TEST); //aan
                    PORTH |= (1 << LED_TEST2); //aan
                    HBRUG_UIT();
                }
                 if ((PINF & (1 << IR_V)) != 0) // geen detectie & in bereik
                {
                    PORTH &= ~(1 << LED_TEST); //uit
                    PORTH &= ~(1 << LED_TEST2); //uit
                    Rechtdoor();
                    MOTORL(snelheidrechtdoor);
                    MOTORR(snelheidrechtdoor);
                }
              
                break;
        }
    }

    return 0;
}
