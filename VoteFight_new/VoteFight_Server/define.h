#pragma once

#ifdef _UNICODE
#define tcout wcout
#define tcin  wcin
#else
#define tcout cout
#define tcin  cin
#endif

#define MAX_CLIENT_CAPACITY 3

#define EPSILON				    0.01f

#define FRAME_BUFFER_WIDTH		1920
#define FRAME_BUFFER_HEIGHT		1080 

#define TERRAIN_WIDTH			400
#define TERRAIN_HEIGHT			400
#define DEPTH_BUFFER_WIDTH	    2048
#define DEPTH_BUFFER_HEIGHT     2048

#define MAX_NPC_COUNT			15
#define MAX_LIGHTS              3
#define MAX_BONES				70
#define MAX_BGM_SOUNDS			4
#define MAX_SFX_SOUNDS			14

#define TEXTURE_MASK_ALBEDO_MAP 0x0001
#define TEXTURE_MASK_NORMAL_MAP 0x0002
#define TEXTURE_MASK_SHADOW_MAP 0x0004

#define DT						CTimeManager::GetInstance()->GetDeltaTime()

#define KEY_NONE(key)			CInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::NONE
#define KEY_TAP(key)			CInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::TAP
#define KEY_HOLD(key)			CInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::HOLD
#define KEY_AWAY(key)			CInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::AWAY
#define CURSOR					CInputManager::GetInstance()->GetCursor()
#define OLD_CURSOR				CInputManager::GetInstance()->GetOldCursor()

enum class SCENE_TYPE
{
	GAME,

	COUNT
};

enum class GROUP_TYPE
{
	STRUCTURE,
	PLAYER,
	SKYBOX,

	COUNT
};

enum class COMPONENT_TYPE
{
	STATE_MACHINE,
	RIGIDBODY,
	ANIMATOR,
	TRANSFORM,
	COLLIDER,
	SPRITE_RENDERER,

	COUNT
};

enum class CAMERA_TYPE
{
	MAIN,

	COUNT
};

enum class WEAPON_TYPE
{
	PUNCH,
	PISTOL,

	COUNT
};

enum class ANIMATION_TYPE
{
	NONE,
	LOOP,
	ONCE,
	ONCE_REVERSE,

	COUNT
};

enum class ANIMATION_CLIP_TYPE
{
	PLAYER_IDLE = 0,
	PLAYER_WALK_FORWARD_AND_BACK,
	PLAYER_WALK_LEFT,
	PLAYER_WALK_RIGHT,
	PLAYER_RUN_FORWARD,
	PLAYER_RUN_LEFT,
	PLAYER_RUN_RIGHT,
	PLAYER_PUNCH,
	PLAYER_PISTOL_IDLE,
	PLAYER_SHOOT,
	PLAYER_DIE,
	NPC_IDLE = 0,
	NPC_WALK_FORWARD,
	NPC_RUN_FORWARD,
	NPC_SHOOT,
	NPC_HIT,
	NPC_DIE,

	COUNT
};

enum class TRIGGER_TYPE
{
	OPEN_DOOR,
	OPEN_ELEC_PANEL,
	SIREN,
	OPEN_GATE,

	COUNT
};

enum class RENDER_TYPE
{
	STANDARD,

	COUNT
};

enum class LIGHT_TYPE
{
	POINT = 1,
	SPOT,
	DIRECTIONAL,

	COUNT
};

enum class ROOT_PARAMETER_TYPE
{
	GameFramework,
	CAMERA,
	GAME_SCENE,
	OBJECT,
	SPRITE,
	BONE_OFFSET,
	BONE_TRANSFORM,
	ALBEDO_MAP,
	NORMAL_MAP,
	CUBE_MAP,
	SHADOW_MAP,

	COUNT
};

enum class TEXTURE_TYPE
{
	ALBEDO_MAP,
	NORMAL_MAP,
	CUBE_MAP,
	SHADOW_MAP,

	COUNT
};

enum class POST_PROCESSING_TYPE
{
	NONE,
	FADE_IN,
	FADE_OUT,

	COUNT
};

enum class SOUND_TYPE
{
	// BGM
	TITLE_BGM,
	INGAME_BGM_1,
	INGAME_BGM_2,
	ENDING_BGM,

	// SFX
	BUTTON_OVER,
	OPEN_DOOR,
	OPEN_EP,
	OPEN_GATE,
	POWER_DOWN,
	BREATH,
	GRUNT_1,
	GRUNT_2,
	PISTOL_SHOT,
	PISTOL_EMPTY,
	SIREN,
	UNLOCK,
	GET_PISTOL,
	GET_KEY,
};