/*
 * app.hpp
 *
 *  Created on: Aug 14, 2022
 *      Author: QuangLV
 */

#ifndef APPLICATION_USER_SRC_APP_HPP_
#define APPLICATION_USER_SRC_APP_HPP_
#include "Game.hpp"
#include "Entity.hpp"

extern Game gameInstance;
extern bool shouldEndGame;
extern bool shouldStopTask;
extern Enemy enemy[MAX_ENEMY];
extern Bullet shipBullet[MAX_BULLET];
extern Bullet enemyBullet[MAX_BULLET];
extern void gameTask(void *argument);

#endif /* APPLICATION_USER_SRC_APP_HPP_ */
