#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("MainScene.csb");

    addChild(rootNode);

	//Init turret and projectile
	_turret = (Sprite*)rootNode->getChildByName("PlayerTurret");

	_projectile = new Projectile();
	_projectile->sprite = (Sprite*)rootNode->getChildByName("Projectile");
	_projectile->speed = 15;
	_projectile->onScreen = false;
	_projectile->radius =_projectile->sprite->getContentSize().width / 2;
	_projectile->damage = 1;

	//Get screen size
	winSize = Director::sharedDirector()->getWinSize();

	//DmgPwrUp - need to move to own method
	initDamagePowerUp();

	//Health Powerup 
	initHealthPowerUp();

	//Enemies
	initEnemies();

	//Init player health
	_cacher->addSpriteFramesWithFile("res/Health.plist");
	_playerShip = (Sprite*)rootNode->getChildByName("PlayerShip");
	_shipHealthInt = 1;
	_shipHealth = Sprite::createWithSpriteFrameName("health_1.png");
	_shipHealth->setScale(2);
	_shipHealth->setAnchorPoint(Point(0, 0));
	_shipHealth->setPosition(winSize.width - (_shipHealth->getContentSize().width * _shipHealth->getScale()), 0);

	this->addChild(_shipHealth);
	
	//Damage up/double damage sprite
	_doubleDamage = Sprite::create("res/DoubleDamage.png");
	_doubleDamage->setPosition(-100, 300);
	this->addChild(_doubleDamage);

	//GAME OVER	
	initGameOverScreen();

	//SCORE
	_scoreLabel = Label::createWithTTF("Distance: 0", "res/Aller_Rg.ttf", 20);
	_scoreLabel->setPosition(0 + _scoreLabel->getContentSize().width/2, winSize.height-20);
	this->addChild(_scoreLabel);
	_scoreCounter = 0;
	_frameCounter = 0;

	//Close button
	_closeButton = Sprite::create("res/Cross.png");
	_closeButton->setPosition(winSize.width - _closeButton->getBoundingBox().size.width, winSize.height - _closeButton->getBoundingBox().size.height);
	this->addChild(_closeButton);

	//MUSIC
	_audio = SimpleAudioEngine::getInstance();
	_audio->playBackgroundMusic("res/BGMusic.mp3", true);

	//TOUCHES
	//Set up a touch listener.
	auto touchListener = EventListenerTouchOneByOne::create();

	//Set callbacks for our touch functions.
	touchListener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);

	//Add our touch listener to event listener list.
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	//background stuff
	_bg1 = (Sprite*)rootNode->getChildByName("Stars1");
	_bg2 = (Sprite*)rootNode->getChildByName("Stars2");

	_bg1->setAnchorPoint(Point(0, 0));
	_bg1->setPosition(Point(0, 0));

	_bg2->setAnchorPoint(Point(0, 0));
	_bg2->setPosition(Point(_bg1->boundingBox().size.width - 1, 0));

	_scrollSpeed = 10;

	this->scheduleUpdate();

    return true;
}

void HelloWorld::initHealthPowerUp()
{
	//HealthPwrUp - need to move to own method
	_healthPwrUp = new PowerUp();

	_healthPwrUp->sprite = Sprite::create();

	_cacher->addSpriteFramesWithFile("res/Heart.plist");

	// load all the animation frames into an array
	Vector<SpriteFrame*> frames;
	for (int i = 1; i <= 2; i++)
	{
		stringstream ss;
		ss << "heart_" << i << ".png";
		frames.pushBack(_cacher->getSpriteFrameByName(ss.str()));
	}

	//play the animation
	Animation* anim = Animation::createWithSpriteFrames(frames, 0.1f);
	_healthPwrUp->sprite->runAction(RepeatForever::create(Animate::create(anim)));
	_healthPwrUp->sprite->setPosition(600, 100);
	_healthPwrUp->sprite->setScale(2.5);
	_healthPwrUp->scale = 2.5;
	_healthPwrUp->onScreen = true;

	_healthPwrUp->counter = 0;

	this->addChild(_healthPwrUp->sprite);
}

