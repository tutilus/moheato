/**
 * 
 * Heatrol.hpp - Pilot wire heating controller library for Arduino & Wiring
 * Copyright (c) 2006 Tutilus.  All right reserved.
 *
 * This program is designed to work with a heating controller
 * which is able to create specific wave from sinusoidal wave
 * based on 2 pins declared in the constructor.
 *   - Pin1 will keep only positive part of the sinusoidal wave
 *   - Pin2 will keep only negative part.
 * @created_at: 26-11-2018
 */

#include "heatrol.h"

// Init static variables

void (*Heatrol::onchange_cb)(Heatrol*);

Heatrol::Heatrol() {}

void Heatrol::begin(uint8_t pin1, uint8_t pin2, hzOrder_t defaultOrder, String name) {
    pins[0] = pin1; // Negative current
    pins[1] = pin2; // Positive current
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
    setName(name);
    setDefaultOrder(defaultOrder);
    toDefault();
}

void Heatrol::begin(uint8_t pin1, uint8_t pin2, hzOrder_t defaultOrder) { begin(pin1, pin2, defaultOrder, "room"); }

void Heatrol::begin(uint8_t pin1, uint8_t pin2, String name) { begin(pin1, pin2, HORSGEL, name); }

void Heatrol::begin(uint8_t pin1, uint8_t pin2) { begin(pin1, pin2, HORSGEL, "room"); }


// SETTER
void Heatrol::setDefaultOrder(hzOrder_t order) { this->defaultOrder = order; }

void Heatrol::setName(String name) { this->name = name; }

void Heatrol::setOrder(hzOrder_t order) {
    static uint8_t WavePlus[] =  { LOW, HIGH, LOW, HIGH };
    static uint8_t WaveMinus[] = { LOW, LOW, HIGH, HIGH };
    
    if (order >= 0 && order < ORDER_NB) {
        this->order = order;
        setState(WavePlus[order], WaveMinus[order]);    
    }
    if (onchange_cb) { onchange_cb(this); }
}

// GETTER
hzOrder_t Heatrol::getDefaultOrder() { return this->defaultOrder; }

String Heatrol::getDefaultOrderLabel() { return orderLabelFor(this->defaultOrder); }

String Heatrol::getName() { return this->name; }

hzOrder_t Heatrol::getOrder() { return this->order; }

String Heatrol::getOrderLabel() { return orderLabelFor(this->order); }


void Heatrol::toDefault(void) {
    setOrder(this->defaultOrder);
}

bool Heatrol::isDefaultOrder(void) {
    return (this->order == this->defaultOrder);
}

void Heatrol::onChange(void (*function)(Heatrol*)) {
    onchange_cb = function;
}

String Heatrol::orderLabelFor(hzOrder_t order) {
    static String Labels[] = {"Confort", "Hors gel", "Arrêt", "Eco" };
    if (order >=0 && order < sizeof(Labels)) {
        return Labels[order];
    }
    return "";

}

void Heatrol::setState(uint8_t val1, uint8_t val2) {
    digitalWrite(pins[0], val1);
    states[0] = val1;
    digitalWrite(pins[1], val2);
    states[1] = val2;    
}