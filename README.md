lc1
===

A pellet burner controller

The aim of this project is to create a more auotomated system for my pellet burner.

Current the burner is controller using a small ST6 microcontroller with very limited
capabilities. The intention with this project is extending the functionality with a
lambda probe and to adaptively control the burner head.

Right now the setup is to have one Cubieboard A10 that acts as a system controller.
The Cubieboard is connected to a LC-1 lambda probe using a USB to serial adapter.
A child process continously reads the lambda values from this probe and forwards them
to the parent process through a simple pipe.

There will also be an Arduino board that acts as a subcontroller to the original
controller board of the Pellet Burner Head (PBH). The intention is to have this board
gradually take over the functionality of the original board. Initially it will be
very simple and only have simple functions like detect the start of a burner cycle,
control the burner fan and read the oneWire network of temperature sensors. These
values will be fed back to the cubieboard using a USB connection.

