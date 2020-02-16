
#include "recorder.hpp"

#include <iostream>

SampleRecorder::SampleRecorder() {
	m_interval_usec = 50;
}

SampleRecorder::~SampleRecorder() {
	stop();
}

void SampleRecorder::setBufferingInterval(int32_t interval) {
	m_interval_usec = interval;
}

void SampleRecorder::bufferSamples() {
	// not implemented, since the buffering is done by the SoundRecorder base class
	// onProcessSamples does about the same, but it's called by the base from another thread
}

bool SampleRecorder::onStart() {
	setProcessingInterval(sf::milliseconds(m_interval_usec));
	return true;
}

bool SampleRecorder::onProcessSamples(const int16_t* samples, size_t sample_count) {
	size_t assign_amount = 0;
	sample_count < m_chunk_size ? assign_amount = sample_count : assign_amount = m_chunk_size;
	//m_samplevec.assign(samples, samples + assign_amount);
	m_new_samples = true;
	return true;
}	
