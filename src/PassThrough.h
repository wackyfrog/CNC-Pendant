#ifndef PASSTHROUGH_H_INCLUDED
#define PASSTHROUGH_H_INCLUDED

#define PASSTHROUGH_BUFFER_SIZE 256

#include "Arduino.h"
#include "GCodeSerial.h"

// Class to accumulate commands received from PanelDue to be passed through
class PassThrough {
public:
    PassThrough(HardwareSerial &inSerial, GCodeSerial &outSerial);

    bool processIncoming();

    bool processOutgoing();

    void resetState();

    void discard();

    const bool isOverflowed();

    bool isAvailableForSend() const;

    uint16_t getDataLength() const;

private:
    HardwareSerial &input;

    GCodeSerial &output;

    enum class State : uint8_t {
        waitingForStart = 0,
        receivingLineNumber,
        receivingCommand,
        receivingQuotedString,
        receivingChecksum,
        haveCommand
    };

    uint16_t dataLength = 0;
    State state = State::waitingForStart;
    bool overflowed = false;
    char rxBuffer[PASSTHROUGH_BUFFER_SIZE];


    /**
     * Store character and add to checksum. If no room, set the overflow flag.
     * @param c
     */
    void add(char c);


};

#endif
