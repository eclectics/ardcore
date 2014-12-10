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

## e16_oneliner

A simple sketch to select from the totally amazing [one line music programs
created and collected by Viznut](http://countercomplex.blogspot.com.au/search/label/algorithmic%20music)
 
* A0 sets the delay between each iteration (I like about 1/4 turn cw)
* A2 chooses the formula
* controls are only read on clock in, so for responsive controls give it
a quick clock. This also allows patching a random source into A2 and using
a gate to change the formula, or patching cv and gate from a keyboard for
easier control.


## e22_quantiser

Basic quantiser

* A0 scale selection (from those set in sketch)
* A1 octave range
* A2 cv to be quantised, full CW for no attenuation; attenuation will alter the range but not in octave increments
* D1 lights when note chnages
* analog out for quantised cv

Different scales can be created, the  process is straightforward and documented in the sketch.

Thanks to dashwood for proper tuning values.

## e26_rdntrig

Simple clocked random triggers on D0/D1, and random looping sequence on analog
out (low byte of a 16 bit shift register, ala Turing Machine and others.

Trigger time is 30ms, easy to edit in sketch

* Clk in for clock
* A0/A1 choose chance of D0/D1 trigger on clock from CCW never to CW always
* A3 chooses chance of bit flip on the 16 bit shift register from CCW never to
CW always

## e27_rndswtrig

Similar to above, but on each clock either D0 or D1 is triggered, chance of D0
determined by A0.
 
* Clk in for clock
* A0 choose chance of D0 trigger on clock from CCW never to CW always; D1
triggers when D0 doesn't
* A3 chooses chance of bit flip on the 16 bit shift register from CCW never to
* A4 scales the analog out from fully attenuated CCW to fully open CW
