#include "GCodeSerial.h"

void GCodeSerial::begin(unsigned long baud) {
    serial.begin(baud);
    lineNumber = 0;
    emptyLine = true;
    txBufferSize = serial.availableForWrite();
}

size_t GCodeSerial::write(uint8_t c) {
    if (c == '\n') {
        if (!emptyLine) {
            print(' ');
            writeRaw('*');
            print(checksum);
            emptyLine = true;
        }
    } else {
        if (emptyLine) {
            ++lineNumber;
            checksum = 0;
            emptyLine = false;      // do this first to avoid infinite recursion
            write('N');
            print(lineNumber);
            print(' ');
        }
        checksum ^= c;
    }
    onActivity();
    writeRaw(c);
    return 1;
}

int GCodeSerial::availableForWrite() {
    return serial.availableForWrite();
}

void GCodeSerial::writeRaw(uint8_t c) {
    onActivity();
    serial.write(c);
}

size_t GCodeSerial::println(void) {
    onActivity();
    return write("\n");
}

void GCodeSerial::onActivity() {
    if (activityCallback != nullptr) {
        activityCallback();
    }
}

bool GCodeSerial::isTxBufferEmpty() {
    return txBufferSize == availableForWrite();
}

void GCodeSerial::setActivityCallback(ActivityCallback *newCallback) {
    activityCallback = newCallback;
}

// End