void HelloWorld::initDamagePowerUp()
{
	_dmgPwrUp = new PowerUp();

	_cacher = SpriteFrameCache::getInstance();
	_cacher->addSpriteFramesWithFile("res/Damage.plist");

	_dmgPwrUp->sprite = Sprite::create();

	// load all the animation frames into an array
	Vector<SpriteFrame*> frames;
	for (int i = 1; i <= 2; i++)
	{
		stringstream ss;
		ss << "Damage_" << i << ".png";
		frames.pushBack(_cacher->getSpriteFrameByName(ss.str()));
	}

	// play the animation
	Animation* anim = Animation::createWithSpriteFrames(frames, 0.1f);
	_dmgPwrUp->sprite->runAction(RepeatForever::create(Animate::create(anim)));
	_dmgPwrUp->sprite->setPosition(100, 100);
	_dmgPwrUp->sprite->setScale(0.5);
	_dmgPwrUp->scale = 2;
	_dmgPwrUp->onScreen = false;

	_dmgPwrUp->counter = 0;

	this->addChild(_dmgPwrUp->sprite);
}

void HelloWorld::initEnemies()
{
	_cacher->addSpriteFramesWithFile("res/Explosion.plist");

	for (int i = 8; i < 10; i++)
	{
		_enemies[i] = new Enemy();
		_enemies[i]->sprite = Sprite::create();
		_enemies[i]->sprite->setSpriteFrame(_cacher->getSpriteFrameByName("Meteor.png"));
		_enemies[i]->damage = 3;
		_enemies[i]->speed = 0.7;
		_enemies[i]->originalHealth = 4;
		_enemies[i]->currentHealth = _enemies[i]->originalHealth;
		_enemies[i]->scale = 3;
		_enemies[i]->sprite->setScale(_enemies[i]->scale);
		_enemies[i]->radius = _enemies[i]->sprite->getContentSize().width / 2 * _enemies[i]->scale;
		_enemies[i]->sprite->runAction(RepeatForever::create(RotateBy::create(15.0f, 360.0f)));
		_enemies[i]->animationFrame = 0;
		_enemies[i]->animationCounter = 0;
		this->addChild(_enemies[i]->sprite);
	}

	for (int i = 5; i < 8; i++)
	{
		_enemies[i] = new Enemy();
		_enemies[i]->sprite = Sprite::create();
		_enemies[i]->sprite->setSpriteFrame(_cacher->getSpriteFrameByName("Meteor.png"));
		_enemies[i]->damage = 2;
		_enemies[i]->speed = 1;
		_enemies[i]->originalHealth = 2;
		_enemies[i]->currentHealth = _enemies[i]->originalHealth;
		_enemies[i]->scale = 2;
		_enemies[i]->sprite->setScale(_enemies[i]->scale);
		_enemies[i]->radius = _enemies[i]->sprite->getContentSize().width / 2 * _enemies[i]->scale;
		_enemies[i]->sprite->runAction(RepeatForever::create(RotateBy::create(10.0f, 360.0f)));
		_enemies[i]->animationFrame = 0;
		_enemies[i]->animationCounter = 0;
		this->addChild(_enemies[i]->sprite);
	}

	for (int i = 0; i < 5; i++)
	{
		_enemies[i] = new Enemy();
		_enemies[i]->sprite = Sprite::create();
		_enemies[i]->sprite->setSpriteFrame(_cacher->getSpriteFrameByName("Meteor.png"));
		_enemies[i]->damage = 1;
		_enemies[i]->speed = 1.4;
		_enemies[i]->originalHealth = 1;
		_enemies[i]->currentHealth = _enemies[i]->originalHealth;
		_enemies[i]->scale = 1;
		_enemies[i]->radius = _enemies[i]->sprite->getContentSize().width / 2 / _enemies[i]->scale;
		_enemies[i]->sprite->runAction(RepeatForever::create(RotateBy::create(5.0f, 360.0f)));
		_enemies[i]->animationFrame = 0;
		_enemies[i]->animationCounter = 0;
		this->addChild(_enemies[i]->sprite);		
	}
	
	_enemySpawn = 0;
	_enemySpawnDistance = 100;

	for (int i = 0; i < 10; i++)
	{
		setEnemySpawn(i);
	}
}

void HelloWorld::initGameOverScreen()
{
	_gameover = false;
	_blackScreen = Sprite::create("res/BlackScreen.jpg");
	_blackScreen->setAnchorPoint(Point(0, 0));
	_blackScreen->setScale((winSize.width / _blackScreen->getContentSize().width), 
		(winSize.height / _blackScreen->getContentSize().height));
	_blackScreen->setPosition(-winSize.width, -winSize.height);
	this->addChild(_blackScreen);	

	_restartButton = ui::Button::create("res/Default/Button_Disable.png", 
		"res/Default/Button_Press.png", "res/Default/Button_Normal.png");
	_restartButton->setTitleText("Restart");
	_restartButton->setScale(2);
	_restartButton->setPosition(Vec2(-300, -300));
	_restartButton->addTouchEventListener(CC_CALLBACK_0(HelloWorld::buttonPressed, this));
	this->addChild(_restartButton);
}

