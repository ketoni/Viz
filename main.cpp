
#include <iostream>
#include <SFML/Audio.hpp>
#include <cstring>

#include "win.hpp"
#include "math.hpp"

#include "unsupported/Eigen/FFT"

#define N 2048


#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>


void initLines(std::vector<DynamicLine>& lines) {

	// middle
	lines[0].setAcceleration(5.0);
	lines[0].setFillColor(sf::Color::Cyan);

	// above
	lines[1].latchTarget(lines[0].getPosition());
	lines[1].setAcceleration(0.1);
	lines[1].setFillColor(sf::Color::Red);

	// below
	lines[2].latchTarget(lines[0].getPosition());
	lines[2].setAcceleration(0.1);
	lines[2].setFillColor(sf::Color::Blue);
	lines[2].setBehavior(DynamicLine::BounceBelow);

}

#include <cmath>


//fR fG fB used as RGB outputs
//fH fS fV used as HSV inputs
void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
  float fC = fV * fS; // Chroma
  float fHPrime = fmod(fH / 60.0, 6);
  float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
  float fM = fV - fC;

  if(0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0;
  } else if(1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0;
  } else if(2 <= fHPrime && fHPrime < 3) {
    fR = 0;
    fG = fC;
    fB = fX;
  } else if(3 <= fHPrime && fHPrime < 4) {
    fR = 0;
    fG = fX;
    fB = fC;
  } else if(4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0;
    fB = fC;
  } else if(5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0;
    fB = fX;
  } else {
    fR = 0;
    fG = 0;
    fB = 0;
  }

  fR += fM;
  fG += fM;
  fB += fM;
}

int initializeUSB() {

    const char *device = "/dev/ttyUSB0";
	int fd;

    fd = open(device, O_RDWR);
	if (fd < 0) {
		printf("Unable to open device '%s':\n", device);
    	perror("");
    	return -1;
    }

	printf("Connected to device '%s'\n", device);

    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    // Block for each read byte
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 1;

    cfsetispeed(&tty, B1000000);
    cfsetospeed(&tty, B1000000);

    // Configure port
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    return fd;

}

void waitForSong(std::string const& filepath, sf::SoundBuffer& sbuf, sf::Sound& sound, int fd) {

    uint8_t buf_out[900];
    char buf_in[256];
    uint8_t led_color[3];
    int i = 0;
    float R, G, B, H, S, V;

	// load the sound buffer
    while (!sbuf.loadFromFile(filepath)) {

        memset(buf_out, 0, sizeof(buf_out));

		H = i++ % 360;
		S = 1;
		V = 0.5f;

		HSVtoRGB(R, G, B, H, S, V);
		led_color[0] = R*255.f;
		led_color[1] = G*255.f;
		led_color[2] = B*255.f;

        for (int i = 3*72; i <= 3*224; i+=3) {
			memcpy(buf_out+i, led_color, 3);
            //memset(buf_out+i, poop, 3);
        }
        write(fd, buf_out, sizeof(buf_out));
        read(fd, &buf_in, sizeof(buf_in));

	}

	// assign the sound buffer, start playing the music
	sound.setBuffer(sbuf);
	// sound.setLoop(true);
	sound.play();

}

int main(int argc, char* argv[]) {


	// check args
	if (argc != 2) {
		std::cerr << "Usage:\n./Viz <audio_file>\n";
		return 1;
	}
	std::string filepath(argv[1]);

    // connect to usb device
    int fd = initializeUSB();
    if (fd == -1) {
        return 1;
    }

    // Initialize sound
	sf::SoundBuffer sbuf;
	sf::Sound sound;

    // initialize FFT & vectors, peak data container, and the window
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

    uint8_t buf_out[900];
    char buf_in[256];
	uint8_t led_color[3];
	int i = 0;

    uint32_t sample_rate, cur_sample, prev_sample;
    uint64_t sample_count;
    const int16_t* samples;
    float offset, flash_intensity, limit_distance, top_distance;

	while (win.isOpen()) {


        while (sound.getStatus() == sf::Sound::Stopped) {
            system("rm /home/keksi/poop/audio.wav");
            system("rm /home/keksi/poop/tube.*");
            waitForSong(filepath, sbuf, sound, fd);

            // initialize loop variables
            cur_sample = 0;
            prev_sample = 0;
            offset = 0.0;
            flash_intensity = 0.0;
            limit_distance = 0.0;
            top_distance = 0.0;

            // initialize audio info, FFT & vectors, peak data container, and the window
            sample_rate = sbuf.getSampleRate();
            sample_count = sbuf.getSampleCount();
            samples = sbuf.getSamples();
        }

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

        // LED stuff
        memset(buf_out, 0, sizeof(buf_out));

        float poop = flash_intensity;
		float R, G, B, H, S, V;

		H = i++ % 360;
		S = 1;
		V = poop*0.5f;

		HSVtoRGB(R, G, B, H, S, V);
		led_color[0] = R*255.f;
		led_color[1] = G*255.f;
		led_color[2] = B*255.f;

        for (int i = 3*72; i <= 3*224; i+=3) {
			memcpy(buf_out+i, led_color, 3);
            //memset(buf_out+i, poop, 3);
        }
        write(fd, buf_out, sizeof(buf_out));


		// draw the graph, update & draw the lines
        //peak_data = win.visualize_graph(freqvec, {50,0.0025});
		peak_data = win.visualize_graph(freqvec, {5,0.002});

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

        read(fd, &buf_in, sizeof(buf_in));
	}

	close(fd);
	return 0;
}
