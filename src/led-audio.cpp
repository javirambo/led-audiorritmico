/*
 * Arduino Toys.
 * Audiorritmico con control de ganancia automático.
 * Controla el brillo de leds RGB, el color cambia automático.
 * 
 * Javier Rambaldo - Noviembre 2020
 */

#include <Arduino.h>
#include "HSLtoRGB.cpp"

void setup()
{
    //leo el audio por A0
    pinMode(A0, INPUT);
    // controlo los LEDs RGB por PWM
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(13, 0);
    Serial.begin(9600);
}

float leerAudio()
{
    static float max, min, adc, top, val, percent;

    // leo el audio analogico
    adc = analogRead(A0);

    // guardo los picos o extremos:
    if (adc < min)
        min = adc;
    if (adc > max)
        max = adc;

    // control automático de ganancia:
    top = (max - min);
    val = (adc - min);
    percent = val / top;

    // compuerta de sonido: lo apaga si no hay sonido.
    if (top < 3)
    {
        percent = 0;
        digitalWrite(13, 1);
    }
    else
    {
        digitalWrite(13, 0);
    }

    //controlo saturacion: brillo va de 0 a 1
    if (percent < 0)
        percent = 0;
    if (percent > 1)
        percent = 1;

    // los extremos se van juntando o normalizando suavemente:
    min += 0.001;
    max -= 0.001;

    return percent;
}

void loop()
{
    static float hue = 0;
    float nivel = leerAudio();

    // controlo solo el brillo, el color va cambiando solito...

    HSL data = HSL(hue, 1, nivel);
    RGB value = HSLToRGB(data);
    analogWrite(9, value.R);
    analogWrite(5, value.G);
    analogWrite(6, value.B);

    // cambio el color suavemente
    hue += 0.1;
    if (hue > 360)
        hue = 0;
}
