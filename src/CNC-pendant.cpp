// CNC pendant interface to Duet
// D Crocker, started 2020-05-04

// Oleksandr Degtiar <adegtiar@gmail.com>

/* This Arduino sketch can be run on either Arduino Nano or Arduino Pro Micro. 
 * It should alo work on an Arduino Uno (using the same wiring scheme as for the Nano) or Arduino Leonardo (using the same wiring scheme as for the Pro Micro).
 * The recommended board is the Arduino Pro Micro because the passthrough works without any modificatoins to the Arduino. 

*** Pendant to Arduino Pro Micro connections ***

Pro Micro Pendant   Wire colours
VCC       +5V       red
GND       0V,       black
          COM,      orange/black
          CN,       blue/black
          LED-      white/black

D2        A         green
D3        B         white
D4        X         yellow
D5        Y         yellow/black
D6        Z         brown
D7        4         brown/black
D8        5         powder (if present)
D9        6         powder/black (if present)
D10       LED+      green/black
A0        STOP      blue
A1        X1        grey
A2        X10       grey/black
A3        X100      orange

NC        /A,       violet
          /B        violet/black

Arduino Nano to Duet PanelDue connector connections:

Pro Micro Duet
VCC       +5V
GND       GND
TX1/D0    Through 6K8 resistor to URXD, also connect 10K resistor between URXD and GND

To connect a PanelDue as well:

PanelDue +5V to +5V/VCC
PanelDue GND to GND
PanelDue DIN to Duet UTXD or IO_0_OUT
PanelDue DOUT to /Pro Micro RX1/D0.

*** Pendant to Arduino Nano connections ***

Nano    Pendant   Wire colours
+5V     +5V       red
GND     0V,       black
        COM,      orange/black
        CN,       blue/black
        LED-      white/black

D2      A         green
D3      B         white
D4      X         yellow
D5      Y         yellow/black
D6      Z         brown
D7      4         brown/black
D8      5         powder (if present)
D9      6         powder/black (if present)
D10     X1        grey
D11     X10       grey/black
D12     X100      orange
D13     LED+      green/black
A0      STOP      blue

NC      /A,       violet
        /B        violet/black

Arduino Nano to Duet PanelDue connector connections:

Nano    Duet
+5V     +5V
GND     GND
TX1/D0  Through 6K8 resistor to URXD, also connect 10K resistor between URXD and GND

To connect a PanelDue as well:

PanelDue +5V to +5V
PanelDue GND to GND
PanelDue DIN to Duet UTXD or IO_0_OUT
PanelDue DOUT to Nano/Pro Micro RX1/D0.

On the Arduino Nano is necessary to replace the 1K resistor between the USB interface chip by a 10K resistor so that PanelDiue can override the USB chip.
On Arduino Nano clones with CH340G chip, it is also necessary to remove the RxD LED or its series resistor.

*/
#if defined(__AVR_ATmega328P__)     // Arduino Nano or Uno
#endif

#include <Arduino.h>
#include "Config.h"
#include "Pendant.h"
#include "GCodeSerial.h"
#include "PassThrough.h"
#include "Led.h"
#include "LedRGB.h"

typedef enum {
    INIT,
    IDLE,
    IDLE_MOVEMENT,
    MOVEMENT,
    MOVEMENT_WCS,
    ACKNOWLEDGE,
    ACKNOWLEDGE_CONFIRM,
    ACKNOWLEDGE_CANCEL,
    TIMEOUT_AFTER_COMMAND
} State;

LedRGB stateLed(PIN_ACTIVITY_LED);

LedRGB serialActivityLed(PIN_ACTIVITY_LED);

Led buttonLed(PIN_BUTTON_LED);

State state = INIT;

Pendant pendant;

GCodeSerial output(UartSerial);

PassThrough passThrough(UartSerial, output);


int axisWhenButtonPressed = Pendant::Axis::OFF;

int acknowelegementResult = 0;

int previousAxis = Pendant::Axis::OFF;
int previousXSwitch = Pendant::XSwitch::UNDEFINED;

uint32_t stateLastChangeTime = 0;

void startupBlink() {
    buttonLed.on();
    delay(500);
    buttonLed.off();

    stateLed.setColorHSV(HUE_RED, SATURATION_MAX, LED_MAX_VAL);
    stateLed.on();
    delay(200);

    stateLed.setColorHSV(HUE_GREEN, SATURATION_MAX, LED_MAX_VAL);
    stateLed.on();
    delay(200);

    stateLed.setColorHSV(HUE_BLUE, SATURATION_MAX, LED_MAX_VAL);
    stateLed.on();
    delay(200);
    stateLed.off();
}

