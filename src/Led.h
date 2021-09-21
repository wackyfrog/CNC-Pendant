//
// Created by Oleksandr Degtiar on 03.03.2021.
//

#ifndef CNC_PENDANT_LED_H
#define CNC_PENDANT_LED_H


class Led {
public:
    typedef enum {
        Off,
        Custom,
        On,
        FastBlink,
        Blink1,
        Blink2
    } Mode;

    typedef void (*Callback)(void *);

    Led(const int pin);

    void on(uint32_t timeout);

    void on();

    void off();

    void tick();

    bool isLighted() const;

    Mode getMode() const;

    void setMode(Mode newMode, Callback newUpdater = nullptr);

    bool isOff();

    uint32_t getNextUpdateTime() const;

    uint8_t getState() const;

    void setState(uint8_t newState);

    void setLighten(bool light);

    void toggleLight();

    void setNextUpdateTime(uint32_t time);

    bool isDynamic();

    void update();

    uint32_t getLastUpdateTime() const;

private:
    const int pin;
    bool lighten = false;
    Mode mode = Off;
    uint32_t nextUpdateTime = 0;
    uint32_t lastUpdateTime = 0;
    uint32_t offAtTime = 0;
    uint8_t state = 0;
    Callback updater = nullptr;

    virtual void out();

protected:

    bool blinking;
};

#endif //CNC_PENDANT_LED_H
