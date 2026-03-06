# drone

## Summary

Over the 2025 summer break, I decided to take on a project to build a drone (or in the most brutish terms to describe this project a "flying machine" ). This repository tracks all the code for the drone, written entirely from scratch (including drivers). No external libraries or HAL are used except the startup file and the linker file.

In short, this is a bare-metal quadcopter/drone project built entirely from the ground up.

![Schematic](Schematic.png)

![Drone Base Image](Base_Image.JPG)

## Current Limitations

- Project does not currently support RC Control (may be added in the future)
- The drone is intended to fly briefly (~30 seconds) before motors are safely stopped and it lowers it altitude to ground level

## Specifications

   Dimensions of the drone: 12x12x3 inches (to be updated as later revissions occur)
   Brushed DC motors
   LP9110s motor drivers
   F103RB microcontroler
   DJI Tello Propllers

## Planned Features

- Asserts & Cases to ensure safe operations
- RC Control