void setState(State newState) {
    if (newState == state) {
        return;
    }

    switch (newState) {
        case IDLE:
//            Serial1.println(F("[MODE][IDLE]"));
            buttonLed.setMode(Led::On);
            stateLed.off();
            break;

        case IDLE_MOVEMENT:
//            Serial1.println(F("[MODE][IDLE_MOVEMENT]"));
            buttonLed.setMode(Led::Blink1);
            stateLed.off();
            break;

        case MOVEMENT:
//            Serial1.println(F("[MODE][MOVEMENT]"));
            buttonLed.setMode(Led::On);
            previousAxis = Pendant::Axis::OFF;
            break;

        case MOVEMENT_WCS:
//            Serial1.println(F("[MODE][MOVEMENT_WCS]"));
            buttonLed.setMode(Led::On);
            break;

        case ACKNOWLEDGE:
            stateLed.setColorWhite(LED_MAX_VAL);
            buttonLed.on();
            stateLed.on();
            acknowelegementResult = 0;
            break;

        case ACKNOWLEDGE_CONFIRM:
            stateLed.setColorHSV(HUE_GREEN, SATURATION_MAX, LED_MAX_VAL);
            stateLed.setMode(Led::FastBlink);
            output.print(CommandConfirm);
            output.println();
            break;

        case ACKNOWLEDGE_CANCEL:
            stateLed.setColorHSV(HUE_RED, SATURATION_MAX, LED_MAX_VAL);
            stateLed.setMode(Led::FastBlink);
            output.print(CommandCancel);
            output.println();
            break;

        case TIMEOUT_AFTER_COMMAND:
            break;

        default:
            break;
    }

    state = newState;
    stateLastChangeTime = millis();
}

bool onButtonPress() {
    axisWhenButtonPressed = pendant.getAxis();
    return true;
}

bool onButtonRelease(const uint32_t time) {
//    output.println(F("onButtonRelease"));
    switch (state) {
        case IDLE:
            if (axisWhenButtonPressed == Pendant::Axis::OFF
                /*&& pendant.getXSwitch() == Pendant::XSwitch::X100*/
                && pendant.getAxis() != Pendant::Axis::OFF) {
                output.print(WCSResetCommands[pendant.getAxis() - 1]);
                output.println();

                stateLed.setColorHSV(HUE_AZURE, SATURATION_MAX, LED_MAX_VAL);
                stateLed.setMode(Led::FastBlink);
                setState(TIMEOUT_AFTER_COMMAND);
                break;
            }

            if (pendant.getAxis() == Pendant::Axis::OFF && time < BUTTON_SHORT_PRESS_MAX_TIME) {
                setState(ACKNOWLEDGE);
            }
            break;

        case IDLE_MOVEMENT:
            if (axisWhenButtonPressed != pendant.getAxis()/* || pendant.getXSwitch() != Pendant::XSwitch::X100*/) {
                break;
            }
            setState(MOVEMENT);
            break;

        case MOVEMENT:
            setState(IDLE_MOVEMENT);
            break;

        case ACKNOWLEDGE:
            if (time < BUTTON_SHORT_PRESS_MAX_TIME) {
                setState(IDLE);
            }
            break;

        default:
            break;
    }
    return true;
}

bool processMovement() {
    static uint32_t whenLastCommandSent = 0;
    const uint32_t now = millis();

    if (pendant.getAxis() == Pendant::Axis::OFF || pendant.getFeedFactor() == 0) {
        pendant.readEncoderValue();
        return false;
    }

    if (now < whenLastCommandSent + MOVEMENT_SEND_INTERVAL) {
        return false;
    }

    if (!output.isTxBufferEmpty()) {
        return false;
    }

    int encoderValue = pendant.readEncoderValue();
    if (encoderValue == 0) {
        return false;
    }

    whenLastCommandSent = now;
    output.write(MoveCommands[pendant.getAxis() - 1]);
    output.print((float) encoderValue / (float) pendant.getFeedFactor(), 2);
    output.println();
    return true;
}


