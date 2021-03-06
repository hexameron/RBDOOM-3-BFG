    ____   ____   ____                           _____  ____   ______ ______
   / __ \ / __ ) / __ \ ____   ____   ____ ___  |__  / / __ ) / ____// ____/
  / /_/ // __  |/ / / // __ \ / __ \ / __ `__ \  /_ < / __  |/ /_   / / __  
 / _, _// /_/ // /_/ // /_/ // /_/ // / / / / /___/ // /_/ // __/  / /_/ /  
/_/ |_|/_____//_____/ \____/ \____//_/ /_/ /_//____//_____//_/     \____/   
_________________________________________


RBDoom3BFG Readme - https://github.com/RobertBeckebans/RBDOOM-3-BFG

Thank you for downloading RBDoom3BFG.



_______________________________________

CONTENTS
_______________________________



This file contains the following sections:

	1) SYSTEM REQUIREMENT

	2) GENERAL NOTES
	
	3) LICENSE
	
	4) GETTING THE SOURCE CODE

	5) COMPILING ON WIN32 WITH VISUAL C++ 2010 EXPRESS EDITION

	6) COMPILING ON GNU/LINUX
	
	7) GETTING THE GAMEDATA, RUNNING THE GAME
	
	8) CHANGES
	
	9) FEATURES
	
	10) CONSOLE VARIABLES
	
	11) KNOWN ISSUES
	
	12) BUG REPORTS
	
	13) CODE LICENSE EXCEPTIONS



___________________________________

1) SYSTEM REQUIREMENTS
__________________________



Minimum system requirements:

	CPU: 2 GHz
	System Memory: 512MB
	Graphics card: OpenGL >= 3.2 ( and Direct3D 10 for Windows. )

Recommended system requirements:

	CPU: 3 GHz + Intel compatible
	System Memory: 1024MB+
	Graphics card: Geforce 9600 GT, ATI HD 5650 or higher. 




_______________________________

2) GENERAL NOTES
______________________

This release does not contain any game data, the game data is still
covered by the original EULA and must be obeyed as usual.

You must patch the game to the latest version.

Note that Doom 3 BFG Edition is available from the Steam store at
http://store.steampowered.com/app/208200/


Steam:
------
The Doom 3 BFG Edition GPL Source Code release does not include functionality for integrating with 
Steam.  This includes roaming profiles, achievements, leaderboards, matchmaking, the overlay, or
any other Steam features.


Bink:
-----
The Doom 3 BFG Edition GPL Source Code release does not include functionality for rendering Bink Videos.
It may become possible to play embedded Bink Videos in Doom 3 BFG using the open source FFMEG codecs. 

_______________________________

3) LICENSE
______________________


See COPYING.txt for the GNU GENERAL PUBLIC LICENSE

ADDITIONAL TERMS:  The Doom 3 BFG Edition GPL Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU GPL which accompanied the Doom 3 BFG Edition GPL Source Code.  If not, please request a copy in writing from id Software at id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.


____________________________________________

4) GETTING THE SOURCE CODE
___________________________________

This project's GitHub.net Git repository can be checked out through Git with the following instruction set: 

	> git clone https://github.com/RobertBeckebans/RBDOOM-3-BFG.git

If you don't want to use git, you can download the source as a zip file at
	https://github.com/RobertBeckebans/RBDOOM-3-BFG/archive/master.zip



___________________________________________________________________

5) COMPILING ON WIN32 WITH VISUAL C++ 2010 EXPRESS EDITION
__________________________________________________________

1. Download and install the Visual C++ 2010 Express Edition.

2. Generate the VC10 projects using CMake by doubleclicking a matching configuration .bat file in the neo/ folder.

3. Use the VC10 solution to compile what you need:
	RBDOOM-3-BFG/build/RBDoom3BFG.sln


__________________________________

6) COMPILING ON GNU/LINUX
_________________________


