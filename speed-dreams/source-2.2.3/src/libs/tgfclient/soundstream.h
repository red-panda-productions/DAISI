#ifndef __SoundStream_h__
#define __SoundStream_h__

/***************************************************************************

    file                 : SoundStream.h
    created              : Fri Dec 23 17:35:18 CET 2011
    copyright            : (C) 2011 Bernhard Wymann
    email                : berniw@bluewin.ch
    version              : $Id: soundstream.h 5042 2012-11-11 15:38:27Z pouillot $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* Intherface for sound streams */

class SoundStream
{
	public: 
		SoundStream(const char* path) {};
		virtual ~SoundStream() {};
		
		enum SoundFormat {
			FORMAT_INVALID,
			FORMAT_MONO16,
			FORMAT_STEREO16
		};

		virtual int getRateInHz() = 0;
		virtual SoundFormat getSoundFormat() = 0;

		virtual bool read(char* buffer, const int bufferSize, int* resultSize, const char*& error) = 0;
		virtual void rewind() = 0;
		virtual void display() = 0;
		// isValid is not nice, but I do not want to introduce exceptions at this time
		virtual bool isValid() = 0;
		
};

#endif // __SoundStream_h__
