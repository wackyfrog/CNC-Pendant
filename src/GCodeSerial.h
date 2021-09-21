#ifndef GCODESERIAL_INCLUDED
#define GCODESERIAL_INCLUDED

static const int ledTimeout = 100;

#include "Arduino.h"
#include "Led.h"

// Class to output to serial, adding line numbers and checksums
class GCodeSerial : public Print {
private:
    typedef void(ActivityCallback)();

    ActivityCallback *activityCallback;
    int txBufferSize = 0;
    HardwareSerial &serial;
    uint16_t lineNumber;
    uint8_t checksum;

    bool emptyLine;

    void onActivity();

public:
    GCodeSerial(HardwareSerial &device) : serial(device) {}

    void begin(unsigned long baud);

    size_t write(uint8_t) override;

    void writeRaw(uint8_t c);

    int availableForWrite() override;

    bool isTxBufferEmpty();

    size_t println(void);

    using Print::write;
    using Print::println;

    void setActivityCallback(ActivityCallback *newCallback);

};

#endif
