#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <sstream>

using namespace cocos2d;
using namespace std;

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
	bool onScreen;
	Point vector;
	float speed;
	int damage;
	Point temp;
	int radius;
	int originalHealth;
	int currentHealth;
	int scale;
	float angle;
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

	void initHEALTHPowerUp();

	//Update Methods
	void update(float);

	void updateBackground();

	void updateProjectile();

	void updateEnemies();

	void setEnemySpawn(int i);

	void updatePlayerShip();

	void updateDMGPowerUp();

	void updateHEALTHPowerUp();

	//Deal with touches.
	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*);

private:
	SpriteFrameCache* cacher;
	Sprite* _bg1;
	Sprite* _bg2;
	Sprite* _turret;
	Sprite* _playerShip;
	Sprite* _shipHealth;
	Sprite* _doubleDamage;
	int _shipHealthInt;
	PowerUp* _dmgPwrUp;
	PowerUp* _healthPwrUp;
	Projectile* _projectile;
	Enemy* _enemies[10];
	int _enemySpawnDistance;
	int _enemySpawn;
	float _turretAngleRadians;
	Size winSize;
	int _scrollSpeed;
};

#endif // __HELLOWORLD_SCENE_H__
