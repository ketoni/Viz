
#include <iostream>

#include "win.hpp"
#include "math.hpp"

Window::Window() : sf::RenderWindow(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Subtex") {
	setFramerateLimit(50);
	setVerticalSyncEnabled(true);
}

void Window::update() {
	parseEvents();	
	display();
	pollEvents();
}


void Window::flash(float intensity, sf::Color c) {
	float supressor = 1 * intensity;
	clear(sf::Color(c.r * supressor, c.g * supressor, c.b * supressor, 255));
}


PointData Window::visualize_graph(std::vector<std::complex<NTYPE>>& values, sf::Vector2f zoom) {

	size_t point_count;
	if (zoom.x < 100.0) {
		point_count = (float) values.size() / (100.0 / zoom.x);
	} else {
		point_count = values.size();
	}
		
	float point_interval = (float) getSize().x / (point_count-1);
	float vert_scaler = zoom.y / 100.0;
	float vert_bound = getSize().y;
	float mag = 0.0;
	float max_mag = 0.0;
	unsigned index_max = 0;
	
	sf::VertexArray graph(sf::LinesStrip, point_count);
	
	for (size_t i = 0; i < point_count; i++) {
		mag = math::magnitude(values[i]);
		if (mag > max_mag) {
			max_mag = mag;
			index_max = i;
		}
		graph[i].position = {point_interval * i, vert_bound - mag * vert_scaler};
	}
	draw(graph);
	PointData foo = {max_mag, graph[index_max].position};
	return foo;
}


bool Window::hasEvents() {
	return !m_events.empty();
}

void Window::pollEvents() {
	sf::Event event;
	while (pollEvent(event)) {
		m_events.push(event);
	}
}

void Window::parseEvents() {

	while (hasEvents()) {
		switch (m_events.front().type) {
			
			case (sf::Event::Closed):
				close();
				break;

			default:
				//std::cerr << "Unhandled event, type '" << m_events.front().type << "'\n";
				break;
		}
		m_events.pop();
	}	
}

