#pragma once
#include "SFML/Graphics/Color.hpp"

namespace ecs {
	struct LivePixel {
		sf::Color colour;
		float offset;
	};

	struct Health {
		float health;
	};

	struct Weapon {
		int type;
		float damage;
	};
}