void processAcknowledgement() {
    const int ackThreshold = 30;
    int value = pendant.readEncoderValue();
    if (value == 0) {
        return;
    }


    acknowelegementResult += value;
    if (acknowelegementResult > ackThreshold) {
        setState(ACKNOWLEDGE_CONFIRM);
        return;

    } else if (acknowelegementResult < -ackThreshold) {
        setState(ACKNOWLEDGE_CANCEL);
        return;
    }

    int hue = HUE_YELLOW + acknowelegementResult * 2;
    if (hue > HUE_GREEN) {
        hue = HUE_GREEN;
    } else if (hue < HUE_RED) {
        hue = HUE_RED;
    } else if (stateLed.getMode() != Led::On) {
        stateLed.on();
    }

    stateLed.setColorHSV(hue, min(abs(acknowelegementResult) * 8, 100), LED_MAX_VAL);
    stateLed.update();
    stateLastChangeTime = millis();
}

void setup() {
    startupBlink();
#if defined(__AVR_ATmega32U4__)     // Arduino Leonardo or Pro Micro
    TX_RX_LED_INIT;
    TXLED0;
    RXLED0;
#endif

    output.begin(BAUD_RATE);
    output.println(F("echo ****** " VERSION " has been started ***** "));

    serialActivityLed.setColorWhite(LED_MAX_VAL);
    output.setActivityCallback([](void) {
        if (serialActivityLed.isOff() && millis() - serialActivityLed.getLastUpdateTime() > 50) {
            serialActivityLed.on(50);
        }
    });

    pendant.onButtonPress = onButtonPress;
    pendant.onButtonRelease = onButtonRelease;
}

void onButtonPressing() {
    switch (state) {
        case MOVEMENT:
            if (pendant.readEncoderValue() != 0
                || pendant.getAxis() != previousAxis
                || pendant.getXSwitch() != previousXSwitch) {
                setState(IDLE);
                return;
            }
            if (pendant.getButtonPressingTime() < 2000) {
                break;
            }

            output.print(WCSSetCommands[axisWhenButtonPressed - 1]);
            output.println();

            stateLed.setColorHSV(axisColor[pendant.getAxis() - 1], SATURATION_MAX, LED_MAX_VAL);
            stateLed.setMode(Led::FastBlink);
            setState(TIMEOUT_AFTER_COMMAND);
            break;

        default:
            break;
    }
}

void loop() {
    pendant.poll();

    buttonLed.tick();
    if (serialActivityLed.isOff() || stateLed.isDynamic() || state == ACKNOWLEDGE) {
        stateLed.tick();

    } else {
        serialActivityLed.tick();
    }

    uint32_t now = millis();
    if (pendant.isButtonPressed()) {
        onButtonPressing();
        pendant.readEncoderValue();
        passThrough.discard();
        return;
    }

    if (pendant.getAxis() != previousAxis && pendant.getAxis() == Pendant::Axis::OFF) {
        setState(IDLE);
    }

    switch (state) {
        case INIT:
            setState(IDLE);
            return;

        case IDLE:
            if (pendant.getAxis() != Pendant::Axis::OFF) {
                setState(IDLE_MOVEMENT);
            }
            pendant.readEncoderValue();
            break;

        case IDLE_MOVEMENT:
            pendant.readEncoderValue();
            break;

        case MOVEMENT:
            if (now - stateLastChangeTime > MOVEMENT_IDLE_TIME) {
                setState(IDLE_MOVEMENT);
                break;
            }
            if ((pendant.getAxis() != previousAxis && pendant.getAxis() != Pendant::Axis::OFF)
                || (pendant.getXSwitch() != previousXSwitch)) {
                stateLed.setColorHSV(axisColor[pendant.getAxis() - 1], SATURATION_MAX, LED_MAX_VAL);
                stateLed.on();
            }
            if (processMovement()) {
                stateLastChangeTime = now;
            }
            break;

        case MOVEMENT_WCS:
            if (pendant.getAxis() != axisWhenButtonPressed) {
                setState(IDLE_MOVEMENT);
            }
            break;

        case ACKNOWLEDGE:
            if (now - stateLastChangeTime > ACKNOWLEDGE_IDLE_TIME || pendant.getAxis() != Pendant::Axis::OFF) {
                setState(IDLE);
                break;
            }

            processAcknowledgement();
            break;

        case ACKNOWLEDGE_CONFIRM:
            // fallthrough
        case ACKNOWLEDGE_CANCEL:
            // fallthrough
        case TIMEOUT_AFTER_COMMAND:
            buttonLed.off();
            pendant.readEncoderValue();
            if (now - stateLastChangeTime > BUTTON_HOLD_TIME_TO_SET_WCS) {
                setState(IDLE);
            }
            break;

    }

    if (passThrough.processIncoming()) {
        passThrough.processOutgoing();
    }
    previousAxis = pendant.getAxis();
    previousXSwitch = pendant.getXSwitch();
}

// End
