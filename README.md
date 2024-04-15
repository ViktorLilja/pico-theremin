# pico-theremin
Code for a DIY Raspberry Pi Pico (RP2040) based digital theremin.

Description of software and tasks
- Signal from antenna needs to be serviced every 0.2ms
- Input from rotary encoder needs to be serviced withing approx. 1 ms
- LCD screen should be updated as often as possible
- DAC needs to be updated at regular intervals with at least 44 kHz frequency

Solution:
- LCD in loop
- Antenna on interrupt
- Rotary encoder on interrupt
- Dac on separate processor core running in loop without interrupts

### TODO:
- Linearization of audio frequency with respect to hand distance
- Implement frequency range selection
- Implement autovibrato
- More accurate frequency counting (PIO)
- Optimization of ISR:s
- Modularization and unit tests

