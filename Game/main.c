#include <stdio.h>
#include <stdlib.h>
#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include <windows.h> 

#pragma region Define
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BPP 32
#define MAX_FPS 240

#define SCREEN_NAME "Wish Fighter"
#define GRAVITY 9.81

#ifdef DEBUG
	#define PRINT(x, text) if (!(x)) { printf("DEBUG: %s\n", text); }
	#define PRINT_M(text) printf("DEBUG: %s\n", text)
#else if RELEASE
    #define PRINT(x, text)
	#define PRINT_M(text)
#endif

#pragma endregion

#pragma region Struct
typedef struct MainData
{
	sfRenderWindow* renderWindow;
	sfClock* clock;
}MainData;

typedef struct Background
{
	sfTexture* texture;
	sfRectangleShape* rectangle;
}Background;

typedef struct Animation
{
	sfTexture* texture;
	sfSprite* sprite;
	int frameCount;
	float frameRate;
	int currentFrame;
	sfBool isLooping;
	sfBool isFinished;
}Animation;

typedef struct PlayerAnimation
{
	Animation idle;
	Animation closeHeavyPunch;
	Animation crouchingHeavyPunch;
	Animation crouchingLightKick;
	Animation crouchingLightPunch;
	Animation crouchingMediumKick;
	Animation heavyPunch;
	Animation lightPunch;
	Animation blocking;
	Animation closeHeavyKick;
	Animation crouching;
	Animation crouchingBlocking;
	Animation crouchingHeavyKick;
	Animation heavyKick;
	Animation lightKick;
	Animation mediumKick;
	Animation jumpingPunch;
	Animation walk;
	Animation walkLeft;
	Animation jump;
	Animation jumpingKick;
	Animation* currentAnimation;
}PlayerAnimation;

typedef struct Player
{
	PlayerAnimation animation;
	int x, y;
	unsigned int w, h;
	int speed;
	int dir;
	float velocityX, velocityY;
	sfBool isJumping;
	sfBool isFall;
	sfBool isAttacking;
	sfBool isCrouching;
	sfBool isBlocking;
	float health;
	float animationTime;
	int jumpForce;
	sfRectangleShape* healthBar;
	sfRectangleShape* healthBarBack;
} Player;

typedef struct Color
{
	sfColor blueGrey;
}Color;

typedef struct HUD
{
	sfFont* font;
	sfText* fpsText;
	sfTexture* texture;
	sfRectangleShape* rectangle;

	sfTexture* ryuWinTexture;
	sfRectangleShape* ryuWinRectangle;
	sfTexture* kenWinTexture;
	sfRectangleShape* kenWinRectangle;
}HUD;

typedef struct GameData
{
	Player player;
	Player player2;
	Color color;
	HUD hud;
	Background background;
	sfBool gameOver;
	sfBool ryuWin;
}GameData;
#pragma endregion

#pragma region Core Define
void Load(MainData* const _mainData, GameData* const _gameData);

void PollEvent(const sfRenderWindow* const _renderWindow, GameData* const _gameData);
void OnKeyPressed(sfKeyEvent _key, const sfRenderWindow* const _renderWindow, GameData* const _gameData);
void OnMouseButtonPressed(sfMouseButtonEvent _button);
void OnMouseMoved(void);

void Update(sfClock* _clock, GameData* const _gameData);
void Draw(sfRenderWindow* const _renderWindow, const GameData* const _gameData);
void Cleanup(MainData* const _mainData, GameData* const _gameData);
#pragma endregion

int Collision(GameData* const _gameData);

#pragma region Function Animation Define
void SetupAnimation(Animation* _anim, const char* _filepath, int _frameCount, float _frameRate, sfBool _isLooping);
void AnimateSprite(Animation* _anim, float const _dt);
sfBool AnimIsFinished(Animation* const _anim);
#pragma endregion

void LoadScreen(MainData* const _mainData);
void LoadBackground(Background* const _background);
void LoadHud(HUD* const _hud);

#pragma region Function Player Define
void LoadPlayer(Player* const _player);
void LoadPlayerAnimations(Player* const _player);
void PlayerOnKeyPressed(sfKeyEvent _key, Player* const _player);
void PlayerKeyUpdate(float _dt, Player* const _player);
void PlayerMouvementUpdate(float _dt, Player* const _player);
void PlayerUpdate(float _dt, Player* const _player);
void PlayerAnimationUpdate(float _dt, Player* const _player);
void CleanupPlayer(Player* const _player);
#pragma endregion

