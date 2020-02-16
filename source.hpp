#ifndef SOURCE_HH
#define SOURCE_HH

#include <vector>

class SampleSource {

	public:
		SampleSource() {}

		bool hasNewSamples();
		void setChunkSize(unsigned);
		
		std::vector<float> const& getSamples();
		virtual void bufferSamples() = 0;

	protected:
		bool m_new_samples = false;
		unsigned m_chunk_size = 0;
		std::vector<float> m_samples;
};

#endif