1. You need the following dependencies in order to compile RBDoom3BFG with all features:
 
	On Debian or Ubuntu:

		> apt-get install cmake libsdl1.2-dev libopenal-dev
	
	On Fedora

		> yum install cmake SDL-devel openal-devel
	
	Instead of SDL1.2 development files you can also use SDL2, but so far it 
	seems like no distributions has packages for it yet, so you may have to 
	compile and install SDL2 yourself. 
	(There may be inofficial repositories like Ubuntu PPAs)
	
	SDL2 has better input support (especially in the console) and better 
	support for multiple displays (especially in fullscreen mode).


2. Generate the Makefiles using CMake:

	> cd neo/
	> ./cmake-eclipse-linux-profile.sh
	
3. Compile RBDoom3BFG targets with

	> cd ../build
	> make

___________________________________________________

7) GETTING THE GAMEDATA, RUNNING THE GAME
__________________________________________

To play the game, you need the game data from a legal copy of the game, which 
unfortunately requires Steam for Windows - Steam for Linux or OSX won't do, because
(at least currently) the Doom3 BFG game is only installable on Steam for Windows.
Even the DVD version of Doom3 BFG only contains encrytped data that is decoded
by Steam on install.

Fortunately, you can run Steam in Wine to install Doom3 BFG and afterwards copy the 
game data somewhere else to use it with native executables.
Winetricks ( http://winetricks.org/ ) makes installing Windows Steam on Linux really easy.

Anyway:

1. Install Doom3 BFG in Steam (Windows version), make sure it's getting 
   updated/patched.

2. Create your own doom3bfg directory, e.g. /path/to/Doom3BFG/

3. Copy the game-data's base dir from Steam to that directory 
   (e.g. /path/to/Doom3BFG/), it's in
	/your/path/to/Steam/steamapps/common/DOOM 3 BFG Edition/base/

4. Copy your RBDoom3BFG executable that you created in 5) or 6) to your own 
   doom3bfg directory (/path/to/Doom3BFG).
   
   Your own doom3bfg directory now should look like:
	/path/to/Doom3BFG/
	 ->	RBDoom3BFG (or RBDoom3BFG.exe on Windows)
	 ->	base/
		 ->	classicmusic/
		 ->	_common.crc
		 ->	(etc)

5. Run the game by executing the RBDoom3BFG executable.

6. Enjoy

7. If you run into bugs, please report them, see 12)

___________________________________________________

8) CHANGES
__________________________________________

	* Flexible build system using CMake
	* Linux support (32 and 64 bit)
	* New OpenAL Soft sound backend primarily developed for Linux but works on Windows as well
	* Win64 support

___________________________________________________

9) FEATURES
__________________________________________

// TODO

___________________________________________________

10) CONSOLE VARIABLES
__________________________________________

// TODO


___________________________________________________

11) KNOWN ISSUES
__________________________________________

	* There may be some issues with the binary format loaders/writers on 64 bit platforms.
	* Open source OpenGL drivers do not support OpenGL 3.2 (yet).

___________________________________________________

12) BUG REPORTS
__________________________________________

Please do not report bugs unless you are using the latest code from https://github.com/RobertBeckebans/RBDOOM-3-BFG

RBDoom3BFG is not perfect, it is not bug free as every other software.
For fixing as much problems as possible we need as much bug reports as possible.
We cannot fix anything if we do not know about the problems.

The best way for telling us about a bug is by submitting a bug report at our GitHub bug tracker page:

	https://github.com/RobertBeckebans/RBDOOM-3-BFG/issues?state=open

The most important fact about this tracker is that we cannot simply forget to fix the bugs which are posted there. 
It is also a great way to keep track of fixed stuff.

If you want to report an issue with the game, you should make sure that your report includes all information useful to characterize and reproduce the bug.

    * Search on Google
    * Include the computer's hardware and software description ( CPU, RAM, 3D Card, distribution, kernel etc. )
    * If appropriate, send a console log, a screenshot, an strace ..
    * If you are sending a console log, make sure to enable developer output:

              RBDoom3BFG.exe +set developer 1 +set logfile 2

NOTE: We cannot help you with OS-specific issues like configuring OpenGL correctly, configuring ALSA or configuring the network.
	

	
	
