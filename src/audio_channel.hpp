#ifndef AUDIO_CHANNEL_HPP
#define AUDIO_CHANNEL_HPP

#include "fixed.hpp"
#include "SDL.h"

class Channel
{
public:
	Channel( Fixed volume = 1.0f, Fixed pan = 0.5f );
	virtual ~Channel( void ) { }
	
	Channel( const Channel & );
	
	template <class T>
	void mix_into_stream( const SDL_AudioSpec &spec, Uint8 *stream, uint len, Fixed global_volume = 1 );
	
	virtual bool empty( void ) const = 0;
	
protected:
	void copy( const Channel & );
	
	virtual Uint8 *get_buffer( uint &len ) = 0;
	virtual void flush( uint len ) = 0;
	
	Fixed volume, pan;
};


template <class T>
void Channel::mix_into_stream( const SDL_AudioSpec &spec, Uint8 *stream_, uint len, Fixed global_volume )
{
	T *stream = reinterpret_cast<T *>(stream_);
	
	std::vector<Fixed> channel_volume(spec.channels, global_volume * volume);
	switch (spec.channels)
	{
	case 1:
		break;
	case 2:
	default:
		channel_volume[0] *= 1 - pan;
		channel_volume[1] *= pan;
		break;
	}
	
	T *buffer = reinterpret_cast<T *>(get_buffer(len));
	
	for (uint i = 0; i < len / sizeof(T); )
	{
		for (int c = 0; c < spec.channels; ++c)
		{
			const Sint32 clip = stream[i] + buffer[i] * channel_volume[c];
			stream[i++] = std::min(std::max(SHRT_MIN, clip), SHRT_MAX);
		}
	}
	
	flush(len);
}

#endif // AUDIO_CHANNEL_HPP
