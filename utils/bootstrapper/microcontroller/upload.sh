#!/bin/bash
avrdude -c usbasp -F -p m328p -U flash:w:$1:a
