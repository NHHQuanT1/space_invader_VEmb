/*
 * Bullet.h
 *
 *  Created on: Aug 14, 2022
 *      Author: QuangLV
 */

#ifndef APPLICATION_USER_SRC_BULLET_HPP_
#define APPLICATION_USER_SRC_BULLET_HPP_
#include "Entity.hpp"

const uint8_t MAX_BULLET = 10;

class Bullet: public Entity {
public:
	Bullet();
	const int MOVE_RATE = 400;
	virtual ~Bullet();
	bool update(uint8_t dt);
};

#endif /* APPLICATION_USER_SRC_BULLET_HPP_ */