void HelloWorld::setEnemySpawn(int i)
{
	//It's too easy now, still needs tweaking
	_enemySpawnDistance += 100;
	_enemySpawn += 1;
	
	if (_enemySpawn == 5)
	{
		_enemySpawn = 1;
	}

	if (_enemySpawnDistance == 600)
	{
		_enemySpawnDistance = 100;
	}

	switch (_enemySpawn)
		{
		case 1:
			_enemies[i]->sprite->setPosition(rand() % (int)winSize.width + 0, -_enemySpawnDistance);
			break;
		case 2:
			_enemies[i]->sprite->setPosition(rand() % (int)winSize.width + 0, (int)winSize.height + _enemySpawnDistance);
			break;
		case 3:
			_enemies[i]->sprite->setPosition(-_enemySpawnDistance, rand() % (int)winSize.height + 0);
			break;
		case 4:
			_enemies[i]->sprite->setPosition((int)winSize.width + _enemySpawnDistance, rand() % (int)winSize.height + 0);
			break;
		}
	_enemies[i]->currentHealth = _enemies[i]->originalHealth;
	_enemies[i]->animationCounter = 0;
	_enemies[i]->animationFrame = 0;
	_enemies[i]->sprite->setSpriteFrame(_cacher->getSpriteFrameByName("Meteor.png"));
	_enemies[i]->sprite->resume();
}

void HelloWorld::update(float delta)
{
	if (!_gameover)
	{
		updateBackground();

		updateDMGPowerUp();

		updateHEALTHPowerUp();
	
		if (_projectile->onScreen)
		{
			updateProjectile();
		}

		updateEnemies();

		updatePlayerShip();

		updateScore();
	}
}

void HelloWorld::updateBackground()
{
	Point bg1Pos = _bg1->getPosition();
	Point bg2Pos = _bg2->getPosition();
	bg1Pos.x -= _scrollSpeed;
	bg2Pos.x -= _scrollSpeed;

	// move scrolling background back by one screen width to achieve "endless" scrolling
	if (bg1Pos.x < -(_bg1->getContentSize().width))
	{
		bg1Pos.x += _bg1->getContentSize().width;
		bg2Pos.x += _bg2->getContentSize().width;
	}

	// remove any inaccuracies by assigning only int values 
	// (prevents floating point rounding errors accumulating over time)
	bg1Pos.x = (int)bg1Pos.x;
	bg2Pos.x = (int)bg2Pos.x;
	_bg1->setPosition(bg1Pos);
	_bg2->setPosition(bg2Pos);
}

void HelloWorld::updateProjectile()
{	
	//checks if projectile is on screen
	if (_projectile->sprite->getPositionX() > winSize.width + _projectile->sprite->getContentSize().width || 
		_projectile->sprite->getPositionX() < 0 - _projectile->sprite->getContentSize().width ||
		_projectile->sprite->getPositionY() > winSize.height + _projectile->sprite->getContentSize().height ||
		_projectile->sprite->getPositionY() < 0 - _projectile->sprite->getContentSize().height)
	{
		_projectile->onScreen = false;
	}
	else
	{
		_projectile->onScreen = true;
		_projectile->temp.x += _projectile->vector.x * _projectile->speed;
		_projectile->temp.y += _projectile->vector.y * _projectile->speed;
		_projectile->sprite->setPosition(_projectile->temp.x, _projectile->temp.y);
	}
}

