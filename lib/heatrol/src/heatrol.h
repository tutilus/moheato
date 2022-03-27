/**
 * 
 * Heatrol.hpp - Pilot wire heating controller library for Arduino & Wiring
 * Copyright (c) 2006 Tutilus.  All right reserved.
 * @created_at: 26-11-2018
 */

#ifndef HEATROL_H
#define HEATROL_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#define ORDER_NB 4

typedef enum { HOURS, MINUTES, SECONDS } hzPeriod_t;
typedef enum { CONFORT, HORSGEL, ARRET, ECO } hzOrder_t; 
typedef enum { PIN1, PIN2 } hzPin_t;
class Heatrol
{
private:
    hzOrder_t order;
    hzOrder_t defaultOrder;
    uint8_t pins[2];
    uint8_t states[2];
    String name;  

    // Callback functions
    static void (*onchange_cb)(Heatrol*);

    void setState(uint8_t, uint8_t);

    String orderLabelFor(hzOrder_t);

public:

    Heatrol();

    /**
     * Initialize the heater with 2 excepted pins to control heater (high and low sinusoidal), 
     * default mode (default=HORS GEL) and,
     * label (default=ROOM)
     */
    void begin(uint8_t pin1, uint8_t pin2, hzOrder_t default_mode, String label);    
    void begin(uint8_t pin1, uint8_t pin2, hzOrder_t default_mode);
    void begin(uint8_t pin1, uint8_t pin2, String label);
    void begin(uint8_t pin1, uint8_t pin2);

    // Setters
    void setName(String name);
    void setDefaultOrder(hzOrder_t order);

    // Getters
    String getName();
    hzOrder_t getOrder();
    String getOrderLabel();
    hzOrder_t getDefaultOrder();
    String getDefaultOrderLabel();

    // Event functions
    void onChange(void (*)(Heatrol*));
    
    void setOrder(hzOrder_t);    

    // Scheduler task functions
    void toDefault(void); // Change heating to default mode

    bool isDefaultOrder(void);
    
};

#endif // HEATROL_H