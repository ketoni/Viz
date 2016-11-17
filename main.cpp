
#include <iostream>
#include <SFML/Audio.hpp>
#include <cstring>

#include "win.hpp"
#include "math.hpp"

#include "unsupported/Eigen/FFT"

#define N 2048



void initLines(std::vector<DynamicLine>& lines) {
	
	// middle	
	lines[0].setAcceleration(20.0);
	lines[0].setFillColor(sf::Color::Cyan);
	
	// above
	lines[1].latchTarget(lines[0].getPosition());
	lines[1].setAcceleration(0.01);
	lines[1].setFillColor(sf::Color::Red);
	
	// below
	lines[2].latchTarget(lines[0].getPosition());
	lines[2].setAcceleration(0.1);
	lines[2].setFillColor(sf::Color::Blue);
	lines[2].setBehavior(DynamicLine::BounceBelow);

}



int main(int argc, char* argv[]) {
	
	// check args
	if (argc != 2) {
		std::cerr << "Usage:\n./Viz <audio_file>\n";
		return -1;
	}

	// load the sound buffer
	std::string filepath(argv[1]);
	sf::SoundBuffer sbuf;

	if (!sbuf.loadFromFile(filepath)) {
		return -1;
	}

	// assign the sound buffer, start playing the music
	sf::Sound sound;
	sound.setBuffer(sbuf);
	sound.setLoop(true);
	sound.play();
     
	// initialize audio info, FFT & vectors, peak data container, and the window
	uint32_t sample_rate = sbuf.getSampleRate();
	uint64_t sample_count = sbuf.getSampleCount();
	const int16_t* samples = sbuf.getSamples();

	std::vector<NTYPE> timevec(N, 0);
	std::vector<std::complex<NTYPE>> freqvec = {};

	Eigen::FFT<NTYPE> fft;
	PointData peak_data;
	Window win;
	
	// construct a vector of DynamicLines, call external initializer for the vector
	sf::Vector2f line_size(math::magnitude(win.getSize()) * 2, 1);
	std::vector<DynamicLine> lines(3, DynamicLine(line_size));
	
	initLines(lines);
	lines[0].latchTarget(peak_data.window_pos);

	// initialize loop variables and enter the main loop
	uint32_t cur_sample = 0, prev_sample = 0;
	float offset = 0.0, flash_intensity = 0.0, limit_distance = 0.0, top_distance = 0.0; 
	
	while (win.isOpen()) {

		// check on which sample the playback is
		offset = sound.getPlayingOffset().asSeconds();
		cur_sample = offset * sample_rate;
			
		// if no new samples since the last check, sleep for 10 ms
		if (cur_sample == prev_sample) {
			sf::sleep(sf::milliseconds(10));
			continue;
		}
		prev_sample = cur_sample;

		// if there are enough samples, assign them to timevec and apply Hanning filter
		if ((cur_sample + N) < sample_count) {
			timevec.assign(samples + cur_sample, samples + cur_sample + N);
			std::transform(timevec.begin(), timevec.end(), back_inserter(timevec), math::hanning_filter);

		}

		// calculate FFT
		fft.fwd(freqvec, timevec);
	
		// clear screen with a solid flash based on previous calculation	
		win.flash(flash_intensity, sf::Color::Cyan);
	
		// draw the graph, update & draw the lines	
		peak_data = win.visualize_graph(freqvec, {50,0.0025});

		for (auto& line : lines) {
			line.react();
			win.draw(line);
		}

		// calculate a intensity value based on the lines' positions
		limit_distance = lines[2].getPosition().y - lines[1].getPosition().y;
		top_distance = lines[0].getPosition().y - lines[1].getPosition().y;
		flash_intensity = pow(1 - top_distance / limit_distance, 5);

		// display screen changes and update
		win.update();
	}
		
	return 0;
}
