
#include "source.hpp"

bool SampleSource::hasNewSamples() {
	bool ret = m_new_samples;
	m_new_samples = false;
	return ret;
}

void SampleSource::setChunkSize(unsigned size) {
	m_samples = std::vector<float>(size, 0);
	m_chunk_size = size;
}

std::vector<float> const& SampleSource::getSamples() {
	bufferSamples(); // should update m_samples
	return m_samples;
}