void Reset(GameData* _gameData);

void cleanupAnimation(Animation* animation);

void CleanupBackground(Background* const _background);

int main(void)
{
	MainData mainData = { 0 };
	GameData gameData = { 0 };
	Load(&mainData, &gameData);

	while (sfRenderWindow_isOpen(mainData.renderWindow))
	{
		PollEvent(mainData.renderWindow, &gameData);

		Update(mainData.clock, &gameData);

		Draw(mainData.renderWindow, &gameData);
	}

	Cleanup(&mainData, &gameData);

	return EXIT_SUCCESS;
}

#pragma region Core
void Load(MainData* const _mainData, GameData* const _gameData)
{
	LoadScreen(_mainData);

	// Chargement des joueurs et du HUD
	LoadPlayer(&_gameData->player);
	//LoadPlayer2(&_gameData->player2);
	LoadHud(&_gameData->hud);
	LoadBackground(&_gameData->background);
	_gameData->gameOver = sfFalse;
	_gameData->ryuWin = sfFalse;

	_gameData->color.blueGrey = sfColor_fromRGB(119, 136, 153);
	_mainData->clock = sfClock_create();

}

void PollEvent(const sfRenderWindow* const _renderWindow, GameData* const _gameData)
{
	sfEvent event;
	while (sfRenderWindow_pollEvent(_renderWindow, &event))
	{
		switch (event.type)
		{
		case sfEvtClosed:
			sfRenderWindow_close(_renderWindow);
			break;
		case sfEvtKeyPressed:
			OnKeyPressed(event.key, _renderWindow, _gameData);
			break;
		case sfEvtMouseButtonPressed:
			OnMouseButtonPressed(event.mouseButton);
			break;
		case sfEvtMouseMoved:
			OnMouseMoved();
			break;
		default:
			break;
		}
	}
}

void OnKeyPressed(sfKeyEvent _key, const sfRenderWindow* const _renderWindow, GameData* const _gameData)
{
	Player* _player = &_gameData->player;

	switch (_key.code)
	{
	case sfKeyEscape:
		sfRenderWindow_close(_renderWindow);
		break;
	case sfKeySpace:
		if (_gameData->gameOver)
		{
			Reset(_gameData);
		}
		break;

	default:
		break;
	}
	PlayerOnKeyPressed(_key, _player);
}

void OnMouseMoved(void)
{
}

void OnMouseButtonPressed(sfMouseButtonEvent _button)
{
	switch (_button.button)
	{
	case sfMouseLeft:
		printf("Left mouse button\n");
		break;
	default:
		printf("touche souris enfoncer\n");
		break;
	}
}

void Update(sfClock* _clock, GameData* const _gameData)
{
	float dt = sfTime_asSeconds(sfClock_restart(_clock));
	char buffer[40];
	snprintf(buffer, sizeof(buffer), "Fps: %.2f", 1 / dt);
	sfText_setString(_gameData->hud.fpsText, buffer);

	if (!_gameData->gameOver)
	{
		PlayerUpdate(dt, &_gameData->player);

		//if (_gameData->player.x < _gameData->player2.x)
		//{
		//	sfSprite_setScale(_gameData->player.sprite, (sfVector2f) { 4, 4 });
		//	sfSprite_setScale(_gameData->player2.sprite, (sfVector2f) { -4, 4 });
		//}
		//if (_gameData->player.x > _gameData->player2.x)
		//{
		//	sfSprite_setScale(_gameData->player.sprite, (sfVector2f) { -4, 4 });
		//	sfSprite_setScale(_gameData->player2.sprite, (sfVector2f) { 4, 4 });
		//}

		/*if (_gameData->player2.health <= 0)
		{
			_gameData->player2.health = 0;
			_gameData->gameOver = sfTrue;
			_gameData->ryuWin = sfTrue;
		}*/
		/*	if (_gameData->player.health <= 0)
			{
				_gameData->player.health = 0;
				_gameData->gameOver = sfTrue;
				_gameData->ryuWin = sfFalse;
			}
			sfVector2f healthSize = { _gameData->player.health * 4, 20 };
			sfVector2f healthSize2 = { _gameData->player2.health * 4, 20 };
			sfRectangleShape_setSize(_gameData->player.healthBar, healthSize);
			sfRectangleShape_setSize(_gameData->player2.healthBar, healthSize2);*/
	}
}

