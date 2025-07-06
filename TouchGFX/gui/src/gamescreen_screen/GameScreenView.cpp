#include <gui/gamescreen_screen/GameScreenView.hpp>

#include <C:\HeNhung\MyApplication\STM32CubeIDE\Application\User\src\app.hpp>
#include <BitmapDatabase.hpp>
#include <cmsis_os2.h>
#include <cmsis_os.h>
#include <cstring>

// Khai báo biến extern và một số biến toàn cục khác
extern void gameTask(void *argument);

extern int currentRound;
extern bool isRoundTransition;
extern int enemyBulletSpeed;
extern const int ENEMY_BULLET_SPEED_MAX;
extern void resetGameObjectsForNextRound();
extern int spawnRate;

extern const int MAX_SPAWN_RATE;


osThreadId_t gameTaskHandle;
uint8_t hearts = 3;
bool shouldStopScreen;
extern osMessageQueueId_t Queue1Handle;
//extern osMessageQueueId_t Queue2Handle;
extern osMessageQueueId_t Queue3Handle;
extern osMessageQueueId_t Queue4Handle;
extern osMessageQueueId_t Queue5Handle;

GameScreenView::GameScreenView() {
	// Khởi tạo đối tượng Game và các thành phần đồ họa trên màn hình game
	gameInstance = Game();
	remove(menu_button);
	// menu_button.setVisible(false);
	remove(score_holder);
	remove(continue_round2);
	remove(round_2);
	// Prepare ship
	// Chuẩn bị hình ảnh cho tàu và thiết lập vị trí ban đầu
	shipImage.setBitmap(touchgfx::Bitmap(BITMAP_SHIP_MAIN_ID));
	shipImage.setXY(gameInstance.ship.coordinateX,
			gameInstance.ship.coordinateY);
	add(shipImage);

	// Chuẩn bị hình ảnh cho đạn của tàu và đạn của kẻ địch, thiết lập vị trí ban đầu và trạng thái ẩn
	for (int i = 0; i < MAX_BULLET; i++) {
		enemyBulletImage[i].setXY(321, 33);
		enemyBulletImage[i].setBitmap(
				touchgfx::Bitmap(BITMAP_ENEMY_BULLET_RED_ID));
		shipBulletImage[i].setXY(321, 33);
		shipBulletImage[i].setBitmap(touchgfx::Bitmap(BITMAP_BULLET_DOUBLE_ID));
	}

	// Chuẩn bị hình ảnh cho các kẻ địch, sử dụng chuyển đổi màu cho từng loại kẻ địch và thiết lập thời gian cập nhật hình ảnh
	for (int i = 0; i < MAX_ENEMY; i++) {
		switch (i % 3) {
		case 0:
			enemyImage[i].setBitmaps(BITMAP_ENEMY_GREEN_01_ID,
					BITMAP_ENEMY_GREEN_02_ID);
			break;
		case 1:
			enemyImage[i].setBitmaps(BITMAP_ENEMY_RED_01_ID,
					BITMAP_ENEMY_RED_02_ID);
			break;
		case 2:
			enemyImage[i].setBitmaps(BITMAP_ENEMY_YELLOW_01_ID,
					BITMAP_ENEMY_YELLOW_02_ID);
			break;
		default:
			break;
		}
		// thiết lập khoảng thời gian giữa 2 lần cập nhật hình ảnh
		enemyImage[i].setUpdateTicksInterval(20);
	}
}

void GameScreenView::setupScreen() {
	GameScreenViewBase::setupScreen();

	// Kết thúc task game trước khi bắt đầu màn hình game mới (đảm bảo không có task game nào đang chạy)
	osThreadTerminate(gameTaskHandle);
	osMessageQueueReset(Queue5Handle);
	const osThreadAttr_t gameTask_attributes = { .name = "gameTask", // Tên của task là "gameTask"
			.stack_size = 8192 * 2, // Kích thước của stack được cấp phát cho task là 8192 * 2 bytes
			.priority = (osPriority_t) osPriorityNormal, // Ưu tiên của task được đặt là osPriorityNormal (ưu tiên bình thường)
			};

	// Tạo một task mới có các thuộc tính đã được khai báo trước đó
	gameTaskHandle = osThreadNew(gameTask, NULL, &gameTask_attributes);
	// Reset tất cả trạng thái game về ban đầu
    currentRound = 1;
    isRoundTransition = false;
    shouldEndGame = false;
    shouldStopTask = false;
    shouldStopScreen = false;
    
    // Reset enemy bullet speed về mặc định
    enemyBulletSpeed = 5;
    spawnRate = 0;

	}

