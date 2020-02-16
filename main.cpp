
#include <iostream>
#include <SFML/Audio.hpp>
#include <cstring>

#include "stdio.h"
#include "win.hpp"
#include "math.hpp"

#include "recorder.hpp"

#include "unsupported/Eigen/FFT"

#define N 1024



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

/*
void pipeSamples(std::vector<NTYPE>& timevec, unsigned amount) {
    for (unsigned i = 0; i < amount; i++) { 
        char buf[2];
        // getchar(); getchar();
        buf[0] = getchar();
        buf[1] = getchar();
        timevec.at(i) = (NTYPE) *((int16_t*) buf);
    }
}
*/


int main(int argc, char* argv[]) {


    // init sources and check args
    // SampleRecorder source;
    // source.setChunkSize(N); 
    
    sf::SoundBuffer sbuf;
        sf::Sound sound;
    if (argc > 1) {
        std::string filepath = argv[1];
        sbuf.loadFromFile(filepath);
        // source.initFileSource(filepath);
        // sound.setBuffer(source.getSoundBuffer());
        sound.setBuffer(sbuf);
        sound.setLoop(true);
        sound.play();
    } else {
        // source.start();
    }
    

    // initialize audio info, FFT & vectors, peak data container, and the window

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

        /*
        if (!source.hasNewSamples()) {
            sf::sleep(sf::milliseconds(10));
            continue;
        }
        */

        timevec = source.getSamples();
        std::for_each(timevec.begin(), timevec.end(), math::hanning_filter);

        // calculate FFT
        fft.fwd(freqvec, timevec);
    
        // clear screen with a solid flash based on previous calculation    
        win.flash(flash_intensity, sf::Color::Cyan);
    
        // draw the graph, update & draw the lines  
        peak_data = win.visualize_graph(freqvec, {50,0.001});

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
    // source.stop();
    return 0;
}
