#ifndef RECORDER_HH
#define RECORDER_HH

#include <SFML/Audio.hpp>

#include "source.hpp"

class SampleRecorder : public sf::SoundRecorder, public SampleSource
{
	public:
		SampleRecorder();
		~SampleRecorder();

		void setBufferingInterval(int32_t);	
		virtual void bufferSamples() override;

	protected:
		virtual bool onStart() override;
		virtual bool onProcessSamples(const int16_t*, size_t) override;

	private:
		int32_t m_interval_usec;
};

#endif
