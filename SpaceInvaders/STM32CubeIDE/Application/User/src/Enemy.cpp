/*
 * Enemy.cpp
 *
 *  Created on: Aug 14, 2022
 *      Author: QuangLV
 */

#include "Enemy.hpp"
Enemy::Enemy() {
	this->width = 32;
	this->height = 32;
	this->displayStatus = IS_HIDDEN;
	this->velocityX = 0;
	this->velocityY = 0;
  this->moveRate = MOVE_RATE;
  this->fireRate = FIRE_RATE;
}

Enemy::~Enemy() {

}

bool Enemy::update(uint8_t dt) {
	if(this->coordinateX < -this->width || this->coordinateX > 240 ||
			this->coordinateY < -this->height || this->coordinateX > 320) {
		this->displayStatus = SHOULD_HIDE;
		this->fireRate = FIRE_RATE;
		this->moveRate = MOVE_RATE;
		return false;
	}
	this->moveRate -= dt;
	if(this->moveRate > 0) return false;
	this->coordinateX += this->velocityX;
	this->coordinateY += this->velocityY;
	this->moveRate = MOVE_RATE;
	return true;
}

bool Enemy::updateBullet(uint8_t dt) {
	this->fireRate -= dt;
	if(this->fireRate > 0) return false;
	this->fireRate = FIRE_RATE;
	return true;
}