void HelloWorld::updateEnemies()
{
	for (int i = 0; i < 10; i++)
	{
		if (_enemies[i]->currentHealth > 0)
		{
			_enemies[i]->angle = ccpToAngle(ccpSub(_enemies[i]->sprite->getPosition(), _turret->getPosition()));
			_enemies[i]->vector = Point(-cos(_enemies[i]->angle), -sin(_enemies[i]->angle));
			_enemies[i]->temp = _enemies[i]->sprite->getPosition();
			_enemies[i]->temp.x += _enemies[i]->vector.x * _enemies[i]->speed;
			_enemies[i]->temp.y += _enemies[i]->vector.y * _enemies[i]->speed;
			_enemies[i]->sprite->setPosition(_enemies[i]->temp.x, _enemies[i]->temp.y);

			if (_projectile->onScreen)
			{
				//Check for enemy collisions
					if (_projectile->sprite->boundingBox().intersectsCircle(_enemies[i]->sprite->getPosition(), _enemies[i]->radius))
					{
						_projectile->sprite->setPosition(-100, -100);
						_enemies[i]->currentHealth -= _projectile->damage;
					}
			}
		}
	}

	for (int i = 0; i < 10; i++)
	{
		int speed = 4;

		if (_enemies[i]->currentHealth <= 0)
		{
			_enemies[i]->animationCounter += 1;
			_enemies[i]->sprite->pause();
		}

		if (_enemies[i]->animationCounter != 0 && _enemies[i]->animationCounter % speed == 0)
		{
			_enemies[i]->animationFrame += 1;
			stringstream ss;
			ss << "explosion_" << _enemies[i]->animationFrame << ".png";
			_enemies[i]->sprite->setSpriteFrame(_cacher->getSpriteFrameByName(ss.str()));
		}

		if (_enemies[i]->animationFrame >= 14)
		{
			setEnemySpawn(i);
		}
	}
}

void HelloWorld::updatePlayerShip()
{	if (_shipHealthInt >= 6)
	{
		_shipHealth->setSpriteFrame(_cacher->getSpriteFrameByName("health_6.png"));
		_blackScreen->setPosition(0,0);
		_gameover = true;
		_audio->stopBackgroundMusic();
		_restartButton->setPosition(Vec2(winSize.width / 2, 100));
		_audio->playEffect("GameOver.mp3", false, 1.0f, 1.0f, 1.0f);
	}
	for (int i = 0; i < 10; i++)
	{
		if (_playerShip->boundingBox().intersectsCircle(_enemies[i]->sprite->getPosition(), _enemies[i]->radius-20) && !(_enemies[i]->currentHealth <= 0))
		{
			_audio->playEffect("res/Hit.mp3", false, 1.0f, 1.0f, 0.5f);
			//setEnemySpawn(i);
			float duration = 0.07;
			_playerShip->runAction(Sequence::create(FadeTo::create(duration, 0), FadeTo::create(duration, 255), nullptr));
			_turret->runAction(Sequence::create(FadeTo::create(duration, 0), FadeTo::create(duration, 255), nullptr));
			_shipHealth->runAction(Sequence::create(FadeTo::create(duration, 0), FadeTo::create(duration, 255), nullptr));
			_enemies[i]->currentHealth = 0;
			_shipHealthInt += _enemies[i]->damage;	
			if (_shipHealthInt < 7)
			{
				stringstream ss;
				ss << "health_" << _shipHealthInt << ".png";
				_shipHealth->setSpriteFrame(_cacher->getSpriteFrameByName(ss.str()));
			}
		}
	}
}

void HelloWorld::updateScore()
{
	_frameCounter++;
	if (_frameCounter == 5)
	{
		_frameCounter = 0;
		_scoreCounter += 5;
		stringstream ss;
		ss << "Distance: " << _scoreCounter << "km";
		_scoreLabel->setString(ss.str());
		_scoreLabel->setPosition(0 + _scoreLabel->getContentSize().width / 2, winSize.height - 20);
	}
}

void HelloWorld::updateDMGPowerUp()
{
	//Maybe we should add a scale variable to some of the structs - that's what the last divide by 2 is for below
	_dmgPwrUp->radius = (_dmgPwrUp->sprite->getContentSize().width / 2) / _dmgPwrUp->scale; //Need to figure out how to fix this

	if (_projectile->sprite->getBoundingBox().intersectsCircle(_dmgPwrUp->sprite->getPosition(), _dmgPwrUp->radius)) //Need to move to own method
	{
		_dmgPwrUp->sprite->setPositionX(-100);
		_projectile->sprite->setPositionY(-100);
		_projectile->damage *= 2;
		_doubleDamage->setPosition(_shipHealth->getPositionX() - 40, _shipHealth->getPositionY() + 30);
	}

	if (_dmgPwrUp->sprite->getPositionX() > winSize.width + _dmgPwrUp->sprite->getContentSize().width ||
		_dmgPwrUp->sprite->getPositionX() < 0 - _dmgPwrUp->sprite->getContentSize().width ||
		_dmgPwrUp->sprite->getPositionY() > winSize.height + _dmgPwrUp->sprite->getContentSize().height ||
		_dmgPwrUp->sprite->getPositionY() < 0 - _dmgPwrUp->sprite->getContentSize().height)
	{
		_dmgPwrUp->onScreen = false;
		_dmgPwrUp->counter++;
		if (_dmgPwrUp->counter == 250)
		{
			_projectile->damage /= 2; 
			_doubleDamage->setPosition(-100, 300);
		}
		if (_dmgPwrUp->counter == 500)
		{
			_dmgPwrUp->sprite->setPosition(rand() % (int)winSize.width + 0, rand() % (int)winSize.height + 0);
			_dmgPwrUp->counter = 0;
		}
	}
	else
	{
		_dmgPwrUp->onScreen = true;
	}
}

