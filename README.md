# Praktikum Eingebettete Systeme – STM32F429

Übungen und Projekte aus dem **Praktikum Eingebettete Systeme** an der THM (WiSe 2025/26), umgesetzt in C auf dem Board **STM32F429I-DISC1** mit der STM32 HAL und CMSIS.

## Projekte

| Ordner | Inhalt |
|---|---|
| `00_Introduction` | Einstieg, Ausgabe auf dem LCD |
| `01_ESD` | Ansteuerung der 7-Segment-Anzeige (ESD) |
| `02_Joystick` | Steuerung der 7-Segment-Anzeige per Joystick |
| `03_LCD` | Zeichenfunktionen für das LCD |
| `04_Potis` | Potentiometerwerte (ADC) als Balkendiagramm auf dem LCD |
| `05_Potis_DMA` | Wie `04_Potis`, aber mit DMA |
| `06_Blinky_Dot` | Blinkender Punkt mit Timer |
| `07_Dimming_Dot` | LED-Helligkeit und PWM-Frequenz steuern |
| `08_Stopwatch` | Stoppuhr mit TIM1-basiertem Tick-Zähler |
| `P1_Fan_Control` | Lüfterregelung mit PI-Regler, PWM und LCD-Ausgabe |
| `P2_Weatherstation` | Vorlage für das Wetterstations-Projekt |
| `PES_Template` | Projektvorlage |

## Module (`modules/`)

Wiederverwendbare Treiber, die von den Projekten gemeinsam genutzt werden: `esd` (7-Segment-Anzeige), `joystick`, `potis` (ADC/DMA), `dot`, `stopwatch`, `fan` (PI-geregelter Lüfter), `my_lcd`, `lcd` (ILI9341), `median` und `utils`.

## Bauen

Jedes Projekt ist ein Eclipse-/System-Workbench-Projekt (`.project`/`.cproject`). Es lässt sich in **STM32CubeIDE** importieren, übersetzen und auf das Board flashen. `CMSIS/` und `HAL_Driver/` stammen von STMicroelectronics.

## Autorin

Gaby Fotio – Technische Hochschule Mittelhessen (THM)
