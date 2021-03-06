/*
 ===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/





// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	System interface for sound, from lxdoom.
//
//-----------------------------------------------------------------------------

// Only build SDL if NOT for MSVC, because I can't get cmake to delete this file from the list..
#ifndef _MSC_VER

#include <math.h>
#include <unistd.h>

#include <SDL/SDL_audio.h>
#include <SDL/SDL_mutex.h>
#include <SDL/SDL_byteorder.h>
#include <SDL/SDL_version.h>

#include "z_zone.h"
#include "m_swap.h"
#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"
#include "d_main.h"

#include "../libs/timidity/timidity.h"
#include "../libs/timidity/controls.h"

qboolean	Music_initialized = false;
qboolean	Sound_initialized = false;

/* The number of internal mixing channels,
  the samples calculated for each mixing step,
  the size of the 16bit, 2 hardware channel (stereo)
  mixing buffer, and the samplerate of the raw data. */

#define CDQ441 44100
// Needed for calling the actual sound output.
#define SAMPLECOUNT	2048
#define NUM_CHANNELS	8
#define SAMPLERATE	CDQ441
// SFX are at 11025, but we double-up to 44010

// The actual lengths of all sound effects.
int	lengths[NUMSFX];

// The actual output device.
int	audio_fd;

// The channel step amount...
unsigned int	channelstep[NUM_CHANNELS];
// ... and a 0.16 bit remainder of last step.
unsigned int	channelstepremainder[NUM_CHANNELS];


// The channel data pointers, start and end.
unsigned char*	channels[NUM_CHANNELS];
unsigned char*	channelsend[NUM_CHANNELS];

// Time/gametic that the channel started playing,
//  used to determine oldest, which automatically
//  has lowest priority.
// In case number of active sounds exceeds
//  available channels.
int		channelstart[NUM_CHANNELS];

// The sound in channel handles,
//  determined on registration,
//  might be used to unregister/stop/modify,
//  currently unused.
int 		channelhandles[NUM_CHANNELS];

// SFX id of the playing sound effect.
// Used to catch duplicates (like chainsaw).
int		channelids[NUM_CHANNELS];			

// Pitch to stepping lookup, unused.
int		steptable[256];

// Set initial Volume.
int		snd_SfxVolume = 0; // goes upto 15*15
int		snd_MusVolume = 0; // goes upto 15*15

// Hardware left and right channel volume lookup.
int		sfxleftvol[NUM_CHANNELS];
int		sfxrightvol[NUM_CHANNELS];

// Timidity
signed short*	musicBuffer = NULL;
signed short*	musicBufferEnd = NULL;
signed short*	musicptr = NULL;
intptr_t	totalBufferSize;

//
// This function loads the sound data from the WAD lump,
//  for single sound.
//
void*
getsfx
( const char*         sfxname,
  int*          len )
{
    unsigned char*      sfx;
    unsigned char*      paddedsfx;
    int                 i;
    int                 size;
    int                 paddedsize;
    char                name[20];
    int                 sfxlump;

    // Get the sound data from the WAD, allocate lump
    sprintf(name, "ds%s", sfxname);
    // using default sound "dspistol"
    if ( W_CheckNumForName(name) == -1 )
      sfxlump = W_GetNumForName("dspistol");
    else
      sfxlump = W_GetNumForName(name);
    
    size = W_LumpLength( sfxlump );

    sfx = (unsigned char*)W_CacheLumpNum(sfxlump, PU_CACHE_SHARED );

    paddedsize = (size + 7) & ~8;

    // Allocate from zone memory.
    paddedsfx = (unsigned char*)Z_Malloc( paddedsize+8, PU_STATIC, 0 );
    // ddt: (unsigned char *) realloc(sfx, paddedsize+8);
    // This should interfere with zone memory handling,
    //  which does not kick in in the soundserver.

    memcpy(  paddedsfx, sfx, size );
    for (i=size ; i<paddedsize+8 ; i++)
        paddedsfx[i] = 128;

    // Remove the cached lump.
    Z_Free( sfx );
    
    // Preserve padded length.
    *len = paddedsize;

    // Return allocated padded data.
    return (void *) (paddedsfx + 8);
}