void HelloWorld::updateHEALTHPowerUp()
{
	_healthPwrUp->radius = (_healthPwrUp->sprite->getContentSize().width / 2) * _healthPwrUp->scale; 

	if (_projectile->sprite->getBoundingBox().intersectsCircle(_healthPwrUp->sprite->getPosition(), _healthPwrUp->radius)) 
	{
		_healthPwrUp->sprite->setPositionX(-100);
		_projectile->sprite->setPositionY(-100);
		if (_shipHealthInt > 1)
		{
			_shipHealthInt -= 1;
			stringstream ss;
			ss << "health_" << _shipHealthInt << ".png";
			_shipHealth->setSpriteFrame(_cacher->getSpriteFrameByName(ss.str()));
		}
	}

	if (_healthPwrUp->sprite->getPositionX() > winSize.width + _healthPwrUp->sprite->getContentSize().width ||
		_healthPwrUp->sprite->getPositionX() < 0 - _healthPwrUp->sprite->getContentSize().width ||
		_healthPwrUp->sprite->getPositionY() > winSize.height + _healthPwrUp->sprite->getContentSize().height ||
		_healthPwrUp->sprite->getPositionY() < 0 - _healthPwrUp->sprite->getContentSize().height)
	{
		_healthPwrUp->onScreen = false;
		_healthPwrUp->counter++;
		if (_healthPwrUp->counter == 500)
		{
			_healthPwrUp->sprite->setPosition(rand() % (int)winSize.width + 0, rand() % (int)winSize.height + 0);
			_healthPwrUp->counter = 0;
		}
	}
	else
	{
		_healthPwrUp->onScreen = true;
	}
}

bool HelloWorld::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	return true;
}

void HelloWorld::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
	if (_closeButton->getBoundingBox().containsPoint(touch->getLocation()))
	{
		CCDirector::sharedDirector()->end();
	}
	//ccpsub gets angle between them
	_turretAngleRadians = ccpToAngle(ccpSub(_turret->getPosition(), touch->getLocation()));

	_turret->setRotation(180 - CC_RADIANS_TO_DEGREES(_turretAngleRadians));

	if (!_projectile->onScreen)
	{
		_projectile->vector = Point(-cos(_turretAngleRadians), -sin(_turretAngleRadians));

		_projectile->sprite->setPosition(_turret->getPosition().x, _turret->getPosition().y);
		_projectile->temp.setPoint(_turret->getPosition().x, _turret->getPosition().y);
		_audio->playEffect("Laser.mp3", false, 1.0f, 1.0f, 0.5f);
	}
	_projectile->onScreen = true;
}

void HelloWorld::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

void HelloWorld::onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event)
{

}

void HelloWorld::resetGame()
{
	//reset enemies
	for (int i = 0; i < 10; i++)
	{
		setEnemySpawn(i);
	}
	
	//reset shp/projectile
	_shipHealthInt = 1;
	_shipHealth->setSpriteFrame(_cacher->getSpriteFrameByName("health_1.png"));
	_projectile->sprite->setPosition(-200, -200);
	_projectile->onScreen = false;

	//reset score
	_scoreCounter = 0;

	//move game over stuff
	_blackScreen->setPosition(-winSize.width, -winSize.height);
	_restartButton->setPositionY(winSize.height + _restartButton->getContentSize().height * _restartButton->getScale());

	//restart music
	_audio->playBackgroundMusic("res/BGMusic.mp3", true);

	_audio->stopAllEffects();

	_gameover = false;
}

void HelloWorld::buttonPressed()
{
	resetGame();
}