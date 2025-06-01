/*
 * app.cpp
 *
 *  Created on: Aug 14, 2022
 *      Author: QuangLV
 */
#include "app.hpp"
#include <cmsis_os2.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "stack_macros.h"

Game gameInstance;
Bullet shipBullet[MAX_BULLET];
Bullet enemyBullet[MAX_BULLET];
Enemy enemy[MAX_ENEMY];
#define MAX_SPAWN_RATE 10000
int spawnRate = MAX_SPAWN_RATE;
int spawnSeed = 0;
uint16_t sx, sy;
bool shouldEndGame;
bool shouldStopTask;

void updateEnemy(uint8_t dt);
void updateShipBullet(uint8_t dt);
void updateEnemyBullet(uint8_t dt);

uint8_t dt = 0;
void gameTask(void *argument) {
	int loopCount = 0;
	for(int i=0; i<MAX_BULLET;i++) {
		shipBullet[i].updateCoordinate(-50, -50);
		enemyBullet[i].updateCoordinate(-50, -50);
		shipBullet[i].updateVelocity(0, -5);
		enemyBullet[i].updateVelocity(0, 5);
		shipBullet[i].updateDisplayStatus(IS_HIDDEN);
		enemyBullet[i].updateDisplayStatus(IS_HIDDEN);
	}

	for(int i=0; i<MAX_ENEMY;i++) {
		enemy[i].updateCoordinate(-50, -50);
		enemy[i].updateDisplayStatus(IS_HIDDEN);
	}

//	uint32_t previousTick = 0, currentTick = 0;
	for(;;) {
		loopCount++;
		if(shouldEndGame == true && shouldStopTask == true) break;
		else if(shouldEndGame) continue;

		if(loopCount >= 2) {
			dt = 1;
			loopCount = 0;
		} else {
			dt = 0;
		}
//		currentTick = xTaskGetTickCount();
//		dt = currentTick - previousTick;
//		dt = dt > 0 ? dt : 1;
//		previousTick = currentTick;
		/*  UPDATE POSITIONS */
		sx = gameInstance.ship.coordinateX;
		sy = gameInstance.ship.coordinateY;

		// update enemy
		updateEnemy(dt);

		// update ship position
		gameInstance.ship.update(dt);

		// update ship bullet position
		updateShipBullet(dt);

		// update enemy bullet position
		updateEnemyBullet(dt);


		/*  CHECK COLLISIONS */

		// check enemy and ship collision
		for(int i=0;i<MAX_ENEMY;i++) {
			if(enemy[i].displayStatus != IS_SHOWN) continue;
			if(Entity::isCollide(enemy[i], gameInstance.ship)) {
				gameInstance.ship.updateShipHp(-1);
				enemy[i].updateDisplayStatus(SHOULD_HIDE);
				gameInstance.ship.updateCoordinate(104, 260);
				break;
			}
		}

		// check enemy and ship bullet collision
		for(int i=0;i<MAX_ENEMY;i++) {
			if(enemy[i].displayStatus != IS_SHOWN) continue;
			for(int j=0;j<MAX_BULLET;j++) {
				if(shipBullet[j].displayStatus != IS_SHOWN) continue;
				if(Entity::isCollide(enemy[i], shipBullet[j])) {
					enemy[i].updateDisplayStatus(SHOULD_HIDE);
					shipBullet[j].updateDisplayStatus(SHOULD_HIDE);
					gameInstance.updateScore(5);=
					break;
				}
			}
		}

		// check ship and enemy bullet collision
		for(int i=0;i<MAX_BULLET;i++) {
			if(enemyBullet[i].displayStatus != IS_SHOWN) continue;
			if(Entity::isCollide(enemyBullet[i], gameInstance.ship)) {
				gameInstance.ship.updateShipHp(-1);
				enemyBullet[i].updateDisplayStatus(SHOULD_HIDE);
				gameInstance.ship.updateCoordinate(104, 260);
				break;
			}
		}

		if(gameInstance.ship.lives <= 0) shouldEndGame = true;
	}
}

void updateEnemy(uint8_t dt) {
	spawnRate -=dt;
	for(int i=0;i<MAX_ENEMY;i++) {
		if(enemy[i].displayStatus != IS_HIDDEN) {
			enemy[i].update(dt);
			continue;
		}

		if(spawnRate > 0) continue;

		uint16_t ex = 0;
		uint16_t ey = 0;
		uint16_t vx;
		uint16_t vy = 0;

		if(spawnSeed%2) {
			ex = -32;
			vx = 1;
		} else {
			ex = 240;
			vx = -1;
		}

		switch(spawnSeed) {
		case 0:
			ey = 42;
			break;
		case 1:
			ey = 80;
			break;
		case 2:
			ey = 120;
			break;
		case 3:
			ey = 160;
			break;
		default:
			break;
		}

		if(spawnSeed >= 3) {
			spawnSeed = 0;
		} else {
			spawnSeed++;
		}

		enemy[i].updateCoordinate(ex, ey);
		enemy[i].updateVelocity(vx, vy);
		enemy[i].updateDisplayStatus(SHOULD_SHOW);

		spawnRate = MAX_SPAWN_RATE;
	}
}

void updateShipBullet(uint8_t dt) {
	bool shouldFire = gameInstance.ship.updateBullet(dt);
	for(int i=0; i<MAX_BULLET;i++) {
			if(shipBullet[i].displayStatus != IS_HIDDEN) {
				shipBullet[i].update(dt);
				continue;
			}

			if(shouldFire) {
				shipBullet[i].updateCoordinate(sx, sy - 16);
				shipBullet[i].updateDisplayStatus(SHOULD_SHOW);
				shouldFire = false;
			}
		}
}

void updateEnemyBullet(uint8_t dt) {
	for(int j=0; j<MAX_ENEMY;j++) {
		if(enemy[j].displayStatus != IS_SHOWN) continue;
		if(enemy[j].updateBullet(dt) == false) continue;
		for(int i=0; i<MAX_BULLET;i++) {
			if(enemyBullet[i].displayStatus != IS_HIDDEN) continue;
			enemyBullet[i].updateCoordinate(enemy[j].coordinateX, enemy[j].coordinateY + 16);
			enemyBullet[i].updateDisplayStatus(SHOULD_SHOW);
			break;
		}
	}

	for(int i=0; i<MAX_BULLET;i++) {
		if(enemyBullet[i].displayStatus != IS_SHOWN) continue;
		enemyBullet[i].update(dt);
	}

}