void Draw(sfRenderWindow* const _renderWindow, const GameData* const _gameData)
{
	sfRenderWindow_clear(_renderWindow, _gameData->color.blueGrey);

	sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->background.rectangle, NULL);

	sfRenderWindow_drawSprite(_renderWindow, _gameData->player.animation.currentAnimation->sprite, NULL);
	/*sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->player.healthBarBack, NULL);
	sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->player2.healthBarBack, NULL);*//*
	sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->player.healthBar, NULL);
	sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->player2.healthBar, NULL);*/

	sfRenderWindow_drawText(_renderWindow, _gameData->hud.fpsText, NULL);

	if (_gameData->gameOver)
	{
		sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->hud.rectangle, NULL);

		if (_gameData->ryuWin)
		{
			sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->hud.ryuWinRectangle, NULL);
		}
		else
		{
			sfRenderWindow_drawRectangleShape(_renderWindow, _gameData->hud.kenWinRectangle, NULL);
		}
	}

	// Affichage final de la fen�tre
	sfRenderWindow_display(_renderWindow);
}

void Cleanup(MainData* const _mainData, GameData* const _gameData)
{
	sfRenderWindow_destroy(_mainData->renderWindow);
	_mainData->renderWindow = NULL;

	sfClock_destroy(_mainData->clock);
	_mainData->clock = NULL;

	CleanupBackground(&_gameData->background);
	CleanupPlayer(&_gameData->player);
}

#pragma endregion

int Collision(GameData* const _gameData)
{
	/*sfFloatRect rect1 = sfSprite_getGlobalBounds(_gameData->player.sprite);
	sfFloatRect rect2 = sfSprite_getGlobalBounds(_gameData->player2.sprite);
	if (sfFloatRect_intersects(&rect1, &rect2, NULL))
	{
		return 1;
	}
	else
	{
		return 0;
	}*/
}

#pragma region Function Animation
void SetupAnimation(Animation* _anim, const char* _filepath, int _frameCount, float _frameRate, sfBool _isLooping)
{
	// Charger la texture
	//printf("%c\n", _filepath);
	_anim->texture = sfTexture_createFromFile(_filepath, NULL);
	_anim->sprite = sfSprite_create();
	sfSprite_setTexture(_anim->sprite, _anim->texture, sfTrue);

	// D�finir les attributs de l'animation
	_anim->frameCount = _frameCount;
	_anim->frameRate = _frameRate;
	_anim->currentFrame = 0;
	_anim->isLooping = _isLooping;
	_anim->isFinished = sfFalse;

	// Calculer la taille de la texture pour d�couper les frames
	sfIntRect rect;
	sfVector2u textureSize = sfTexture_getSize(_anim->texture);

	int frameWidth = textureSize.x / _frameCount;
	int frameHeight = textureSize.y;

	// Initialiser le rectangle de la texture pour le sprite
	rect.top = 0;
	rect.left = 0;
	rect.width = frameWidth;
	rect.height = frameHeight;

	sfSprite_setTextureRect(_anim->sprite, rect);

	sfVector2f origin = { (frameWidth / 2.0f) / _frameCount, frameHeight };
	sfSprite_setOrigin(_anim->sprite, origin);
}

void AnimateSprite(Animation* _anim, float const _dt)
{
	if (_anim != NULL)
	{
		sfIntRect area = sfSprite_getTextureRect(_anim->sprite);
		sfVector2u textureSize = sfTexture_getSize(_anim->texture);
		int frameWidth = textureSize.x / _anim->frameCount;

		if (!_anim->isFinished) {
			area.left += frameWidth;

			if (area.left >= _anim->frameCount * frameWidth) {
				if (_anim->isLooping) {
					area.left = 0;
				}
				else {
					area.left = 0;
					_anim->isFinished = sfTrue;
				}
			}

			sfSprite_setTextureRect(_anim->sprite, area);
		}
	}
}

sfBool AnimIsFinished(Animation* const _anim)
{
	return _anim->isFinished;
}
#pragma endregion

