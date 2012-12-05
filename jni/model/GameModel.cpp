#include <jni.h>  
#include <string.h>  
#include <android/log.h>  

#include "GameModel.h"
#include "model/PacManModel.h"
#include "model/GhostModel.h"
#include "model/PinkyModel.h"
#include "model/InkeyModel.h"
#include "model/ClydeModel.h"
#include "model/IGameOverHandler.h"
#include "data/MotionDirection.h"
#include "data/TileType.h"
#include "data/IntPosition.h"
#include "data/GameState.h"
  
#define DEBUG_TAG "NDK_GameModel"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,DEBUG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,DEBUG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,DEBUG_TAG,__VA_ARGS__)
  
namespace game{
    static const int MAX_LIVES = 3;
    static const int MAX_TABLETS = 240;
    
    static const int TABLET_SCORE = 10;
    static const int ENERGIZER_SCORE = 50;

    static const int GHOST_SCORE_LEVELS = 4;
    static const int GHOST_SCORE[GHOST_SCORE_LEVELS] = {200, 400, 800, 1600};

    GameModel::GameModel(IGameOverHandler* gameOverHandler) : gameOverHandler (gameOverHandler){
        PathArray fieldPathLocal = {
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x52,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x52,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x00,0x05,0x01,0x05,0x01,0x05,0x00,0x00,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x32,0x12,0x12,0x12,0x00,0x05,0x00,0x05,0x00,0x05,0x00,0x00,0x12,0x12,0x12,0x32,0x12,0x12,0x12,0x12,0x12,0x12,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x52,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x12,0x12,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x52,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00 },
            { 0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x32,0x00,0x00 },
            { 0x00,0x00,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x00,0x00 },
            { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }
        };
        memcpy( &fieldPath, &fieldPathLocal, sizeof(fieldPathLocal));
        
        pacManModel = new PacManModel( fieldPath, TileType::PACMAN_PATH );
        blinkyModel = new GhostModel(
			fieldPath,
			TileType::GHOST_PATH,
			TileType::GHOST_HOME,
			pacManModel,
			new IntPosition (14,11),
			new IntPosition (26,0),
			0
		);
        
        pinkyModel = new PinkyModel(
			fieldPath,
			TileType::GHOST_PATH,
			TileType::GHOST_HOME,
			pacManModel,
			new IntPosition (14,14),
			new IntPosition (3,0),
			0
		);
        
        inkeyModel = new InkeyModel(
			fieldPath,
			TileType::GHOST_PATH,
			TileType::GHOST_HOME,
			pacManModel,
			new IntPosition (12,14),
			new IntPosition (3,30),
			30,
            blinkyModel
		);
        
        clydeModel = new ClydeModel(
			fieldPath,
			TileType::GHOST_PATH,
			TileType::GHOST_HOME,
			pacManModel,
			new IntPosition (16,14),
			new IntPosition (3,30),
			60
		);
        
