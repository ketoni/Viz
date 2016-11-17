#ifndef DYNAMICLINE_HH
#define DYNAMICLINE_HH

#include <SFML/Graphics.hpp>

class DynamicLine : public sf::RectangleShape {

	public:
		
		template <typename... Args>
		DynamicLine(Args... args) : sf::RectangleShape(args...) {
			m_behavior = &DynamicLine::bounceAbove;
			setFillColor(sf::Color::White);
		}

		enum BehaviorType {
			BounceAbove,
			BounceBelow,
			Chase,
			Align
		};

		void setBehavior(BehaviorType);
		void setAcceleration(float);
		void latchTarget(sf::Vector2f const&);
		void react();

	protected:
		void align(sf::Vector2f const&);
		void chase(sf::Vector2f const&);
		void bounceAbove(sf::Vector2f const&);
		void bounceBelow(sf::Vector2f const&);
	
	private:
		sf::Vector2f m_pos;
		float m_speed = 0;
		float m_accel = 2.0;
		
		const sf::Vector2f* m_targetpos = nullptr;
		float m_distance = 0;

		void (DynamicLine::*m_behavior)(sf::Vector2f const&) = nullptr;

};

#endif