void LoadScreen(MainData* const _mainData)
{
	sfVideoMode videoMode = { SCREEN_WIDTH, SCREEN_HEIGHT, BPP };
	_mainData->renderWindow = sfRenderWindow_create(videoMode, SCREEN_NAME, sfClose, NULL);
	sfRenderWindow_setVerticalSyncEnabled(_mainData->renderWindow, sfFalse);
	sfRenderWindow_setFramerateLimit(_mainData->renderWindow, MAX_FPS);
}

void LoadBackground(Background* const _background)
{
	_background->texture = sfTexture_createFromFile("Assets/Sprites/Background.png", NULL);
	_background->rectangle = sfRectangleShape_create();
	sfVector2f size = { SCREEN_WIDTH, SCREEN_HEIGHT };
	sfRectangleShape_setSize(_background->rectangle, size);
	sfRectangleShape_setTexture(_background->rectangle, _background->texture, sfTrue);
}

void LoadHud(HUD* const _hud)
{
	_hud->font = sfFont_createFromFile("arial.ttf");
	_hud->fpsText = sfText_create();
	sfText_setCharacterSize(_hud->fpsText, 20);
	sfText_setFont(_hud->fpsText, _hud->font);
	sfVector2f textPosition = { 20, 20 };
	sfText_setPosition(_hud->fpsText, textPosition);

	_hud->texture = sfTexture_createFromFile("Assets/Sprites/GameOver.png", NULL);
	_hud->rectangle = sfRectangleShape_create();
	sfVector2u textureSize = sfTexture_getSize(_hud->texture);
	sfVector2f size = { textureSize.x, textureSize.y };
	sfRectangleShape_setSize(_hud->rectangle, size);
	sfRectangleShape_setTexture(_hud->rectangle, _hud->texture, sfTrue);

	sfRectangleShape_setOrigin(_hud->rectangle, (sfVector2f) { (float)textureSize.x / 2, (float)textureSize.y });
	sfRectangleShape_setPosition(_hud->rectangle, (sfVector2f) { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });

	_hud->ryuWinTexture = sfTexture_createFromFile("Assets/Sprites/RyuWin.png", NULL);
	_hud->ryuWinRectangle = sfRectangleShape_create();
	sfVector2u textureSize2 = sfTexture_getSize(_hud->ryuWinTexture);
	sfVector2f size2 = { textureSize2.x, textureSize2.y };
	sfRectangleShape_setSize(_hud->ryuWinRectangle, size2);
	sfRectangleShape_setTexture(_hud->ryuWinRectangle, _hud->ryuWinTexture, sfTrue);

	sfRectangleShape_setOrigin(_hud->ryuWinRectangle, (sfVector2f) { (float)textureSize2.x / 2, (float)textureSize2.y });
	sfRectangleShape_setPosition(_hud->ryuWinRectangle, (sfVector2f) { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + textureSize.y / 2 + textureSize2.y / 2 });

	_hud->kenWinTexture = sfTexture_createFromFile("Assets/Sprites/KenWin.png", NULL);
	_hud->kenWinRectangle = sfRectangleShape_create();
	sfVector2u textureSize1 = sfTexture_getSize(_hud->kenWinTexture);
	sfVector2f size1 = { textureSize1.x, textureSize1.y };
	sfRectangleShape_setSize(_hud->kenWinRectangle, size1);
	sfRectangleShape_setTexture(_hud->kenWinRectangle, _hud->kenWinTexture, sfTrue);

	sfRectangleShape_setOrigin(_hud->kenWinRectangle, (sfVector2f) { (float)textureSize1.x / 2, (float)textureSize1.y });
	sfRectangleShape_setPosition(_hud->kenWinRectangle, (sfVector2f) { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + textureSize.y / 2 + textureSize1.y });
}

#pragma region Function Player

void LoadPlayer(Player* const _player)
{
	PRINT_M("Hello, world!\n");
	PRINT(0 == 0, "Player is a null value.\n");
	_player->x = 120;
	_player->y = SCREEN_HEIGHT * 0.9f;
	_player->speed = 700;
	_player->jumpForce = 1200;
	_player->dir = 0;
	_player->velocityX = 0;
	_player->velocityY = 0;
	_player->isJumping = sfFalse;
	_player->isFall = sfFalse;
	_player->isAttacking = sfFalse;
	_player->isCrouching = sfFalse;
	_player->isBlocking = sfFalse;
	_player->health = 100;
	_player->animationTime = 0;

	LoadPlayerAnimations(_player);

	_player->animation.currentAnimation = &_player->animation.idle;
}