//
// This function adds a sound to the
//  list of currently active sounds,
//  which is maintained as a given number
//  (eight, usually) of internal channels.
// Returns a handle.
//
int
addsfx
( int		sfxid,
  int		volume,
  int		step,
  int		seperation )
{
    static unsigned short	handlenums = 0;
    int		i;
    int		oldestnum = 0;
    int		slot;
    int		gametic = I_GetTime();
    int		oldest = gametic;

    // Play these sound effects only one at a time.
    if ( sfxid == sfx_sawup
	 || sfxid == sfx_sawidl
	 || sfxid == sfx_sawful
	 || sfxid == sfx_sawhit
	 || sfxid == sfx_stnmov
	 || sfxid == sfx_pistol	 )
    {
	i = 0; // use first slot only for those.
		// Win32 code plays the loudest Chainsaw.
		// Pistol ( Chaingun ) should be in another slot
    } else {
	// Loop other channels to find oldest SFX.
	for ( i = 1; (i<NUM_CHANNELS) && (channels[i]); i++)
	{
		if (channelstart[i] < oldest)
		{
			oldestnum = i;
			oldest = channelstart[i];
		}
	}
    }

    // If we found a channel, fine.
    // If not, we simply overwrite the first one.
    if (i == NUM_CHANNELS)
	slot = oldestnum;
    else
	slot = i;

    // Okay, in the less recent channel,
    //  we will handle the new SFX.
    // Set pointer to raw data.
    channels[slot] = (unsigned char *) S_sfx[sfxid].data;
    // Set pointer to end of raw data.
    channelsend[slot] = channels[slot] + lengths[sfxid] - 1;

    // Reset current handle number, limited to 0..100.
    if (!handlenums)
	handlenums = 100;

    // Assign current handle number.
    // Preserved so sounds could be stopped (unused).
    channelhandles[slot] = --handlenums;
    channelstep[slot] = step;
    channelstepremainder[slot] = 0;
    channelstart[slot] = gametic;

    // Separation, that is, orientation/stereo.
    //  range is: 128-n to 128+n
    //  Testing suggests 64 < n < 80

    //  x^2 seperation,
    //  snd_SfxVolume is 0-15*15, 8 bit
    volume = snd_SfxVolume << 2; // 10 bit
    sfxleftvol[slot] = volume - ( (volume*seperation*seperation) >> 16);
    seperation = 256 - seperation;
    sfxrightvol[slot] = volume - ( (volume*seperation*seperation) >> 16);

    // Preserve sound SFX id,
    channelids[slot] = sfxid;
    return handlenums;
}

void I_SetChannels()
{
  // Init internal lookups (raw data, mixing buffer, channels).
  // This function sets up internal lookups used during
  //  the mixing process. 
  int		i;
  int		j;
    
  int*	steptablemid = steptable + 128;
  
  // Preset internal mixing channels to zero.
  for (i=0; i<NUM_CHANNELS; i++)
  {
    channels[i] = 0;
  }

  // This table provides step widths for pitch parameters.
  for (i=-128 ; i<128 ; i++)
    steptablemid[i] = (int)(pow(2.0, (i/64.0))*65536.0);
}	

void I_SetSfxVolume(int volume)
{
    snd_SfxVolume = volume * volume;
}

// Retrieve the raw data lump index
//  for a given SFX name.
int I_GetSfxLumpNum(sfxinfo_t* sfx)
{
    char namebuf[9];
    sprintf(namebuf, "ds%s", sfx->name);
    return W_GetNumForName(namebuf);
}

// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.

int I_StartSound ( int id, mobj_t *origin, mobj_t *listener, int vol, int pitch, int priority ) {
	priority = 0;
	float to_x, to_y;
	float cos_x, sin_y;
	float sound_x, sound_y;

	int seperation = 128;
	angle_t pAngle;

	if (listener && origin)
	{
		pAngle = listener->angle >> ANGLETOFINESHIFT;

		to_x = (float)(origin->x - listener->x);
		to_y = (float)(origin->y - listener->y);
		cos_x = (float)(finecosine[pAngle]);
		sin_y = (float)(finesine[pAngle]);

		sound_x = to_x * cos_x + to_y *sin_y;
		sound_y = to_x * sin_y - to_y *cos_x;

		if ( sound_x < 0 )
			{ sound_x = - sound_x; }
		sound_x += 10;  // avoid divide by zero.

		if ( sound_y < -sound_x )
			{ seperation -= 80; }
		if ( sound_y > sound_x )
			{ seperation += 80; }
		if ( seperation == 128 )
			{ seperation += (int)( sound_y * 80.0 / sound_x ); }
	}

	// volume is calculated in i_sound.cpp
	SDL_LockAudio();
	id = addsfx( id, vol, steptable[pitch], seperation );
	SDL_UnlockAudio();

	return id;
}

void I_StopSound (int handle, int player)
{
  // You need the handle returned by StartSound.
  // Would be looping all channels,
  //  tracking down the handle,
  //  an setting the channel to zero.
}

int I_SoundIsPlaying(int handle)
{
    // Ouch.
    return 0; // gametic < handle;
}

