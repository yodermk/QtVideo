QtVideo
=======

Little program to convert photos on the command line to frames of a fancy video

Run it with something like:

    ./QtVideo [-d /destination/directory] photo.jpg [photo2.jpg ......]

The output is a series of PNG files, the filename being an 8 digit number starting with 00000000.png.
Each file is one frame.  You can turn all these PNGs into a video thusly:

    ffmpeg -i %8d.png -r 24 -vcodec mjpeg -qscale 1 video.avi

If I want to narrate the video, I start playing it and hit Record on my handy Zoom H2 recorder
at the very same instant.  Talk, and press Stop right when the video is ending.  That should yield
an audio file of about the same length.  Then, merge them:

    ffmpeg -i video.avi -i audio.wav -c:v copy -c:a copy videoWithAudio.avi

And you're done!

Obviously, ffmpeg has a lot of other options you may use at your discretion.

Here is the first "real" video I produced with this method - a short photo documentary of my
trip to Churchill, Manitoba in winter!
[YouTube link](http://youtu.be/N1-njDU4pZU)

Building
--------

It requires Qt 5.2 for the command line parser stuff.  If you remove that it might build on Qt 4, but
I don't really care.  I use Qt 5 and don't want to limit myself to Qt 4 stuff in future additions.

First, just type:

    qmake

to generate the Makefile.  Then, just

   make

and you should have a QtVideo executable!

