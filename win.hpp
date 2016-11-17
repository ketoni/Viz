#ifndef WINDOW_HPP
#define WINDOW_HPP

#define WIN_WIDTH 1200
#define WIN_HEIGHT 600

#define NTYPE float

#include <queue>
#include <complex>

#include "dynamicline.hpp"


struct PointData {
	float magnitude;
	sf::Vector2f window_pos;
};


class Window : public sf::RenderWindow {

	public:
		Window();

		void update();
		void flash(float, sf::Color = sf::Color::White);

		PointData visualize_graph(std::vector<std::complex<NTYPE>>&, sf::Vector2f = {100,100});
		float magnitude(std::complex<NTYPE>&, bool = false);

	private:
		bool hasEvents();
		void pollEvents();
		void parseEvents();

		std::queue<sf::Event> m_events;
};


#endif