void LoadPlayerAnimations(Player* const _player)
{
	SetupAnimation(&_player->animation.idle, "Assets/Sprites/Ryu_idle.png", 4, 0.2f, sfTrue);
	SetupAnimation(&_player->animation.crouching, "Assets/Sprites/Ryu_Crouching.png", 1, 0.1f, sfFalse);
	SetupAnimation(&_player->animation.blocking, "Assets/Sprites/Ryu_Blocking.png", 1, 0.1f, sfFalse);
	SetupAnimation(&_player->animation.crouchingBlocking, "Assets/Sprites/Ryu_Crouching_Blocking.png", 1, 0.1f, sfFalse);

	SetupAnimation(&_player->animation.lightPunch, "Assets/Sprites/Ruy_Light_Punch.png", 3, 0.085f, sfFalse);
	SetupAnimation(&_player->animation.heavyPunch, "Assets/Sprites/Ruy_Heavy_Punch.png", 5, 0.085f, sfFalse);
	SetupAnimation(&_player->animation.lightKick, "Assets/Sprites/Ryu_Light_Kick.png", 5, 0.085f, sfFalse);
	SetupAnimation(&_player->animation.heavyKick, "Assets/Sprites/Ryu_Heavy_Kick.png", 5, 0.085f, sfFalse);

	SetupAnimation(&_player->animation.crouchingLightPunch, "Assets/Sprites/Ryu_Crouching_Light_Punch.png", 3, 0.085f, sfFalse);
	SetupAnimation(&_player->animation.crouchingHeavyPunch, "Assets/Sprites/Ryu_Crouching_Heavy_Punch.png", 4, 0.1f, sfFalse);
	SetupAnimation(&_player->animation.crouchingLightKick, "Assets/Sprites/Ryu_Crouching_Light_Kick.png", 3, 0.085f, sfFalse);
	SetupAnimation(&_player->animation.crouchingHeavyKick, "Assets/Sprites/Ryu_Crouching_Heavy_Kick.png", 5, 0.085f, sfFalse);

	SetupAnimation(&_player->animation.jump, "Assets/Sprites/Ryu_Jump.png", 2, 0.4f, sfFalse);
	SetupAnimation(&_player->animation.jumpingPunch, "Assets/Sprites/Ryu_Jump_Punch.png", 3, 0.11f, sfFalse);
	SetupAnimation(&_player->animation.jumpingKick, "Assets/Sprites/Ryu_Jump_Kick.png", 3, 0.11f, sfFalse);

	SetupAnimation(&_player->animation.walk, "Assets/Sprites/Ryu_walk.png", 2, 0.15f, sfTrue);
	SetupAnimation(&_player->animation.walkLeft, "Assets/Sprites/Ryu_walk_left.png", 2, 0.15f, sfTrue);
}

