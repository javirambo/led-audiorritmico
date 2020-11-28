/*
 * Audiorritmico con control de ganancia automático.
 * Controla el brillo de leds RGB, el color cambia automático.
 * 
 * Javier Rambaldo - Julio 2020
 */

#include <Arduino.h>

float hue = 0, max = 0, min = 1024, atenuacion, adc, brillo;
float max1 = 0, min1 = 1024, atenuacion1, adc1, brillo1;

struct RGB
{
    byte R;
    byte G;
    byte B;
} rgb;

float fract(float x) { return x - int(x); }
float mix(float a, float b, float t) { return a + (b - a) * t; }
void HSV2RGB(float h, float s, float b)
{
    // h = hue; s = saturation; b = brightness
    rgb.R = (byte)(255.0 * (s * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b)));
    rgb.G = (byte)(255.0 * (s * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b)));
    rgb.B = (byte)(255.0 * (s * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), b)));
}

void setup()
{
    //leo el audio por A0
    pinMode(A0, INPUT);
    // controlo los LEDs RGB por PWM
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);
    Serial.begin(9600);
}

void trace_values()
{
    //--solo traceo si hay cambios...
    if (min - min1 + max - max1 + brillo - brillo1 + adc - adc1 + atenuacion - atenuacion1)
    {
        min1 = min;
        max1 = max;
        adc1 = adc;
        atenuacion1 = atenuacion;
        brillo1 = brillo;

        Serial.print(adc);
        Serial.print("  ");
        Serial.print(min);
        Serial.print("  ");
        Serial.print(max);
        Serial.print("  ");
        Serial.print(atenuacion);
        Serial.print("  ");
        Serial.print(brillo);
        Serial.print("  ");
        Serial.println();
    }
}

void loop()
{
    // leo el audio analogico
    adc = analogRead(A0);

    // guardo los picos o extremos:
    if (adc < min)
        min = adc;
    if (adc > max)
        max = adc;

    // control automático de ganancia:
    atenuacion = (max / min) * 20.5;

    // le aplico un logaritmo para atenuar los picos máximos
    brillo = log(adc / atenuacion) + 0.5;

    trace_values();

    //controlo saturacion: brillo va de 0 a 1
    if (brillo < 0)
        brillo = 0;
    if (brillo > 1)
        brillo = 1;

    // compuerta de sonido: lo apaga si no hay sonido.
    if (max - min <= 4)
        brillo = 0;

    // controlo solo el brillo, el color va cambiando solito...
    HSV2RGB(hue, brillo, 1.0);

    analogWrite(5, rgb.R);
    analogWrite(6, rgb.G);
    analogWrite(9, rgb.B);

    // cambio el color suavemente
    hue += 0.0001;
    if (hue > 1)
        hue = 0;

    // los extremos se van juntando o normalizando suavemente:
    min += 0.1;
    max -= 0.1;
}
