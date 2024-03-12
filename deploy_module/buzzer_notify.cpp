#include "buzzer_notify.h"

BuzzerNotify::BuzzerNotify(int pin) : pin_number(pin){};

void BuzzerNotify::Setup()
{
    pinMode(pin_number, OUTPUT);
}
void BuzzerNotify::Check()
{
    curr_cycles++;
    #if DEBUG_BUZZ
        Serial.print("Cycles: ");
        Serial.print(curr_cycles);
        Serial.print("/");
        Serial.println(MAX_CYCLES);
    #endif
        if (curr_cycles > MAX_CYCLES && !beeping){
        #if !NO_BUZZ
            digitalWrite(pin_number, HIGH);
        #endif

            beeping = true;

        #if DEBUG_BUZZ
            Serial.println("Buzz");
        #endif

        }
        if (curr_cycles > MAX_CYCLES_ON)
        {
        #if !NO_BUZZ
            digitalWrite(pin_number, LOW);
        #endif
            beeping = false;
            Reset();
        }
  };
void BuzzerNotify::Trigger()
{
    if (!beeping)
    {
    #if !NO_BUZZ
        digitalWrite(pin_number, HIGH);
    #endif
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
    #if !NO_BUZZ
        digitalWrite(pin_number, LOW);
    #endif
    }
}
void BuzzerNotify::Reset()
{
    curr_cycles = 0;
}