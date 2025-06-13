# LPC2148 Digital Clock with Alarm

This project implements a fully functional digital clock with alarm feature using the LPC2148 ARM7 microcontroller.

## Features

- Real-Time Clock (RTC) with date and time keeping
- LCD 16x2 display interface
- User control with 4 buttons:
  - Set (Edit Mode)
  - Increment
  - Decrement
  - Save
  - Mode (switch between Time Edit and Alarm Edit)
- Alarm functionality with buzzer output
- Date validation and leap year handling
- Proteus simulation compatible

## Hardware

- LPC2148 ARM7 microcontroller
- 16x2 LCD (8-bit mode)
- External 32.768 kHz crystal for RTC
- 12 MHz main crystal for system clock
- 5 push buttons connected to P0.11–P0.15
- Buzzer connected via transistor to P0.16
- Proteus simulation circuit available

## Folders

- `/src` — Source code (`main.c`, `lcd.c`, `lcd.h`, `project.hex`)
- `/iar` — IAR Embedded Workbench project files
- `/proteus` — Proteus simulation files

## Usage

- Open the IAR project to build and load code to hardware or Proteus simulation.
- Alternatively, use the provided `clk.hex` in `src` directly for Proteus simulation.

## License

Free for educational and personal use.