void PlayerOnKeyPressed(sfKeyEvent _key, Player* const _player)
{
	switch (_key.code)
	{
	case sfKeyZ:
		if (!_player->isAttacking) {
			if (!_player->isJumping)
			{
				_player->animation.jump.isFinished = sfFalse;
				_player->velocityY -= _player->jumpForce;
				_player->isJumping = sfTrue;
				_player->animation.currentAnimation = &_player->animation.jump;
				_player->animationTime = 0;
			}
		}
		break;
		// Coup de poing l�ger
	case sfKeyA:
		if (!_player->isAttacking)
		{
			_player->isAttacking = sfTrue;

			if (_player->isJumping)
			{
				_player->animation.currentAnimation = &_player->animation.jumpingPunch;
			}
			else if (_player->isCrouching)
			{
				_player->animation.currentAnimation = &_player->animation.crouchingLightPunch;
			}
			else
			{
				_player->animation.currentAnimation = &_player->animation.lightPunch;
			}
			_player->animationTime = 0;
		}
		break;

		// Coup de poing lourd
	case sfKeyR:
		if (!_player->isAttacking)
		{
			_player->isAttacking = sfTrue;

			if (_player->isJumping)
			{
				_player->animation.currentAnimation = &_player->animation.jumpingPunch;
			}
			else if (_player->isCrouching)
			{
				_player->animation.currentAnimation = &_player->animation.crouchingHeavyPunch;
			}
			else
			{
				_player->animation.currentAnimation = &_player->animation.heavyPunch;
			}
			_player->animationTime = 0;
		}
		break;

		// Coup de pied l�ger
	case sfKeyE:
		if (!_player->isAttacking)
		{
			_player->isAttacking = sfTrue;

			if (_player->isJumping)
			{
				_player->animation.currentAnimation = &_player->animation.jumpingKick;
			}
			else if (_player->isCrouching)
			{
				_player->animation.currentAnimation = &_player->animation.crouchingLightKick;
			}
			else
			{
				_player->animation.currentAnimation = &_player->animation.lightKick;
			}
			_player->animationTime = 0;
		}
		break;

		// Coup de pied lourd
	case sfKeyF:
		if (!_player->isAttacking)
		{
			_player->isAttacking = sfTrue;

			if (_player->isJumping)
			{
				_player->animation.currentAnimation = &_player->animation.jumpingKick;
			}
			else if (_player->isCrouching)
			{
				_player->animation.currentAnimation = &_player->animation.crouchingHeavyKick;
			}
			else
			{
				_player->animation.currentAnimation = &_player->animation.heavyKick;
			}

			_player->animationTime = 0;
		}
		break;
	default:
		break;
	}
}

void PlayerKeyUpdate(float _dt, Player* const _player)
{
	_player->dir = 0;

	if (!_player->isAttacking) {
		if (sfKeyboard_isKeyPressed(sfKeyQ))
		{
			_player->dir = -1;
		}
		else if (sfKeyboard_isKeyPressed(sfKeyD))
		{
			_player->dir = 1;
		}
	}

	if (sfKeyboard_isKeyPressed(sfKeyS) && !_player->isJumping && !_player->isAttacking) {
		_player->isCrouching = sfTrue;
	}
	else {
		_player->isCrouching = sfFalse;
	}
	if (sfKeyboard_isKeyPressed(sfKeyLShift) && !_player->isJumping && !_player->isAttacking)
	{
		_player->isBlocking = sfTrue;
	}
	else
	{
		_player->isBlocking = sfFalse;
	}
}

void PlayerMouvementUpdate(float _dt, Player* const _player)
{
	if (!_player->isAttacking && !_player->isCrouching && !_player->isBlocking)
	{
		_player->velocityX = _player->speed * _player->dir;
		_player->x += _player->velocityX * _dt;
	}


	_player->velocityY += GRAVITY;
	_player->y += _player->velocityY * _dt;

	if (_player->x < 0)
	{
		_player->x = 0;
	}
	else if (_player->x > SCREEN_WIDTH - _player->w)
	{
		_player->x = SCREEN_WIDTH - _player->w;
	}

	if (_player->y >= SCREEN_HEIGHT * 0.9f)
	{
		_player->y = SCREEN_HEIGHT * 0.9f;
		_player->velocityY = 0;
		_player->isJumping = sfFalse;
		_player->isFall = sfFalse;
	}

	if (_player->y < _player->h)
	{
		_player->y = _player->h;
		_player->velocityY = 0;
	}
}

void PlayerUpdate(float _dt, Player* const _player)
{
	PlayerKeyUpdate(_dt, _player);

	PlayerMouvementUpdate(_dt, _player);

	PlayerAnimationUpdate(_dt, _player);

	sfVector2f position = { _player->x, _player->y };
	sfSprite_setPosition(_player->animation.currentAnimation->sprite, position);
}

