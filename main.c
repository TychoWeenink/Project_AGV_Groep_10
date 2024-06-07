#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Define.h"

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
              RESET} AGV;

// ISR voor PWM motor L
ISR(TIMER3_OVF_vect)
{
    PORTH ^= (1 << ENB);
}

ISR(TIMER3_COMPA_vect)
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
        OCR3A = (65536 * percentage) / 100;
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
    TCCR3A = (1 << WGM31);  // Fast PWM, TOP=ICR3
    TCCR3B = (1 << WGM33) | (1 << WGM32) | (1 << CS30);  // Prescaler = 1
    ICR3 = 65535;  // Set TOP value for 16-bit timer
    TIMSK3 = (1 << OCIE3A) | (1 << TOIE3);
    OCR3A = 0;

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
    DDRH |= (1 << ENA) | (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4) | (1 << ENB);

    // Zet uitvoer pinnen uit
    PORTH &= ~((1 << LED_TEST) | (1 << LED_TEST2));

    // H-brug pinnen LOW
    PORTH &= ~((1 << ENA) | (1 << IN1) | (1 << IN2) | (1 << IN3) | (1 << IN4) | (1 << ENB));



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
            PORTH &= ~(1 << LED_TEST);
            PORTH &= ~(1 << LED_TEST2);
            toestand = NOODSTOP;
        }

        switch (toestand)
        {
            case NOODSTOP:
                // Voeg hier je code voor NOODSTOP toe
                if (VOLGAAN = 1)
                {
                    if((PINK & (1 << START_knop)) !=0)
                    {
                        if((PINK & (1 << START_knop)) ==0)
                        {
                            Rechtdoor();
                            MOTORL(snelheidrechtdoor);
                            MOTORR(snelheidrechtdoor);
                            toestand = VOLG;
                        }
                    }
                }
                else
                {

                if (baan == 0)
                {
                    if((PINK & (1 << START_knop)) !=0)
                    {
                        if((PINK & (1 << START_knop)) ==0)
                        {
                            Rechtdoor();
                            MOTORL(snelheidrechtdoor);
                            MOTORR(snelheidrechtdoor);
                            toestand = AUTOLR;
                        }
                    }
                }

                if (baan == 1)
                {
                    if((PINK & (1 << START_knop)) !=0)
                    {
                        if((PINK & (1 << START_knop)) ==0)
                        {
                            Rechtdoor();
                            MOTORL(snelheidrechtdoor);
                            MOTORR(snelheidrechtdoor);
                            toestand = RIJDEN;               //!!!!!!!!!!!!!
                        }
                    }
                }

                else
                {
                    if((PINK & (1 << START_knop)) !=0)
                    {
                        if((PINK & (1 << START_knop)) ==0)
                        {
                            Rechtdoor();
                            MOTORL(snelheidrechtdoor);
                            MOTORR(snelheidrechtdoor);
                            toestand = AUTOLR;               //!!!!!!!!!!!!!
                        }
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

                if (((PINF & (1 << IR_R)) && (PINF & (1<< IR_L))) != 0) // Voorbij beide balken
                {
                    MOTORL(99);
                    MOTORR(30);
                    toestand = BOCHT_1;
                }

                if (BOOM = 1) // boomgedetecteerd
                {
                    HBRUG_UIT();
                    toestand = BOOMSTOP;
                }

                break;

            case BOOMSTOP:

                break;

            case BOCHT_1: // van balken af
                if(baan == 0) // bocht links
                {
                    if((PINK & (1 << LM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << LM_switch)) ==0)
                        {
                            LOM++;
                        }
                    }
                    if((PINK & (1 << RM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << RM_switch)) ==0)
                        {
                            ROM++;
                        }
                    }
                    if((LOM && ROM) <= 6)
                    {
                        ROM = 0;
                        LOM = 0;
                        MOTORL(60);
                        MOTORR(30);
                        toestand = BOCHT_2;
                    }
                }

                else // bocht rechts
                {

                }

                break;

            case BOCHT_2: //90 graden
                if(baan == 0) // bocht links
                {
                    if((PINK & (1 << LM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << LM_switch)) ==0)
                        {
                            LOM++;
                        }
                    }
                    if((PINK & (1 << RM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << RM_switch)) ==0)
                        {
                            ROM++;
                        }
                    }
                    if((LOM <= 1) && (ROM <= 9))
                    {
                        ROM = 0;
                        LOM = 0;
                        MOTORL(60);
                        MOTORR(60);
                        toestand = BOCHT_2;
                    }
                }

                else // bocht rechts
                {

                }

                break;

            case BOCHT_3: //stukje door
                if(baan == 0) // bocht links
                {
                    if((PINK & (1 << LM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << LM_switch)) ==0)
                        {
                            LOM++;
                        }
                    }
                    if((PINK & (1 << RM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << RM_switch)) ==0)
                        {
                            ROM++;
                        }
                    }
                    if((LOM && ROM) <= 3)
                    {
                        ROM = 0;
                        LOM = 0;
                        MOTORL(60);
                        MOTORR(30);
                        toestand = BOCHT_2;
                    }
                }

                else // bocht rechts
                {

                }

                break;

            case BOCHT_4: //90 graden
                if(baan == 0) // bocht links
                {
                    if((PINK & (1 << LM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << LM_switch)) ==0)
                        {
                            LOM++;
                        }
                    }
                    if((PINK & (1 << RM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << RM_switch)) ==0)
                        {
                            ROM++;
                        }
                    }
                    if((LOM <= 1) && (ROM <= 9))
                    {
                        ROM = 0;
                        LOM = 0;
                        MOTORL(40);
                        MOTORR(40);
                        toestand = BOCHT_2;
                    }
                }

                else // bocht rechts
                {

                }

                break;

            case BOCHT_5: //stukje tot balk
                if((baan) == 0) // bocht links
                {
                    if((PINK & (1 << LM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << LM_switch)) ==0)
                        {
                            LOM++;
                        }
                    }
                    if((PINK & (1 << RM_switch)) != 0) // als knop ingedrukt optellen
                    {
                        DEBOUNCE;
                        if((PINK &(1 << RM_switch)) ==0)
                        {
                            ROM++;
                        }
                    }
                    if((LOM && ROM) <= 6)
                    {
                        ROM = 0;
                        LOM = 0;
                        MOTORL(snelheidrechtdoor);
                        MOTORR(snelheidrechtdoor);
                        toestand = RIJDEN;
                    }
                }

                else // bocht rechts
                {

                }
                break;


            case RIJDEN:
                // code recht door rijden en stoppen wanneer object voor de agv

                break;

            case VOLG:
                if (((PINF & (1 << IR_V)) == 0) && (ULTRA_V_afstand > 20)) // Detectie & te ver
                {
                    PORTH |= (1 << LED_TEST);
                    PORTH |= (1 << LED_TEST2);
                    HBRUG_UIT();

                }
                if (((PINF & (1 << IR_V)) != 0) && (ULTRA_V_afstand <= 20)) // geen detectie & in bereik
                {
                    PORTH &= ~(1 << LED_TEST);
                    PORTH &= ~(1 << LED_TEST2);
                    Rechtdoor();
                    MOTORL(snelheidrechtdoor);
                    MOTORR(snelheidrechtdoor);
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
        }
    }

    return 0;
}
