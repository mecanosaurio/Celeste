t = TempoClock.new(144/60, 0);
t.schedAbs(t.nextBar, (t.beatsPerBar_(14)));
ProxySpace.push(s.boot, clock:t);

p = [-1, -0.75, -0.5, -0.25, 0, 0.25, 0.5, 0.75, 1];


(
t.schedAbs(t.nextBar, {
	/*~echo = Pbind(
		\instrument, \pads,
		\amp, Pn(Pseries(14, -1, 14), inf) / 70,
		\octave, 6,
		\degree, [0, 1, 5, 3],
	//\degree, Pstutter(28, Pseq([0, 3], inf)) + [0, 1, 5, 3],
		\pan, Pseq(p, inf),

		\dur, 0.5,
		\sustain, 1
	);

	~string1 = Pbind(
		\instrument, \reversenote,
		\amp, 0.1,
		\pan, Pseq([1, -1], inf),

		\octave, [5, 6],
		\degree, Pseq([0, 1, -2, 3], inf),

		\dur, 7,
		\sustain, 10,
	);


	~string2 = Pbind(
		\instrument, \reversenote,
		\amp, 0.1,
		\pan, Pseq([-1, 1], inf),

		\octave, [6, 7],
		\degree, Pstutter(2, Pseq([0, 1, -2, 3], inf)) + 2,

		\dur, 3.5,
		\sustain, 7,
	);

	~string3 = Pbind(
		\instrument, \reversenote,
		\amp, 0.1,
		\pan, Pseq([0], inf),

		\octave, [7, 8],
		\degree, Pstutter(4, Pseq([0, 1, -2, 3], inf)) + 4,

		\dur, Pseq([3,3,6,5,2],inf),
		\sustain, 7,
	);*/

	~perc = Pbind(
		\instrument, \kit,
		\amp, 0.25,
		\dur, 0.25,
		//\bd, Place([[1,0], 0, 0, [1,0.2,0.5,1], [1,0], 0, 0], inf),
		\bd, Place([[1,0], 0, 0, 0, 0, 0, 0], inf),
		\hh, Prand([1,1,0,1,0,2,0,1,0.5,1,1,0], inf),
		\hhpan, Pxrand(p, inf),
	);

	// 12:48::https://www.youtube.com/watch?v=lPEdPnl7MrE


});
)

/*
se muestra el proceso de aprendizaje de una ia mediante el error-aprendizaje
en un juego simple, atari-like, correcting and failing
*/

