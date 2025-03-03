#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <sstream>
#include "SimpleAudioEngine.h"  
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace std;
using namespace CocosDenshion;

struct Projectile
{
	Sprite* sprite;
	bool onScreen;
	Point vector;
	int speed;
	int damage;
	Point temp;
	int radius;
	int scale;
};

struct PowerUp
{
	Sprite* sprite;
	bool onScreen;
	int counter;
	float effect;
	int radius;
	int scale;
};

struct Enemy
{
	Sprite* sprite;
	Point vector;
	float speed;
	int damage;
	Point temp;
	int radius;
	int originalHealth;
	int currentHealth;
	int scale;
	float angle;
	int animationFrame;
	int animationCounter;
};

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	//Init Methods - planned on refactoring more code into seperate init methods.
	void initEnemies();

	void initHealthPowerUp();

	void initDamagePowerUp();

	void initGameOverScreen();

	//Update Methods
	void update(float);

	void updateBackground();

	void updateProjectile();

	void updateEnemies();

	void setEnemySpawn(int i);

	void updatePlayerShip();

	void updateScore();

	void updateDMGPowerUp();

	void updateHEALTHPowerUp();

	void resetGame();

	//Deal with touches.
	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);

	void buttonPressed();

private:
	//Sprites and animation
	SpriteFrameCache* _cacher;

	//Backgrounds
	Sprite* _bg1;
	Sprite* _bg2;
	Size winSize;
	int _scrollSpeed;
	Sprite* _closeButton;

	//Player ship and turret
	float _turretAngleRadians;
	Sprite* _turret;
	Projectile* _projectile;
	Sprite* _playerShip;
	Sprite* _shipHealth;
	int _shipHealthInt;

	//Score
	Label* _scoreLabel;
	int _scoreCounter;
	int _frameCounter;

	//Powerups
	Sprite* _doubleDamage;
	PowerUp* _dmgPwrUp;
	PowerUp* _healthPwrUp;

	//Restart Screen
	Sprite* _blackScreen;
	ui::Button* _restartButton;
	bool _gameover;

	//Enemy
	Enemy* _enemies[10];
	int _enemySpawnDistance;
	int _enemySpawn;

	//Audio
	SimpleAudioEngine* _audio;
};

#endif // __HELLOWORLD_SCENE_H__
