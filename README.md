# ShoutSend
# simple streaming source client for ShoutCast/IceCast

**Original source can be found [here](http://zagrodzki.net/~sebek/shoutsend/), This is just a github fork**

ShoutSend is a simple streaming client for ShoutCast/IceCast servers. It takes
audio data stream from stdin (in mp3 or ogg format) and without any processing
sends it along with some metadata (like stream description) to streaming server.

You'll find old documentation (in Polish) in index-pl.html.

You can read about how this tool was used to provide live audio transmissions
from Polish Linux User Group meetings in Warsaw (Polish only, sorry):
http://warszawa.linux.org.pl/nagrywanie.html


## Compilation:

	gcc -o shoutsend -lshout -lpthread shoutsend.c

If you use libshoutcast version 1, use shoutsend-for-libshout1.c:

	gcc -o shoutsend -lshout shoutsend-for-libshout1.c

Please note, that shoutsend-for-libshout1 supports only mp3.


## Examples

Example: stream mp3 file repeatedly to http://127.0.0.1:8000/stream.mp3

	while true; do cat myfile.mp3; done | shoutsend -m /stream.mp3
 

Example: record audio from sound card using "rawrec" program, encode to
64kbit mp3 format, then simultaneously save to a file and send it to
server at http://127.0.0.1:8000/live.mp3:

	rawrec | lame -r -b 64 - - | tee somefile.mp3 | shoutsend

(you might have to use additional options for lame to specify your input stream
parameters, like samplerate, number of channels, endianness etc.)


Example: fetch stream in aac format using "mplayer", then reencode it to both
mp3/128kbit and ogg/vorbis/96kbit formats simultaneusly (using "lame", "oggenc"
and "tpipe") and stream to two mountpoints on localhost. If the source stream
is not available, play some information message, wait 5s and try again:

	#!/bin/sh
	while true; do
		mplayer -ao pcm:file=/proc/self/fd/3 \
			http://213.251.138.121:7000/ \
			3>&1 >/dev/null 2>&1
		cat ~/pleasewait.wav
		sleep 5
	done |\
	tpipe "lame -b 128 - - | shoutsend -m /live.mp3" |\
	oggenc -Q -b 96 -o - - | shoutsend -o -m /live.ogg


## Usage

	Usage: shoutsend [options ...]
	Version 1.02
	Options:
		-s host		server (default: 127.0.0.1)
		-p port		port (default: 8000)
		-U user		username (default: source)
		-P pass		password (default: letmein)
		-o		set data format to OGG/Vorbis (default: MP3)
		-m mnt		mountpoint (default: /live.mp3)
		-d description	stream description (default: none)
		-u url		stream description url (default: none)
		-1		use AudioCast protocol (IceCast1 compatible)
		-S		use ShoutCast protocol (ShoutCast compatible)
				default protocol: HTTP (IceCast2 compatible)

## License

This software is provided under terms and conditions of GNU GPL license<BR>