        resetGame(0L);
    }

    void GameModel::resetPersons() { 
        pacManModel->reset(gameTime);
		blinkyModel->reset(gameTime);
		pinkyModel->reset(gameTime);
		inkeyModel->reset(gameTime);
		clydeModel->reset(gameTime);
    }

    void GameModel::resetLevel(long time) {
        LOGD("resetLevel");
        for (int j = 0; j < FieldSize::HEIGHT; j++) {
			for (int i = 0; i < FieldSize::WIDTH; i++) {
				if ((fieldPath[j][i] & TileType::TABLET) != 0)
					tabletPath[j][i] = 1;
				else 
					tabletPath[j][i] = 0;
				
				if ((fieldPath[j][i] & TileType::ENERGIZER) != 0) {
					energizerPath[j][i] = 1;
					energizerCount ++;
				} else { 
					energizerPath[j][i] = 0;
				}
			}
		}
		
		gameTime = time;
		
		ghostsEaten = 0;
		tabletsEaten = 0;
		
		resetPersons();
		gameState = GameState::LEVEL_INTRO;
		enteredCurrentGameStateAt = gameTime;
    }
    
    void GameModel::resetGame(long time) {
        gameTime = time;
    
        level = 1;
		lives = MAX_LIVES;
		score = 0;
		
		resetLevel(time);
    }
    
    PacManModel* GameModel::getPacManModel() {
        return pacManModel;
    }
    
    GhostModel* GameModel::getBlinkyModel() {
        return blinkyModel;
    }
    
    GhostModel* GameModel::getPinkyModel() {
        return pinkyModel;
    }
    
    GhostModel* GameModel::getInkeyModel() {
        return inkeyModel;
    }
    
    GhostModel* GameModel::getClydeModel() {
        return clydeModel;
    }
    
    PathArray& GameModel::getTabletPath() {
        return tabletPath;
    }
    
    PathArray& GameModel::getEnergizerPath() {
        return energizerPath;
    }
    
    int GameModel::getEnergizerCount() {
        return energizerCount;
    }
    
    int GameModel::getScore() {
		return score;
	}

    int GameModel::getLives() {
		return lives;
	}

    GameState* GameModel::getGameState() {
		return gameState;
	}

	int GameModel::getLevel() {
		return level;
	}

    void GameModel::step(long time) {
        if (gameState == GameState::LEVEL_INTRO) {
			long delta = time - enteredCurrentGameStateAt;
			if (delta > gameState->getDuration()) {
				gameState = GameState::GAME;
				enteredCurrentGameStateAt = time;
			}
		} else if (gameState == GameState::LEVEL_COMPLETED) {
			long delta = time - enteredCurrentGameStateAt;
			if (delta > gameState->getDuration()) {
				level++;
				
				resetLevel(time);
				gameState = GameState::LEVEL_INTRO;
				enteredCurrentGameStateAt = time;
				
			}
		} else if (gameState == GameState::GAME_OVER) {
			long delta = time - enteredCurrentGameStateAt;
			if (delta > gameState->getDuration()) {
				gameState = GameState::LEVEL_INTRO;
				enteredCurrentGameStateAt = time;
				if (gameOverHandler != 0)
					gameOverHandler->handleGameOver();
			}
		} else {
			gameTime = time;
		}
		
		pacManModel->step(gameTime);
		blinkyModel->step(gameTime);
		pinkyModel->step(gameTime);
		inkeyModel->step(gameTime);
		clydeModel->step(gameTime);
		
		IntPosition* pacManPosition = pacManModel->getCurrentPosition();
		
		if (tabletPath[pacManPosition->getY()][pacManPosition->getX()] > 0) {
			tabletPath[pacManPosition->getY()][pacManPosition->getX()] = 0;
			score += TABLET_SCORE;
			tabletsEaten++;
			if(tabletsEaten >= MAX_TABLETS) {
				gameState = GameState::LEVEL_COMPLETED;
				enteredCurrentGameStateAt = gameTime;
			}
			blinkyModel->tabletEaten(gameTime, tabletsEaten);
			pinkyModel->tabletEaten(gameTime, tabletsEaten);
			inkeyModel->tabletEaten(gameTime, tabletsEaten);
			clydeModel->tabletEaten(gameTime, tabletsEaten);
		} else if (energizerPath[pacManPosition->getY()][pacManPosition->getX()] > 0) {
			energizerPath[pacManPosition->getY()][pacManPosition->getX()] = 0;
			ghostsEaten = 0;
			score += ENERGIZER_SCORE;
			
			blinkyModel->frighten(gameTime);
			pinkyModel->frighten(gameTime);
			inkeyModel->frighten(gameTime);
			clydeModel->frighten(gameTime);
		}
		
		checkGhostHit( pacManPosition, blinkyModel);
		checkGhostHit( pacManPosition, pinkyModel);
		checkGhostHit( pacManPosition, inkeyModel);
		checkGhostHit( pacManPosition, clydeModel);
    }
    
	void GameModel::checkGhostHit (IntPosition* pacManPosition, GhostModel* ghost) {
		if(!ghost->getIsReturning() && pacManPosition->equals(ghost->getCurrentPosition())) {
			if (ghost->getIsFrightened() ) {
				/**
				 * TODO: force ghost to return home
				 * show earned points in place
				 */
				ghost->eaten(GHOST_SCORE[ghostsEaten]);
				score += GHOST_SCORE[ghostsEaten];
				ghostsEaten++;
				if (ghostsEaten >= GHOST_SCORE_LEVELS) {
					ghostsEaten = GHOST_SCORE_LEVELS - 1;
				}

			} else if (!ghost->getIsFrightened()){
				lives--;
				if (lives < 0) {
					gameState = GameState::GAME_OVER;
					enteredCurrentGameStateAt = gameTime;
				} else {
					resetPersons();
				}
			}
		}
	}


    void GameModel::setMotionDirection( MotionDirection* md ) {
        pacManModel->setMotionDirection(md);	
    }
}
