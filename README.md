# Actividad 2 — Ascensor inteligente ACME S.A.

## Descripción

Continuación de la actividad 1. Se simula la lógica de control completa de un ascensor de cinco plantas en Wokwi, añadiendo control de temperatura con algoritmo ON-OFF con zona muerta y control de iluminación escalonado con LEDs.

**Simulación en Wokwi:** https://wokwi.com/projects/463634493693459457

---

## Componentes utilizados

| Componente | Pin | Función |
|---|---|---|
| DHT22 | 2 | Temperatura y humedad |
| Botón P1 | 3 | Llamada planta 1 |
| Botón P2 | 4 | Llamada planta 2 |
| Botón P3 | 5 | Llamada planta 3 |
| Botón P4 | 6 | Llamada planta 4 |
| Botón P5 | 7 | Llamada planta 5 |
| LED amarillo L3 | 8 | Iluminación nivel 3 |
| Servo SG90 | 9 | Movimiento cabina |
| LED azul (frío) | 10 | Electroválvula enfriamiento |
| LED rojo (calor) | 11 | Electroválvula calentamiento |
| LED amarillo L1 | 12 | Iluminación nivel 1 |
| LED amarillo L2 | 13 | Iluminación nivel 2 |
| LDR | A0 | Sensor de iluminación |
| LCD I2C | A4/A5 | Pantalla HMI |

---

## Funcionamiento

### Ascensor
Cinco botones con INPUT_PULLUP, uno por planta. Al pulsar uno el servo gira gradualmente hasta el ángulo correspondiente a esa planta (P1=0°, P2=45°, P3=90°, P4=135°, P5=180°). El LCD muestra la planta destino durante el movimiento.

### Control de temperatura (ON-OFF con zona muerta)
- Setpoint: 25°C, zona muerta ±3°C
- Temperatura > 28°C → LED azul encendido (enfriando)
- Temperatura entre 22°C y 28°C → ningún LED (zona muerta)
- Temperatura < 22°C → LED rojo encendido (calentando)

### Control de iluminación (escalonado)
El LDR devuelve un valor ADC entre 0 y 1023. Cuanta menos luz, más LEDs encendidos:
- ADC > 700 → 3 LEDs
- ADC > 400 → 2 LEDs
- ADC > 200 → 1 LED
- ADC < 200 → 0 LEDs

### LCD I2C
- Línea 1: planta actual y temperatura
- Línea 2: temperatura anterior vs temperatura actual

---

## Log de datos (puerto serie)

Cada 2 segundos se envía una línea con el formato:

```
Planta,TempAnterior,TempActual,Accion,LDR
```

Ejemplo:
```
Planta,TempAnt,TempAct,Accion,Lux
1,24.0,29.5,ENFRIANDO,620
1,29.5,28.8,ENFRIANDO,620
1,28.8,25.1,OK,580
2,25.1,19.8,CALENTANDO,750
```

---

## Librerías necesarias

- `SimpleDHT` — sensor DHT22
- `LiquidCrystal_I2C` — pantalla LCD con módulo I2C
- `Servo` — control del servomotor

---