void I_UpdateSound( void ){}

// Adust sound for current player location.
//
// This function loops all active (internal) sound
//  channels, retrieves a given number of samples
//  from the raw sound data, modifies it according
//  to the current (internal) channel parameters,
//  mixes the per channel samples into the given
//  mixing buffer, and clamping it to the allowed
//  range.
//
// This function currently supports only 16bit.
//

static SDL_AudioSpec audio;

// SDL callback
void I_UpdateSound(void *unused, Uint8 *stream, int len)
{
  // Mix current sound data.
  // Data, from raw sound, for right and left.
  unsigned char	sample, nsample;
  int msample, isample;
  register int		dl;
  register int		dr;
  int	interpolate;

  signed short*		leftout;
  signed short*		rightout;
  signed short*		leftend;
  // Step in stream, left and right, thus two.
  int	step;

  // Mixing channel index.
  int	chan;

    // Left and right channel
    //  are in audio stream, alternating.
    leftout = (signed short *)stream;
    rightout = ((signed short *)stream)+1;
    step = 2;
    interpolate = 0;

    // Determine end, for left channel only
    //  (right channel is implicit).
    leftend = leftout + (len >> 1);

    // Mix sounds into the mixing buffer.
    // Loop over step*SAMPLECOUNT,
    //  that is 512 values for two channels.
    while (leftout < leftend)
    {
	dl = 0;
	dr = 0;
	++interpolate&=3;

	for ( chan = 0; chan < NUM_CHANNELS; chan++ )
	{
	    // Check channel, if active.
	    if (channels[ chan ])
	    {
		// Get the raw data from the channel. 
		sample = *channels[ chan ];
		// It may not be quicker to interpolate bytes in real time
		// than precalculate 4 signed short words in memory.
		// Assume that we are on a second cpu, so cycles are free.
		// Approx sin curve interpolation:4/0/8/C becomes  4431/0026/889B/CC..
		// 8 bit unsigned byte becomes 10 bits signed int.
		if ( (interpolate == 1)||(interpolate == 2) )
		{
			isample = (sample << 2) - (0x80 << 2);
		}else{
			nsample = *(channels[ chan ] + 1);
			if (interpolate == 3)
			{
				isample = ( sample * 3 + nsample )
					- ( 0x80 * 3 + 0x80 );
			}else{
				isample = ( sample + nsample * 3 )
					- ( 0x80 * 3 + 0x80 );
			}
		}

		dl += (isample * sfxleftvol[ chan ]) >> 5;
		dr += (isample * sfxrightvol[ chan ]) >> 5;

		if (interpolate == 0)
		// expect that the output buffer is a multiple of 4 pairs
		{
			channelstepremainder[ chan ] += channelstep[ chan ];
			channels[ chan ] += channelstepremainder[ chan ] >> 16;
			channelstepremainder[ chan ] &= 65536-1;
			if (channels[ chan ] >= channelsend[ chan ])
				channels[ chan ] = 0;
		}
	    }
	}

	if (musicBuffer)
	{
		// mix in music
		msample = (int)*( musicptr++ );
		dl += ( snd_MusVolume * msample ) >> 8;
		msample = (int)*( musicptr++ );
		dr += ( snd_MusVolume * msample ) >> 8;
		if ( musicptr >= musicBufferEnd )
			musicptr = musicBuffer;
	}

	// Clamp to range. Left hardware channel.
	if (dl > 0x7fff)
	    *leftout = 0x7fff;
	else if (dl < -0x8000)
	    *leftout = -0x8000;
	else
	    *leftout = dl;

	// Same for right hardware channel.
	if (dr > 0x7fff)
	    *rightout = 0x7fff;
	else if (dr < -0x8000)
	    *rightout = -0x8000;
	else
	    *rightout = dr;

	// Increment current pointers in stream
	leftout += step;
	rightout += step;
    }
}

void
I_UpdateSoundParams
( int	handle,
  int	vol,
  int	sep,
  int	pitch)
{
  // UNUSED.
  handle = vol = sep = pitch = 0;
}

void I_ShutdownSound(void)
{
  int chan;

  Sound_initialized = false;
  I_ShutdownMusic();
  SDL_CloseAudio();

// Segfault issue with Doom II closing audio and restarting on level load.
// Clear channel pointers, to stop effects playing in SDL callback.
  for ( chan = 0; chan < NUM_CHANNELS; chan++ )
  {
	channels[ chan ] = 0;
  }
}

