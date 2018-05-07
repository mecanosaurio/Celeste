s.boot
s.scope
s.plotTree;

t = TempoClock.new(144/60, 0);
t.schedAbs(t.nextBar, (t.beatsPerBar_(14)));
ProxySpace.push(s.boot, clock:t);

(
SynthDef.new(\pads,{
	arg freq=440, sustain=1, amp=0.1, pan=0;
	var note, env, osc, output;

	osc = {LFSaw.ar(freq, mul: amp) + LFCub.ar(freq+1, mul:amp)};
	// note = Env([1, 0.8, 0.6, 0.4, 0.2, 0] * amp, Array.fill(6, t.beatDur * sustain/4));
	env = EnvGen.kr(Env.perc(0.005, releaseTime:sustain*t.beatDur), doneAction:2);
	Out.ar(0, Pan2.ar((osc * env), pan) );
}).add;



SynthDef.new(\kit,{
		arg pan=0, bdpan=0, hhpan=0, adpan=0,bd=0, hh=0, ad=0, amp=0.5;
		var osc, env, output1, output2, kick, snare, hihat;

		// bass drum sound
		osc = {SinOsc.ar(60, 1, amp*bd)};
		env = {Line.ar(1, 0, 0.15, doneAction:2)};
		output1 = osc*env;

		osc = {PinkNoise.ar(0.1 *  bd * amp)};
		env = {Line.ar(1, 0, 0.1)};
		output2 = osc*env;

		kick = Pan2.ar(output1 + output2, bdpan);

		// hi-hat sound
		hihat = Pan2.ar({WhiteNoise.ar(amp * hh * 0.5) * EnvGen.ar(Env.perc(0.002, 0.05,amp));}, hhpan);

		// snare sound
		osc = LFNoise0.ar(6000, mul: ad*amp);
		env = EnvGen.ar(Env.perc(0.01, 0.1, amp));
		snare = Pan2.ar(osc * env, adpan);

		// collect
		Out.ar(0, Pan2.ar(kick + snare + hihat, pan));
}).add;


SynthDef.new(\reversenote, {
		arg freq=440, sustain=1, amp=0.1, pan=0;
		var note, env, osc, output;

		osc = SinOsc.ar(freq, mul: amp);
		note = Env([0, 0, 0.1, 0.4, 1] / 5, Array.fill(4, (t.beatDur * sustain/4)));
		env = {EnvGen.kr(note, doneAction:2)};
		output = osc*env;

		osc = SinOsc.ar( Line.kr(0, freq, t.beatDur), mul: amp);
		env = EnvGen.kr(Env.perc(0.06, sustain * t.beatDur, amp), doneAction:2);
		Out.ar(0, Pan2.ar((output + (osc*env)), pan));
}).add;



SynthDef.new(\glissando,{

	arg f1=440, f2=250, sustain=4, amp=0.1, pan=0;
	var note, osc, env;
	osc = LFTri.ar(Line.kr(f1, f2, sustain));
	note = Env([0,0.4,0.8,0.7,0.6,0.5,0.4,0.3,0.2,0.1,0] * amp, Array.fill(10, sustain * t.beatDur/10));
	env = {EnvGen.kr(note, doneAction:2)};
	Out.ar(0, Pan2.ar((osc * env), pan));
}).add;


)


