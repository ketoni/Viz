
#include <iostream>

#include "dynamicline.hpp"

void DynamicLine::setBehavior(BehaviorType type) {
	switch(type) {
		case BounceAbove:
			m_behavior = &DynamicLine::bounceAbove;
			break;
		case BounceBelow:
			m_behavior = &DynamicLine::bounceBelow;
			break;
		case Chase:
			m_behavior = &DynamicLine::chase;
			break;
		case Align:
			m_behavior = &DynamicLine::align;
			break;
	}
}


void DynamicLine::setAcceleration(float accel) {
	m_accel = accel;
}

void DynamicLine::latchTarget(sf::Vector2f const& target_pos) {
	m_targetpos = &target_pos;
}

void DynamicLine::react() {
	m_pos = getPosition();
	if (m_targetpos != nullptr && m_behavior != nullptr) {
		(this->*m_behavior)(*m_targetpos);
	}
}

void DynamicLine::align(sf::Vector2f const& targetpos) {
	setPosition({m_pos.x, targetpos.y});
	m_speed = 0.0;
}

void DynamicLine::chase(sf::Vector2f const& targetpos) {
	if (targetpos.y < m_pos.y) {
		setPosition(m_pos.x, m_pos.y - m_speed);
	} else {
		setPosition(m_pos.x, m_pos.y + m_speed);
	}
	m_speed += m_accel;
}

void DynamicLine::bounceAbove(sf::Vector2f const& targetpos) {
	if (targetpos.y > m_pos.y) {
		chase(targetpos);
	} else {
		align(targetpos);
	}
}

void DynamicLine::bounceBelow(sf::Vector2f const& targetpos) {
	if (targetpos.y < m_pos.y) {
		chase(targetpos);
	} else {
		align(targetpos);
	}
}