____________________________________________________________________________________

13) CODE LICENSE EXCEPTIONS - The parts that are not covered by the GPL:
_______________________________________________________________________


EXCLUDED CODE:  The code described below and contained in the Doom 3 BFG Edition GPL Source Code release
is not part of the Program covered by the GPL and is expressly excluded from its terms. 
You are solely responsible for obtaining from the copyright holder a license for such code and complying with the applicable license terms.

FFMPEG
-----------------------------------------------------------------------------
Most files in FFmpeg are under the GNU Lesser General Public License version 2.1
or later (LGPL v2.1+). Read the file COPYING.LGPLv2.1 for details. Some other
files have MIT/X11/BSD-style licenses. In combination the LGPL v2.1+ applies to
FFmpeg.

JPEG library
-----------------------------------------------------------------------------
neo/libs/jpeg-6/*

Copyright (C) 1991-1995, Thomas G. Lane

Permission is hereby granted to use, copy, modify, and distribute this
software (or portions thereof) for any purpose, without fee, subject to these
conditions:
(1) If any part of the source code for this software is distributed, then this
README file must be included, with this copyright and no-warranty notice
unaltered; and any additions, deletions, or changes to the original files
must be clearly indicated in accompanying documentation.
(2) If only executable code is distributed, then the accompanying
documentation must state that "this software is based in part on the work of
the Independent JPEG Group".
(3) Permission for use of this software is granted only if the user accepts
full responsibility for any undesirable consequences; the authors accept
NO LIABILITY for damages of any kind.

These conditions apply to any software derived from or based on the IJG code,
not just to the unmodified library.  If you use our work, you ought to
acknowledge us.

NOTE: unfortunately the README that came with our copy of the library has
been lost, so the one from release 6b is included instead. There are a few
'glue type' modifications to the library to make it easier to use from
the engine, but otherwise the dependency can be easily cleaned up to a
better release of the library.

zlib library
---------------------------------------------------------------------------
neo/libs/zlib/*

Copyright (C) 1995-2012 Jean-loup Gailly and Mark Adler

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Base64 implementation
---------------------------------------------------------------------------
neo/idlib/Base64.cpp

Copyright (c) 1996 Lars Wirzenius.  All rights reserved.

June 14 2003: TTimo <ttimo@idsoftware.com>
	modified + endian bug fixes
	http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=197039

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

IO for (un)compress .zip files using zlib
---------------------------------------------------------------------------
neo/libs/zlib/minizip/*

Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

Modifications of Unzip for Zip64
Copyright (C) 2007-2008 Even Rouault

Modifications for Zip64 support
Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

MD4 Message-Digest Algorithm
-----------------------------------------------------------------------------
neo/idlib/hashing/MD4.cpp
Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD4 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD4 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

MD5 Message-Digest Algorithm
-----------------------------------------------------------------------------
neo/idlib/hashing/MD5.cpp
This code implements the MD5 message-digest algorithm.
The algorithm is due to Ron Rivest.  This code was
written by Colin Plumb in 1993, no copyright is claimed.
This code is in the public domain; do with it what you wish.

CRC32 Checksum
-----------------------------------------------------------------------------
neo/idlib/hashing/CRC32.cpp
Copyright (C) 1995-1998 Mark Adler

OpenGL headers
---------------------------------------------------------------------------
neo/renderer/OpenGL/glext.h
neo/renderer/OpenGL/wglext.h

Copyright (c) 2007-2012 The Khronos Group Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and/or associated documentation files (the
"Materials"), to deal in the Materials without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Materials, and to
permit persons to whom the Materials are furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Materials.

THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.

Timidity
---------------------------------------------------------------------------
neo/libs/timidity/*

Copyright (c) 1995 Tuukka Toivonen 

From http://www.cgs.fi/~tt/discontinued.html :

If you'd like to continue hacking on TiMidity, feel free. I'm
hereby extending the TiMidity license agreement: you can now 
select the most convenient license for your needs from (1) the
GNU GPL, (2) the GNU LGPL, or (3) the Perl Artistic License.  

