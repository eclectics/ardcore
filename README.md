# Ardcore

Sketches for the Snazzy FX ardcore eurorack module (and expander)

Most of these sketches are documented in the code itself; but I may have
updated the code more recently than the comments.

There's a bunch more to add after I've tidied them up.

## e11_multifunction 
A bunch of sketches in one. Use A0 and A1 to specify which sketch, then press
the reset. 

With A0 and A1 full CCW, use each knob to dial a value from 0-7, which is shown on the expander LEDs, thus
selecting between up to 32 different sketches (ie 3/4 is equivalent to 4/3)
then press the reset button to load the relevant sketch. There's many empty
slots which will be filled in over time.
 
## e15_meadow

An attempt to create something like meadow physics on the monome. Not that I've
ever played with a monome, or meadow physics -- but it sounded interesting. 

There's 8 independent counters (expander LED 0-7). Counter 0 decrements on each
clock pulse.

When a counter hits 0; the LED lights (400ms gate), and it will decrement any
linked counters, and then run a rule on a destination line, and reset to it's
reset value.

It makes for interesting gate patterns, and also for pseudo-random cv sequences.

It's seriously limited in terms of the interface, you can either set up the controls
or run it, since the 8 expander LEDs is all I have to give interface feedback.

* A0 full CW is running, otherwise in control mode
* A0 anything else is selecting a line to change values (0-7); displayed by the
LED on the expander
* A1 selects the reset value for the line; what it goes back to after hitting 0
* A2 selects the linked lines (a line can't link to itself); the lit ones are
linked
* A3 selects the rule (0-7). Haven't added a way to select the destination yet,
* When running A2/A3 can be read as a result of a couple of rules I've added.

The matrix array holds initial values.

It seems like more fun to decrement all the counters on each clock. 