void PlayerAnimationUpdate(float _dt, Player* const _player)
{
	if (!_player->isAttacking && !_player->isJumping && !_player->isCrouching && !_player->isBlocking)
	{
		if (_player->dir != 0)
		{
			if (_player->dir == 1)
			{
				_player->animation.currentAnimation = &_player->animation.walk;
			}
			else
			{
				_player->animation.currentAnimation = &_player->animation.walkLeft;
			}
		}
		else
		{
			_player->animation.currentAnimation = &_player->animation.idle;
		}
	}
	else if (_player->isAttacking)
	{
		if (AnimIsFinished(_player->animation.currentAnimation))
		{
			_player->animation.currentAnimation->isFinished = sfFalse;
			_player->isAttacking = sfFalse;
			_player->animation.currentAnimation = &_player->animation.idle;
		}
	}
	else if (_player->isJumping)
	{
		sfVector2u textureSize = sfTexture_getSize(_player->animation.jump.texture);
		if (!_player->isFall)
		{
			if (_player->velocityY < 0)
			{
				sfSprite_setTextureRect(_player->animation.jump.sprite,
					(sfIntRect) {
					0, 0, textureSize.x / _player->animation.jump.frameCount, textureSize.y
				});
			}
		}
		if (_player->velocityY > -20)
		{
			sfSprite_setTextureRect(_player->animation.jump.sprite,
				(sfIntRect) {
				textureSize.x / _player->animation.jump.frameCount, 0, textureSize.x / _player->animation.jump.frameCount, textureSize.y
			});
			_player->isFall = sfTrue;
		}

		_player->animation.currentAnimation = &_player->animation.jump;
	}
	else if (_player->isCrouching)
	{
		if (_player->isBlocking)
		{
			_player->animation.currentAnimation = &_player->animation.crouchingBlocking;
		}
		else
		{
			_player->animation.currentAnimation = &_player->animation.crouching;
		}
	}
	else if (_player->isBlocking)
	{
		_player->animation.currentAnimation = &_player->animation.blocking;
	}


	_player->animationTime += _dt;
	if (_player->animationTime >= _player->animation.currentAnimation->frameRate)
	{
		AnimateSprite(_player->animation.currentAnimation, _dt);
		_player->animationTime -= _player->animation.currentAnimation->frameRate;
	}
}

void CleanupPlayer(Player* const _player)
{
	if (!_player) return;

	// Nettoyer toutes les animations
	cleanupAnimation(&_player->animation.idle);
	cleanupAnimation(&_player->animation.closeHeavyPunch);
	cleanupAnimation(&_player->animation.crouchingHeavyPunch);
	cleanupAnimation(&_player->animation.crouchingLightKick);
	cleanupAnimation(&_player->animation.crouchingLightPunch);
	cleanupAnimation(&_player->animation.crouchingMediumKick);
	cleanupAnimation(&_player->animation.heavyPunch);
	cleanupAnimation(&_player->animation.lightPunch);
	cleanupAnimation(&_player->animation.blocking);
	cleanupAnimation(&_player->animation.closeHeavyKick);
	cleanupAnimation(&_player->animation.crouching);
	cleanupAnimation(&_player->animation.crouchingBlocking);
	cleanupAnimation(&_player->animation.crouchingHeavyKick);
	cleanupAnimation(&_player->animation.heavyKick);
	cleanupAnimation(&_player->animation.lightKick);
	cleanupAnimation(&_player->animation.mediumKick);
	cleanupAnimation(&_player->animation.jumpingPunch);
	cleanupAnimation(&_player->animation.walk);
	cleanupAnimation(&_player->animation.walkLeft);
	cleanupAnimation(&_player->animation.jump);
	cleanupAnimation(&_player->animation.jumpingKick);

	// Nettoyer les barres de sant�
	if (_player->healthBar) {
		sfRectangleShape_destroy(_player->healthBar);
		_player->healthBar = NULL;
	}
	if (_player->healthBarBack) {
		sfRectangleShape_destroy(_player->healthBarBack);
		_player->healthBarBack = NULL;
	}
}

#pragma endregion

void Reset(GameData* _gameData)
{
	//_gameData = { 0 };
	LoadPlayer(&_gameData->player);
	LoadHud(&_gameData->hud);
	LoadBackground(&_gameData->background);
	_gameData->gameOver = sfFalse;
	_gameData->ryuWin = sfFalse;

	_gameData->color.blueGrey = sfColor_fromRGB(119, 136, 153);
}


void cleanupAnimation(Animation* animation) {
	if (animation->texture) {
		sfTexture_destroy(animation->texture);
		animation->texture = NULL;
	}
	if (animation->sprite) {
		sfSprite_destroy(animation->sprite);
		animation->sprite = NULL;
	}
}

void CleanupBackground(Background* const _background)
{
	sfTexture_destroy(_background->texture);
	_background->texture = NULL;

	sfRectangleShape_destroy(_background->rectangle);
	_background->rectangle = NULL;
}