void GameScreenView::tearDownScreen() {
	GameScreenViewBase::tearDownScreen();
}

// Render game objects
void GameScreenView::handleTickEvent() {
	GameScreenViewBase::handleTickEvent();

	// display end game screen
	uint8_t stopFlag = 0;
	uint32_t count5 = osMessageQueueGetCount(Queue5Handle);
	// get latest message
	while (count5 > 0) {
		osMessageQueueGet(Queue5Handle, &stopFlag, NULL, 0);
		count5 --;
	}
	if (stopFlag == 1 && !shouldStopScreen) {
		add(menu_button);

		Unicode::snprintf(score_holderBuffer, SCORE_HOLDER_SIZE, "%d",
				gameInstance.score);
		add(score_holder);
		menu_button.invalidate();
		score_holder.invalidate();
		invalidate();
		shouldStopTask = true;
		shouldStopScreen = true;
		osThreadTerminate(gameTaskHandle);

		// Reset currentRound về 1 khi game over
        currentRound = 1;
        isRoundTransition = false;
	}

	if (stopFlag == 2 && !shouldStopScreen) {
		add(continue_round2);
		add(round_2);
		continue_round2.invalidate();
		round_2.invalidate();
		invalidate();
		shouldStopTask = true;
		shouldStopScreen = true;
		osThreadTerminate(gameTaskHandle); 
	}

	// Get input
	uint8_t res = 0;

	uint32_t count = osMessageQueueGetCount(Queue1Handle);
	if (count > 0) {
		osMessageQueueGet(Queue1Handle, &res, NULL, osWaitForever);
		if (res == 'R') {
			gameInstance.ship.updateVelocityX(gameInstance.ship.VELOCITY);
			shipImage.setBitmap(touchgfx::Bitmap(BITMAP_SHIP_RIGHT_ID));
			osMessageQueueReset(Queue1Handle);
		} else if (res == 'N') {
			gameInstance.ship.updateVelocityX(0);
			shipImage.setBitmap(touchgfx::Bitmap(BITMAP_SHIP_MAIN_ID));
		} else {
			gameInstance.ship.updateVelocityX(-gameInstance.ship.VELOCITY);
			shipImage.setBitmap(touchgfx::Bitmap(BITMAP_SHIP_LEFT_ID));
			osMessageQueueReset(Queue1Handle);
		}
	}

	uint32_t count3 = osMessageQueueGetCount(Queue3Handle);
	if (count3 > 0) {
		osMessageQueueGet(Queue3Handle, &res, NULL, osWaitForever);
		if (res == 'U') {
			gameInstance.ship.updateVelocityY(gameInstance.ship.VELOCITY);
			osMessageQueueReset(Queue4Handle);
		} else if (res == 'N') {
			gameInstance.ship.updateVelocityY(0);
		}
	}

	uint32_t count4 = osMessageQueueGetCount(Queue4Handle);
	if (count4 > 0) {
		osMessageQueueGet(Queue4Handle, &res, NULL, osWaitForever);
		if (res == 'D') {
			gameInstance.ship.updateVelocityY(-gameInstance.ship.VELOCITY);
			osMessageQueueReset(Queue3Handle);
		} else if (res == 'N') {
			gameInstance.ship.updateVelocityY(0);
		}
	}

	// update player position and sprite
	shipImage.moveTo(gameInstance.ship.coordinateX,
			gameInstance.ship.coordinateY);

	// render ship bullet
	for (int i = 0; i < MAX_BULLET; i++) {
		switch (shipBullet[i].displayStatus) {
		case IS_HIDDEN:
			break;
		case IS_SHOWN:
			shipBulletImage[i].moveTo(shipBullet[i].coordinateX,
					shipBullet[i].coordinateY);
			break;
		case SHOULD_SHOW:
			shipBulletImage[i].moveTo(shipBullet[i].coordinateX,
					shipBullet[i].coordinateY);
			add(shipBulletImage[i]);
			shipBullet[i].updateDisplayStatus(IS_SHOWN);
			break;
		case SHOULD_HIDE:
			remove(shipBulletImage[i]);
			shipBullet[i].updateDisplayStatus(IS_HIDDEN);
			break;
		default:
			break;
		}
	}

	// render enemy bullet
	for (int i = 0; i < MAX_BULLET; i++) {
		switch (enemyBullet[i].displayStatus) {
		case IS_HIDDEN:
			break;
		case IS_SHOWN:
			enemyBulletImage[i].moveTo(enemyBullet[i].coordinateX,
					enemyBullet[i].coordinateY);
			break;
		case SHOULD_SHOW:
			enemyBulletImage[i].moveTo(enemyBullet[i].coordinateX,
					enemyBullet[i].coordinateY);
			add(enemyBulletImage[i]);
			enemyBullet[i].updateDisplayStatus(IS_SHOWN);
			break;
		case SHOULD_HIDE:
			remove(enemyBulletImage[i]);
			enemyBullet[i].updateDisplayStatus(IS_HIDDEN);
			break;
		default:
			break;
		}
	}

	// render enemy
	for (int i = 0; i < MAX_ENEMY; i++) {
		switch (enemy[i].displayStatus) {
		case IS_SHOWN:
			enemyImage[i].moveTo(enemy[i].coordinateX, enemy[i].coordinateY);
			break;
		case SHOULD_SHOW:
			enemyImage[i].moveTo(enemy[i].coordinateX, enemy[i].coordinateY);
			enemyImage[i].startAnimation(false, true, true);
			add(enemyImage[i]);
			enemy[i].updateDisplayStatus(IS_SHOWN);
			break;
		case SHOULD_HIDE:
			enemyImage[i].stopAnimation();
			remove(enemyImage[i]);
			enemy[i].updateDisplayStatus(IS_HIDDEN);
			break;
		case IS_HIDDEN:
		default:
			break;
		}
	}

	// check if player is out of health and should end game
	if (gameInstance.ship.lives != hearts) {
		hearts = gameInstance.ship.lives;
		// reset hearts
		heart_03.setAlpha(80);
		heart_02.setAlpha(80);
		heart_01.setAlpha(80);
		if (hearts >= 1)
			heart_03.setAlpha(255);
		if (hearts >= 2)
			heart_02.setAlpha(255);
		if (hearts >= 3)
			heart_01.setAlpha(255);
		// If player is out of health
		if (hearts < 1) {
			invalidate();
		}
	}

	Unicode::snprintf(score_boardBuffer, SCORE_BOARD_SIZE, "%d",
			gameInstance.score);
	invalidate();
	
	if (continue_round2.getPressedState()) {
		// Sang round 2
		currentRound = 2;
		isRoundTransition = false;
		shouldEndGame = false;
		shouldStopTask = false;
		shouldStopScreen = false;

		// Reset trạng thái game cho round mới (không reset điểm và mạng)
		resetGameObjectsForNextRound();

		// Remove all enemy and bullet images from UI
		for (int i = 0; i < MAX_ENEMY; i++) {
			remove(enemyImage[i]);
		}
		for (int i = 0; i < MAX_BULLET; i++) {
			remove(shipBulletImage[i]);
			remove(enemyBulletImage[i]);
		}

		// Ẩn nút và label round 2
		remove(continue_round2);
		remove(round_2);
		continue_round2.invalidate();
		round_2.invalidate();
		invalidate();

		hearts = gameInstance.ship.lives; // Đảm bảo trái tim hiển thị đúng

		stopFlag = 0;
		spawnRate = 0;

		// Clear queue để tránh message cũ
        osMessageQueueReset(Queue5Handle);


		// Tạo lại task
		const osThreadAttr_t gameTask_attributes = {
			.name = "gameTask",
			.stack_size = 8192 * 2,
			.priority = (osPriority_t) osPriorityNormal
		};
		gameTaskHandle = osThreadNew(gameTask, NULL, &gameTask_attributes);

	}

}
