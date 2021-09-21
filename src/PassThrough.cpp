#include "PassThrough.h"
#include "GCodeSerial.h"

void PassThrough::add(char c) {
    if (dataLength < PASSTHROUGH_BUFFER_SIZE) {
        rxBuffer[dataLength++] = c;
    } else {
        overflowed = true;
    }
}

bool PassThrough::processOutgoing() {
    uint16_t length = getDataLength();

    if (length > 0 && output.isTxBufferEmpty()) {
        char *buffer = rxBuffer;
        // removing leading spaces
        while (*buffer == ' ' && length > 0) {
            ++buffer;
            length--;
        }
        output.write(buffer, length);
        output.println();
        resetState();
        return true;

    } else {
        return false;
    }
}

bool PassThrough::processIncoming() {
    char c = '\0';
    static uint8_t actualChecksum = 0;
    static uint8_t receivedChecksum = 0;
    bool readNextChar = true;

    do {
        if (readNextChar) {
            c = input.read();
            if (c == -1) {
                break;
            }
        }
        readNextChar = true;

        switch (state) {
            case State::waitingForStart:
                if (c == 'N') {
                    actualChecksum = c;
                    state = State::receivingLineNumber;
                }
                break;

            case State::receivingLineNumber:
                if (c >= '0' && c <= '9') {
                    actualChecksum ^= c;
                } else {
                    readNextChar = false;
                    state = State::receivingCommand;
                }
                break;

            case State::receivingCommand:
                if (c == '*') {
                    state = State::receivingChecksum;
                    receivedChecksum = 0;
                    break;
                }
                if (c == '\n' || c == '\r') {
                    resetState();
                    break;
                }
                if (c == '"') {
                    state = State::receivingQuotedString;
                }
                add(c);
                actualChecksum ^= c;
                break;

            case State::receivingQuotedString:
                if (c == '\n' || c == '\r') {
                    resetState();
                    break;
                }
                if (c == '"') {
                    state = State::receivingCommand;
                }
                add(c);
                actualChecksum ^= c;
                break;

            case State::receivingChecksum:
                if (c == '\n' || c == '\r') {
                    if (receivedChecksum == actualChecksum) {
                        if (dataLength > 0) {
                            state = State::haveCommand;
                        } else {
                            resetState();
                        }

                    } else {
//                        output.println("CRC err");
                        resetState();
                    }

                } else if (c >= '0' && c <= '9') {
                    receivedChecksum = (10 * receivedChecksum) + (c - '0');
                } else {
                    resetState();
                }
                break;

            default:
                break;
        }
    } while (readNextChar || state != State::haveCommand);
    return state == State::haveCommand;
}

const bool PassThrough::isOverflowed() {
    return overflowed;
}

void PassThrough::resetState() {
    overflowed = false;
    dataLength = 0;
    state = State::waitingForStart;
}

void PassThrough::discard() {
    processIncoming();
    resetState();
}

uint16_t PassThrough::getDataLength() const {
    return state == State::haveCommand ? dataLength : 0;
}

bool PassThrough::isAvailableForSend() const {
    return state == State::haveCommand;
}


PassThrough::PassThrough(HardwareSerial &inSerial, GCodeSerial &outSerial) : input(inSerial), output(outSerial) {
    resetState();
}

// End
