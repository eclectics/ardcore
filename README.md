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

## e18_quneo

An experiment in interfacing the ardcore with the KMI QuNeo controller.
The sketch itself is a simple 16 step trigger sequencer.

* clkIn for sequencer rate
* D1 for trigger out
* A2 for serial midi in (fully CW)
* D0 for serial midi out

The QuNeo is attached via the expander to a midi din to mini jack interface
which goes via regular patch cables to the ardcore. I've used a powered arduino midi
shield and a couple of alligator leads and jack sockets to do this.

I'm using a QuNeo preset supplied as ArdcoreInterface.quneopreset which disables most of the midi info it's capable of
sending, and which makes the midi notes sent by the controls match those needed
to control the leds.

The pads on the QuNeo turn on/off the different steps of the trigger sequencer.
Stop/Run work, and the first set of horizontal arrow buttons change the
direction of the sequence. The diamond button beside Stop resets it.

## e22_quantiser

Basic quantiser

* A0 scale selection (from those set in sketch)
* A1 octave range
* A2 cv to be quantised, full CW for no attenuation; attenuation will alter the range but not in octave increments
* D1 lights when note chnages
* analog out for quantised cv

Different scales can be created, the  process is straightforward and documented in the sketch.

Thanks to dashwood for proper tuning values, and the arpeggio scales

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
* A2 chooses chance of bit flip on the 16 bit shift register from CCW never to
* A3 scales the analog out from fully attenuated CCW to fully open CW

## e28_rls
A random looping sequencer in the style of the Turing Machine and its
predecessors. The output can be quantised with user selectable scales which
work the same was as e22. There's also range for the output, and a sequence
length.

Sequence length is interesting; it goes from 8-40 bits, in 1 bit increments.
The sequence is implemented internally as an array rather than as a shift
register. The sequence length always counts from the start of the array, the
shifting is done by moving a window through the array. The upshot is that (for
now at least) for a locked sequence, the minimum sequence length always refers to the same
8 positions, not those coming up in the sequence.

Trigger is selectable on startup using A0. CCW to about 20% and D0 fires when the 
TM bit flips. After about 20% it's a selectable % chance to change-- but can't be
altered without cycling power on the ardcore.

* A0 User selectable scale for quantisation (details in the sketch). CCW is
unquantised.
* A1 Range for the output, CCW is minimal, CW is full range. Unlike e22, range
is by number of notes, not by octave
* A2 Chance of bit getting flipped (changing the sequence), CCW no chance -- so
sequence is locked, CW is always (so sequence is locked at double length,
2nd part is the inverse of the first).
* A3 length of shift register, from 8 bits to 40, incrementing in single bits
* D0 trigger either following bit flip on the sequence, or chance set using A0
on startup. D1 does the opposite, so either D0 or D1 fires each clock pulse.

## e31_random
A bunch of different sorts of random on clock, with adjustable range and some other
parameters.
* A0 - which random to use
  * random (pick a number)
  * random looping sequencer (A2 controls chance of change CCW 0% - CW 100%)
  * random walk
  * shift register (not really random, just a 255 length sequence)
  * random pick of the last 12 notes
  * flips a random bit
  * randomly picks one of the others
* A1 - range that the random value is mapped on to CW is full range
* A2 - chance of flip for TM (CCW is low, CW is high)
* A3 - chance of trigger on D0
* D0 - trigger based on A3 chance
* D1 - digital noise
* Clk - required for random dac