void
I_InitSound()
{ 
  int i;

  if ( Sound_initialized == true ){
	return;
  }
  
  // Open the audio device
  audio.freq = SAMPLERATE;
  if ( SDL_BYTEORDER == SDL_BIG_ENDIAN ) {
    audio.format = AUDIO_S16MSB;
  } else {
    audio.format = AUDIO_S16LSB;
  }
  audio.channels = 2;
  audio.samples = SAMPLECOUNT;
  audio.callback = I_UpdateSound;
  if ( SDL_OpenAudio(&audio, NULL) < 0 ) {
    I_PrintfE("Failed to open audio with desired format\n");
    return;
  }
  I_Printf("Configured audio device with %d samples/slice\n", audio.samples);

  // Initialize external data (all sounds) at start, keep static.
  for (i=1 ; i<NUMSFX ; i++)
  { 
    // Alias? Example is the chaingun sound linked to pistol.
    if (!S_sfx[i].link)
    {
      // Load data from WAD file.
      S_sfx[i].data = getsfx( S_sfx[i].name, &lengths[i] );
    }	
    else
    {
      // Previously loaded already?
      S_sfx[i].data = S_sfx[i].link->data;
      lengths[i] = lengths[(S_sfx[i].link - S_sfx)/sizeof(sfxinfo_t)];
    }
  }
  
  SDL_PauseAudio(0);
  Sound_initialized = true;
  I_InitMusic();
}

void I_SubmitSound(void){}

void I_ProcessSoundEvents( void ) {}

//
// MUSIC API.
//

int Mus2Midi(unsigned char* bytes, unsigned char* out, int* len);


void I_ShutdownMusic(void) 
{
	if ( Music_initialized == false ) return;
	Music_initialized = false;

	if ( musicBuffer != NULL )
	{
		SDL_LockAudio();
		free( musicBuffer );
		musicBuffer = NULL;
		SDL_UnlockAudio();
	}
	Timidity_Shutdown();
}

#define MIDI_RATE	CDQ441
#define MIDI_CHANNELS	2
#define MIDI_FORMAT	AUDIO_S16
#define BYTESPERSAMPLE  2

void I_InitMusic(void)
{
	int err;
	if ( Music_initialized == true ) return;

	Music_initialized = true;

	err = Timidity_Init( MIDI_RATE, MIDI_FORMAT, MIDI_CHANNELS, MIDI_RATE, "classicmusic/gravis.cfg" );
	if ( err < 0 )
	{
		I_Printf("Timidity_Init Error: %d\n",err);
	}
}

MidiSong* doomMusic;

namespace {
	const int MaxMidiConversionSize = 1024 * 1024;
	unsigned char midiConversionBuffer[MaxMidiConversionSize];
}

void I_PlaySong( const char *songname, int looping)
{
	int length = 0;
	idStr lumpName = "d_";

	if ( !Music_initialized ) return;

	if ( musicBuffer != NULL )
	{
		SDL_LockAudio();
		free( musicBuffer );
		musicBuffer = NULL;
		SDL_UnlockAudio();
        }


	lumpName += static_cast< const char * >( songname );
	unsigned char * musFile = static_cast< unsigned char * >( W_CacheLumpName( lumpName.c_str(), PU_STATIC_SHARED ) );

	Mus2Midi( musFile, midiConversionBuffer, &length );

	I_Printf( "Converted Midi file:%d bytes.\n", length );

	doomMusic = Timidity_LoadSongMem( midiConversionBuffer, length );

	if ( doomMusic ) {
		totalBufferSize = doomMusic->samples * MIDI_CHANNELS * BYTESPERSAMPLE;
		musicptr = (signed short *)malloc( totalBufferSize );

		Timidity_Start( doomMusic );

                int	rc = 0;
		int	offset = 0;
		int	num_bytes = 0;

                do {
                        rc = Timidity_PlaySome( (byte *)musicptr + offset, MIDI_RATE, &num_bytes );
                        offset += num_bytes;
                } while ( rc != RC_TUNE_END );

		// music will start playing as soon as musicBuffer is set
		musicBufferEnd = musicptr + ( totalBufferSize >> 1 );
		musicBuffer = musicptr;

                Timidity_Stop();
                Timidity_FreeSong( doomMusic );
	}
}

void I_PauseSong (int handle)
{
}

void I_ResumeSong (int handle)
{
}

void I_StopSong(int handle)
{
	if ( musicBuffer != NULL )
	{
		SDL_LockAudio();
		free( musicBuffer );
		musicBuffer = NULL;
		SDL_UnlockAudio();
	}
}

void I_UnRegisterSong(int handle)
{
// Does nothing.
}

int I_RegisterSong(void* data, size_t len)
{
// Does nothing.
  return (0);
}

void I_SetMusicVolume(int volume)
{
	snd_MusVolume = volume * volume;
}

#endif // _MSC_VER not defined
