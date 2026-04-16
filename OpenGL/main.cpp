#include <Windows.h>	
#include <gl/GL.h>		
#include <math.h>
#include <gl/GLU.h>
#include <mmsystem.h> // for PlaySound

#pragma comment (lib, "OpenGL32.lib") 
#pragma comment (lib, "GLU32.lib") 
#pragma comment (lib, "Winmm.lib")  // for PlaySound

#define CLASS_TITLE "OpenGL Window Class"
#define WINDOW_TITLE "Jiang Shi" 

// ===== Global App State =====
int questionNum = 1;
float movementSpeed = 0.1f;
float rotSpeed = 5.0f;

// ===== Character Transform =====
float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
float characterFacingY = 0.0f;

// ===== Camera Defaults =====
const float START_CAM_X = 0.0f;
const float START_CAM_Y = 0.8f;
const float START_CAM_Z = 3.2f;
const float START_CAM_ROT_X = 0.0f;
const float START_CAM_ROT_Y = 0.0f;
const float START_CAM_ROT_Z = 0.0f;

// ===== Camera Runtime State =====
float camX = START_CAM_X;
float camY = START_CAM_Y;
float camZ = START_CAM_Z;
float camRotX = START_CAM_ROT_X;
float camRotY = START_CAM_ROT_Y;
float camRotZ = START_CAM_ROT_Z;

bool isPerspective = true;
bool isLightMovementMode = false;
bool showLightVisualizer = false;
int windowWidth = 800;
int windowHeight = 800;

// ===== OpenGL Draw Helpers =====
GLUquadricObj* sphere = gluNewQuadric();
GLUquadricObj* cylinder = gluNewQuadric();
GLUquadricObj* disk = gluNewQuadric();
int slices = 8;
int stacks = 8;

// ===== Color Palette =====
const GLfloat COLOR_MAROON[] = { 0.5f, 0.0f, 0.0f };
const GLfloat COLOR_DARK_PURPLE[] = { 0.145f, 0.016f, 0.29f };
const GLfloat COLOR_DARK_PLUM[] = { 0.1f, 0.0f, 0.12f };
const GLfloat COLOR_BABY_BLUE[] = { 0.592, 0.753, 0.922 };
const GLfloat COLOR_ORANGE[] = { 0.91, 0.667, 0.412 };
const GLfloat COLOR_GREY[] = { 0.831, 0.831, 0.831 };
const GLfloat COLOR_GOLD[] = { 1.0f, 0.84f, 0.0f };
const GLfloat COLOR_SKIN_BEIGE[] = { 0.94f, 0.82f, 0.72f };
const GLfloat COLOR_SKIN_BROWN[] = { 0.65f, 0.45f, 0.32f };
const GLfloat COLOR_CHARCOAL[] = { 0.18f, 0.18f, 0.18f };
const GLfloat COLOR_WHITE[] = { 1.0f, 1.0f, 1.0f };
const GLfloat COLOR_GREEN[] = { 0.133f, 0.196f, 0.050f };
const GLfloat COLOR_BROWN[] = { 0.341f, 0.247f, 0.0f };

// ===== Control Mode =====
enum ControlMode
{
	CONTROL_CAMERA = 0,
	CONTROL_LIGHT1 = 1,
	CONTROL_LIGHT2 = 2,
	CONTROL_LIGHT3 = 3
};

int controlMode = CONTROL_CAMERA;

// ===== Light State =====
float lightX = 0.9f;
float lightY = 1.8f;
float lightZ = 0.4f;
float light2X = -1.3f;
float light2Y = 0.3f;
float light2Z = -0.4f;
float light3X = -2.0f;
float light3Y = 3.0f;
float light3Z = 1.0f;
bool isLightOn = true;
bool isSecondLightOn = true;

// lighting
GLfloat ambientLightColour[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLightColour1[] = { 0.851, 0, 0.91, 1.0f };
GLfloat diffuseLightColour2[] = { 0.247, 0.91, 0, 1.0f };
GLfloat diffuseLightColour3[] = { 0.91, 0.494, 0.027, 1.0f };
GLfloat diffuseLightPosition[] = { lightX, lightY, lightZ, 1.0f };
GLfloat diffuseLightPosition2[] = { light2X, light2Y, light2Z, 1.0f };
GLfloat diffuseLightPosition3[] = { light3X, light3Y, light3Z, 1.0f };

GLfloat matAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat matDiffuse[] = { 0.3f, 0.5f, 0.0f, 1.0f };

// ===== Texture Handles =====
GLuint T_talisman = 0;
GLuint T_fabric = 0;
GLuint T_embroidery = 0;
GLuint T_embroidery2 = 0;
GLuint T_embroidery3 = 0;
GLuint T_dragon = 0;
GLuint T_concrete = 0;
GLuint T_wood = 0;
GLuint T_bricks = 0;
GLuint T_copper = 0;
GLuint T_copper2 = 0;
GLuint T_spiral = 0;
GLuint T_beam = 0;
int embroideryTextureIndex = 0;

// ===== Audio (SE) =====
enum SoundEffectId
{
	SE_NONE = 0,
	SE_JUMP = 1,
	SE_BELL = 2,
	SE_SUMMON = 3,
	SE_LAND = 4,
	SE_TWEAK = 5,
	SE_SELECT = 6,
	SE_COUNT = 6
};

const char* seFilePath[SE_COUNT + 1] =
{
	NULL,
  "..\\SE\\jump.wav",
	"..\\SE\\bell.wav",
   "..\\SE\\summon.wav",
	"..\\SE\\land.wav",
	"..\\SE\\tweak2.wav",
	"..\\SE\\select.wav"
};

bool isAudioInitialized = false;
const char* BGM_FILE_PATH = "..\\SE\\bgm.mp3";
bool isBgmPlaying = false;

// ===== Bitmap Loading =====
BITMAP BMP;
HBITMAP hBMP = NULL;

// ===== Movement Input =====
bool moveForward = false;
bool moveBackward = false;
bool moveLeft = false;
bool moveRight = false;

// ===== Animation / Character State =====
bool showBell = false;
bool showKeybindOverlay = false;
bool isSummonActive = false;
bool isDelayActive = false;
bool hasBellPeakSoundPlayed = false;
bool jumpRequested = false;
bool isJumping = false;
bool isJumpPoseToggle = false;
bool isWalking = false;
float jumpOffsetY = 0.0f;
float jumpVelocity = 0.0f;
float walkPhase = 0.0f;
float ponytailSwingPhase = 0.0f;
float ponytailSwingAngle = 0.0f;
float shoulderAngle = 0.0f;
float elbowAngle = 0.0f;
float hipAngle = 0.0f;
float kneeAngle = 0.0f;
float headAngle = 0.0f;

// ===== Timing / Physics Constants =====
const DWORD SUMMON_DELAY_MS = 1300;
DWORD delayStartTick = 0;

const float JUMP_STEP = 0.006f;
const float JUMP_START_VELOCITY = 0.02f; // higher = jumps higher / longer
const float GRAVITY = -0.0004f; // more negative = falls faster
const float JUMP_POSE_PREVIEW_Y = 0.20f;
const float PONYTAIL_SWING_SPEED = 0.2f;
const float PONYTAIL_SWING_MAX_ANGLE = 180;

// ===== Summon Coffin Animation =====
const float SUMMON_COFFIN_START_Y = -1.0f;
const float SUMMON_COFFIN_TARGET_Y = 0.0f;
const float SUMMON_COFFIN_RISE_SPEED = 0.003f;
float summonedCoffinLiftY = SUMMON_COFFIN_START_Y;

const int TALISMAN_RAIN_COUNT = 24;
const float TALISMAN_RAIN_SPEED = 0.95f;
const float TALISMAN_RAIN_TOP_Y = 4.5f;
const float TALISMAN_RAIN_BOTTOM_Y = 0.2f;

// ===== Joint Adjustment Limits =====
const float JOINT_ANGLE_STEP = 5.0f;
const float SHOULDER_MIN_ANGLE = -30.0f;
const float SHOULDER_MAX_ANGLE = 90.0f;
const float ELBOW_MIN_ANGLE = 0.0f;
const float ELBOW_MAX_ANGLE = 110.0f;
const float HIP_MIN_ANGLE = -45.0f;
const float HIP_MAX_ANGLE = 45.0f;
const float KNEE_MIN_ANGLE = -8.0f;
const float KNEE_MAX_ANGLE = 8.0f;
const float HEAD_MIN_ANGLE = -30.0f;
const float HEAD_MAX_ANGLE = 30.0f;


// ===== Customization / Selection =====
enum BodyPartSelection
{
	BODY_PART_SHOULDER = 0,
	BODY_PART_ELBOW = 1,
	BODY_PART_HIP = 2,
	BODY_PART_KNEE = 3,
	BODY_PART_HEAD = 4,
	BODY_PART_COUNT = 5
};

enum FaceExpression
{
	FACE_DEFAULT = 0,
	FACE_BITE = 1,
	FACE_QUESTION = 2,
	FACE_COUNT = 3
};

int selectedBodyPart = BODY_PART_SHOULDER;
int selectedFace = FACE_DEFAULT;

void ClampJointAngles()
{
	if (shoulderAngle < SHOULDER_MIN_ANGLE) shoulderAngle = SHOULDER_MIN_ANGLE;
	if (shoulderAngle > SHOULDER_MAX_ANGLE) shoulderAngle = SHOULDER_MAX_ANGLE;
	if (elbowAngle < ELBOW_MIN_ANGLE) elbowAngle = ELBOW_MIN_ANGLE;
	if (elbowAngle > ELBOW_MAX_ANGLE) elbowAngle = ELBOW_MAX_ANGLE;
	if (hipAngle < HIP_MIN_ANGLE) hipAngle = HIP_MIN_ANGLE;
	if (hipAngle > HIP_MAX_ANGLE) hipAngle = HIP_MAX_ANGLE;
	if (kneeAngle < KNEE_MIN_ANGLE) kneeAngle = KNEE_MIN_ANGLE;
	if (kneeAngle > KNEE_MAX_ANGLE) kneeAngle = KNEE_MAX_ANGLE;
	if (headAngle < HEAD_MIN_ANGLE) headAngle = HEAD_MIN_ANGLE;
	if (headAngle > HEAD_MAX_ANGLE) headAngle = HEAD_MAX_ANGLE;
}

const char* GetSelectedBodyPartName()
{
 switch (selectedBodyPart)
	{
	case BODY_PART_SHOULDER: return "Shoulder";
	case BODY_PART_ELBOW: return "Elbow";
	case BODY_PART_HIP: return "Hip";
	case BODY_PART_KNEE: return "Knee";
	default: return "Head";
	}
}

const char* GetSelectedFaceName()
{
	switch (selectedFace)
	{
	case FACE_BITE: return "Bite";
	case FACE_QUESTION: return "Question";
	default: return "Default";
	}
}

const char* GetCustomizationName()
{
	switch (embroideryTextureIndex)
	{
	case 0: return "Customization 1";
	case 1: return "Customization 2";
	default: return "Customization 3";
	}
}

void AdjustSelectedBodyPart(float delta)
{
	if (selectedBodyPart == BODY_PART_SHOULDER)
		shoulderAngle += delta;
    else if (selectedBodyPart == BODY_PART_ELBOW)
		elbowAngle += delta;
	else if (selectedBodyPart == BODY_PART_HIP)
		hipAngle += delta;
    else if (selectedBodyPart == BODY_PART_KNEE)
		kneeAngle += delta;
	else
     headAngle += delta;

	ClampJointAngles();
}

const GLfloat* GetCustomizationColor(int colorSlot)
{
    static const GLfloat* colors[3][3] =
	{
		{ COLOR_MAROON, COLOR_DARK_PURPLE, COLOR_DARK_PLUM },
		{ COLOR_GOLD, COLOR_MAROON, COLOR_GREEN },
		{ COLOR_GREY, COLOR_BABY_BLUE, COLOR_ORANGE }
		
	};

	return colors[embroideryTextureIndex % 3][colorSlot % 3];
}

GLuint GetCustomizationTexture()
{
  switch (embroideryTextureIndex % 3)
	{
	case 1: return T_embroidery2;
	case 2: return T_embroidery3;
	default: return T_embroidery;
	}
}


void ResetSceneState();
void ResetCharacterState();
void UpdateAnimation();
bool HasDelayElapsed(DWORD startTick, DWORD delayMs);
void drawTalisman(float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
void drawText2D(float x, float y, const char* text);
void drawKeybindOverlay();
void drawTalismanRain();
bool InitAudio();
void playSE(int seNum);
void StartBgmLoop();
void StopBgm();

GLuint fontBase = 0;

// UINT = Unsigned integer e.g. Mouse Moved (Like email title)
// WPARAM, LPARAM = Parameters
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
	{
		int w = LOWORD(lParam);
		int h = HIWORD(lParam);
		if (w > 0) windowWidth = w;
		if (h > 0) windowHeight = h;
		break;
	}

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			ResetSceneState();
			break;
     case 'R':
			ResetCharacterState();
			break;
		case 0x31: // 1
			questionNum = 1;
			break;
		case 0x32: // 2
			questionNum = 2;
			break;
		case 0x33: // 3
			questionNum = 3;
			break;  

			// Camera movement
		case 'W':
			if (controlMode == CONTROL_CAMERA)
				camZ -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT1)
				lightY += movementSpeed;
			else if (controlMode == CONTROL_LIGHT2)
				light2Y += movementSpeed;
			else if (controlMode == CONTROL_LIGHT3)
				light3Y += movementSpeed;
	
			break;  
		case 'S':
			if (controlMode == CONTROL_CAMERA)
				camZ += movementSpeed;
			else if (controlMode == CONTROL_LIGHT1)
				lightY -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT2)
				light2Y -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT3)
				light3Y -= movementSpeed;
	
			break;  
		case 'A':
			if (controlMode == CONTROL_CAMERA)
				camX -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT1)
				lightX -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT2)
				light2X -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT3)
				light3X -= movementSpeed;
	
			break;  
		case 'D':
			if (controlMode == CONTROL_CAMERA)
				camX += movementSpeed;
			else if (controlMode == CONTROL_LIGHT1)
				lightX += movementSpeed;
			else if (controlMode == CONTROL_LIGHT2)
				light2X += movementSpeed;
			else if (controlMode == CONTROL_LIGHT3)
				light3X += movementSpeed;
		
			break; 
		case 'E':
			if (controlMode == CONTROL_CAMERA)
				camY += movementSpeed;
			else if (controlMode == CONTROL_LIGHT1)
				lightZ += movementSpeed;
			else if (controlMode == CONTROL_LIGHT2)
				light2Z += movementSpeed;
			else if (controlMode == CONTROL_LIGHT3)
				light3Z += movementSpeed;
		
			break;
		case 'Q':
			if (controlMode == CONTROL_CAMERA)
				camY -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT1)
				lightZ -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT2)
				light2Z -= movementSpeed;
			else if (controlMode == CONTROL_LIGHT3)
				light3Z -= movementSpeed;

			break; 

			// X-axis rotation
		case 'P': camRotX += rotSpeed;
			break;  
		case 'O': camRotX -= rotSpeed;
			break;  

			// Y-axis rotation
		case 'K': camRotY -= rotSpeed;
			break;  
		case 'L': camRotY += rotSpeed;
			break; 

			// Z-axis rotation
		case 'M': camRotZ += rotSpeed;
			break;  
		case 'N': camRotZ -= rotSpeed;
			break;  

		case VK_TAB:
			isPerspective = !isPerspective;
			break;
		case VK_F1:
			showKeybindOverlay = !showKeybindOverlay;
			break;
		case VK_F2:
			isLightOn = !isLightOn;
			break;
      case VK_F3:
			StopBgm();
			break;
      case VK_CAPITAL:
			showLightVisualizer = !showLightVisualizer;
			break;
		case VK_SHIFT:
            controlMode = (controlMode + 1) % 4;
			break;

		case VK_SPACE:
			jumpRequested = true;
			break;
		case 'B':
			showBell = !showBell;
         if (showBell)
				playSE(SE_BELL);
          if (showBell)
			{
				if (!isSummonActive)
				{
                 isDelayActive = true;
					delayStartTick = GetTickCount();
				}
			}
			else
			{
                isDelayActive = false;
			}
			break;
       case 'G':
			embroideryTextureIndex = (embroideryTextureIndex + 1) % 3;
          playSE(SE_SELECT);
			break;
		case 'J':
			isJumpPoseToggle = !isJumpPoseToggle;
			jumpRequested = false;
			break;
		case 'Z':
          AdjustSelectedBodyPart(JOINT_ANGLE_STEP);
          playSE(SE_TWEAK);
			break;
		case 'X':
          AdjustSelectedBodyPart(-JOINT_ANGLE_STEP);
          playSE(SE_TWEAK);
			break;
		case VK_OEM_6: // ]
			selectedBodyPart = (selectedBodyPart + 1) % BODY_PART_COUNT;
          playSE(SE_SELECT);
			break;
		case VK_OEM_4: // [
			selectedBodyPart = (selectedBodyPart + BODY_PART_COUNT - 1) % BODY_PART_COUNT;
          playSE(SE_SELECT);
			break;
		case VK_OEM_COMMA: // <
			selectedFace = (selectedFace + FACE_COUNT - 1) % FACE_COUNT;
          playSE(SE_SELECT);
			break;
		case VK_OEM_PERIOD: // >
			selectedFace = (selectedFace + 1) % FACE_COUNT;
          playSE(SE_SELECT);
			break;
		case VK_UP:
			moveForward = true;
			break;
		case VK_DOWN:
			moveBackward = true;
			break;
		case VK_LEFT:
			moveLeft = true;
			break;
		case VK_RIGHT:
			moveRight = true;
			break;

		}
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_UP:
			moveForward = false;
			break;
		case VK_DOWN:
			moveBackward = false;
			break;
		case VK_LEFT:
			moveLeft = false;
			break;
		case VK_RIGHT:
			moveRight = false;
			break;
		default:
			break;
		}
		break;


	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------


bool InitPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.cAlphaBits = 8;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ResetSceneState()
{
	movementSpeed = 0.1f;
	rotSpeed = 5.0f;

	posX = 0.0f; posY = 0.0f; posZ = 0.0f;
	rotX = 0.0f; rotY = 0.0f; rotZ = 0.0f;
	characterFacingY = 0.0f;

	camX = START_CAM_X;
	camY = START_CAM_Y;
	camZ = START_CAM_Z;
	camRotX = START_CAM_ROT_X;
	camRotY = START_CAM_ROT_Y;
	camRotZ = START_CAM_ROT_Z;

	isPerspective = true;
	isLightMovementMode = false;
	controlMode = CONTROL_CAMERA;

	lightX = 0.9f;
	lightY = 1.8f;
	lightZ = 0.4f;
	light2X = -1.3f;
	light2Y = 0.3f;
	light2Z = -0.4f;
	light3X = -2.0f;
	light3Y = 3.0f;
	light3Z = 1.0f;
	isLightOn = true;

	ResetCharacterState();

	showKeybindOverlay = false;
	isSummonActive = false;
    isDelayActive = false;
	delayStartTick = 0;
	summonedCoffinLiftY = SUMMON_COFFIN_START_Y;
	selectedBodyPart = BODY_PART_SHOULDER;
	selectedFace = FACE_DEFAULT;
	embroideryTextureIndex = 0;

}

void ResetCharacterState()
{
	posX = 0.0f; posY = 0.0f; posZ = 0.0f;
	rotX = 0.0f; rotY = 0.0f; rotZ = 0.0f;
	characterFacingY = 0.0f;

	moveForward = false;
	moveBackward = false;
	moveLeft = false;
	moveRight = false;
	showBell = false;
    hasBellPeakSoundPlayed = false;
    isDelayActive = false;
	delayStartTick = 0;
	jumpRequested = false;
	isJumping = false;
	isJumpPoseToggle = false;
	isWalking = false;
	jumpOffsetY = 0.0f;
	jumpVelocity = 0.0f;
	walkPhase = 0.0f;
	shoulderAngle = 0.0f;
	elbowAngle = 0.0f;
	hipAngle = 0.0f;
	kneeAngle = 0.0f;
	headAngle = 0.0f;
	selectedFace = FACE_DEFAULT;
}

void setupProjection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float aspect = (windowHeight > 0) ? ((float)windowWidth / (float)windowHeight) : 1.0f;

	if (isPerspective)
        gluPerspective(60, aspect, 0.1f, 100.0f);
	else
    {
		if (aspect >= 1.0f)
			glOrtho(-2.0f * aspect, 2.0f * aspect, -2.0f, 2.0f, 0.1f, 100.0f);
		else
			glOrtho(-2.0f, 2.0f, -2.0f / aspect, 2.0f / aspect, 0.1f, 100.0f);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void setupCameraView()
{
	glLoadIdentity();
	glTranslatef(-camX, -camY, -camZ);
	glRotatef(-camRotX, 1.0f, 0.0f, 0.0f);
	glRotatef(-camRotY, 0.0f, 1.0f, 0.0f);
	glRotatef(-camRotZ, 0.0f, 0.0f, 1.0f);
}

void setupLight()
{
	diffuseLightPosition[0] = lightX;
	diffuseLightPosition[1] = lightY;
	diffuseLightPosition[2] = lightZ;
	diffuseLightPosition[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, diffuseLightPosition);

	diffuseLightPosition2[0] = light2X;
	diffuseLightPosition2[1] = light2Y;
	diffuseLightPosition2[2] = light2Z;
	diffuseLightPosition2[3] = 1.0f;
	glLightfv(GL_LIGHT1, GL_POSITION, diffuseLightPosition2);

	diffuseLightPosition3[0] = light3X;
	diffuseLightPosition3[1] = light3Y;
	diffuseLightPosition3[2] = light3Z;
	diffuseLightPosition3[3] = 1.0f;
	glLightfv(GL_LIGHT2, GL_POSITION, diffuseLightPosition3);
}

void setupRenderState()
{
   if (isLightOn)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightColour);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLightColour1);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLightColour);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLightColour2);
		glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLightColour);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLightColour3);
	}
	else
	{
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
	}
	glDisable(GL_TEXTURE_2D);
}

bool HasDelayElapsed(DWORD startTick, DWORD delayMs)
{
	return (GetTickCount() - startTick) >= delayMs;
}

bool InitAudio()
{
	isAudioInitialized = true;
	return true;
}

void StartBgmLoop()
{
	if (isBgmPlaying)
		return;

	mciSendStringA("close bgm", NULL, 0, NULL);

	char openCommand[512] = { 0 };
	wsprintfA(openCommand, "open \"%s\" type mpegvideo alias bgm", BGM_FILE_PATH);
	if (mciSendStringA(openCommand, NULL, 0, NULL) != 0)
		return;

	if (mciSendStringA("play bgm repeat", NULL, 0, NULL) == 0)
		isBgmPlaying = true;
	else
		mciSendStringA("close bgm", NULL, 0, NULL);
}

void StopBgm()
{
	mciSendStringA("stop bgm", NULL, 0, NULL);
	mciSendStringA("close bgm", NULL, 0, NULL);
	isBgmPlaying = false;
}

void playSE(int seNum)
{
	if (!isAudioInitialized)
		InitAudio();

	if (seNum <= SE_NONE || seNum > SE_COUNT)
		return;

	const char* sePath = seFilePath[seNum];
	if (sePath == NULL)
		return;

	mciSendStringA("close se", NULL, 0, NULL);

	char openCommand[512] = { 0 };
	wsprintfA(openCommand, "open \"%s\" type waveaudio alias se", sePath);
	if (mciSendStringA(openCommand, NULL, 0, NULL) != 0)
		return;

	mciSendStringA("play se from 0", NULL, 0, NULL);
}

void UpdateAnimation()
{
	float moveX = 0.0f;
	float moveZ = 0.0f;

	if (moveForward) moveZ -= JUMP_STEP;
	if (moveBackward) moveZ += JUMP_STEP;
	if (moveLeft) moveX -= JUMP_STEP;
	if (moveRight) moveX += JUMP_STEP;

	if (moveForward)
		characterFacingY = 180.0f;
	else if (moveBackward)
		characterFacingY = 0.0f;
	else if (moveRight)
		characterFacingY = 90.0f;
	else if (moveLeft)
		characterFacingY = -90.0f;

    if ((moveForward || moveBackward || moveLeft || moveRight) && !isJumpPoseToggle && !isJumping && !jumpRequested)
		jumpRequested = true;

	posX += moveX;
	posZ += moveZ;

	isWalking = false;
	walkPhase = 0.0f;
	ponytailSwingPhase = 0.0f;
	ponytailSwingAngle = 0.0f;

	if (isJumpPoseToggle)
	{
		jumpRequested = false;
		isJumping = false;
		jumpOffsetY = 0.0f;
		jumpVelocity = 0.0f;
		return;
	}

	if (jumpRequested && !isJumping)
	{
		isJumping = true;
		jumpVelocity = JUMP_START_VELOCITY;
	}
	jumpRequested = false;

	if (isJumping)
	{
		jumpVelocity += GRAVITY;
		jumpOffsetY += jumpVelocity;

		if (jumpOffsetY <= 0.0f)
		{
			jumpOffsetY = 0.0f;
			jumpVelocity = 0.0f;
			isJumping = false;
		}
	}

	if (isJumping)
	{
		ponytailSwingPhase += PONYTAIL_SWING_SPEED;
		ponytailSwingAngle = sinf(ponytailSwingPhase) * PONYTAIL_SWING_MAX_ANGLE;
	}
	else
	{
		ponytailSwingAngle *= 0.88f;
		if (fabsf(ponytailSwingAngle) < 0.05f)
		{
			ponytailSwingAngle = 0.0f;
			ponytailSwingPhase = 0.0f;
		}
	}

	if (isSummonActive && summonedCoffinLiftY < SUMMON_COFFIN_TARGET_Y)
	{
		summonedCoffinLiftY += SUMMON_COFFIN_RISE_SPEED;
		if (summonedCoffinLiftY > SUMMON_COFFIN_TARGET_Y)
			summonedCoffinLiftY = SUMMON_COFFIN_TARGET_Y;
	}

	 if (isDelayActive && !isSummonActive)
		{
		   if (HasDelayElapsed(delayStartTick, SUMMON_DELAY_MS))
			{
				isSummonActive = true;
				isDelayActive = false;
			}
		}
}

GLuint LoadTexture(const char* filename)
{
	hBMP = (HBITMAP)LoadImageA(NULL, filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (!hBMP)
	{
		MessageBoxA(NULL, filename, "Failed to load texture", MB_OK | MB_ICONERROR);
		return 0;
	}

	GetObject(hBMP, sizeof(BMP), &BMP);

	GLuint texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	DeleteObject(hBMP);
	hBMP = NULL;
	return texID;
}

void InitTextures()
{
	// every pixel = 4 bytes
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glEnable(GL_TEXTURE_2D);

	T_talisman = LoadTexture("..\\Textures\\talisman.bmp");
	T_fabric = LoadTexture("..\\Textures\\fabric.bmp");
	T_embroidery = LoadTexture("..\\Textures\\embroidery.bmp");
	T_embroidery2 = LoadTexture("..\\Textures\\embroidery2.bmp");
	T_embroidery3 = LoadTexture("..\\Textures\\embroidery3.bmp");
	T_dragon = LoadTexture("..\\Textures\\dragon.bmp");
	T_concrete = LoadTexture("..\\Textures\\floor3.bmp");
	T_wood = LoadTexture("..\\Textures\\wood.bmp");
	T_bricks = LoadTexture("..\\Textures\\wall2.bmp");
	T_copper = LoadTexture("..\\Textures\\copper.bmp");
	T_copper2 = LoadTexture("..\\Textures\\copper2.bmp");
	T_spiral = LoadTexture("..\\Textures\\spiral.bmp");
	T_beam = LoadTexture("..\\Textures\\beam.bmp");
}

struct TextureBindState
{
	GLboolean wasTextureEnabled;
	GLint previousTexture;
};

TextureBindState BindTexture(GLuint texID, GLUquadricObj* quadric = NULL)
{
	TextureBindState state;
	state.wasTextureEnabled = glIsEnabled(GL_TEXTURE_2D);
	state.previousTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.previousTexture);
	glEnable(GL_TEXTURE_2D);
	if (quadric != NULL)
		gluQuadricTexture(quadric, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texID);
	return state;
}

void UnbindTexture(const TextureBindState& state, GLUquadricObj* quadric = NULL)
{
	if (quadric != NULL)
		gluQuadricTexture(quadric, GL_FALSE);
	glBindTexture(GL_TEXTURE_2D, (GLuint)state.previousTexture);
	if (!state.wasTextureEnabled)
		glDisable(GL_TEXTURE_2D);
}

float DegreeToRadian(float degree)
{
	return degree * 3.142f / 180.0f;
}

float ComputeSwingAngle(float timeSec, float speed, float maxAngle, float phaseOffset = 0.0f, float swingFactor = 1.0f)
{
	return sinf((timeSec * speed) - phaseOffset) * (maxAngle * swingFactor);
}

void ApplySwingRotateZ(float timeSec, float speed, float maxAngle, float phaseOffset = 0.0f, float swingFactor = 1.0f)
{
	glRotatef(ComputeSwingAngle(timeSec, speed, maxAngle, phaseOffset, swingFactor), 0.0f, 0.0f, 1.0f);
}
void ApplySwingRotateY(float timeSec, float speed, float maxAngle, float phaseOffset = 0.0f, float swingFactor = 1.0f)
{
	glRotatef(ComputeSwingAngle(timeSec, speed, maxAngle, phaseOffset, swingFactor), 0.0f, 1.0f, 0.0f);
}
void ApplySwingRotateX(float timeSec, float speed, float maxAngle, float phaseOffset = 0.0f, float swingFactor = 1.0f)
{
	glRotatef(ComputeSwingAngle(timeSec, speed, maxAngle, phaseOffset, swingFactor), 1.0f, 0.0f, 0.0f);
}

void drawHalfCylinder(float baseRadius, float topRadius, float height, int segments, bool capBottom = true, bool capTop = true, bool closeFlatSide = true)
{
	if (segments < 1)
		segments = 1;

	const float startAngle = 0.0f;
	const float endAngle = 3.142f;

	// curved outer wall
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= segments; i++)
	{
		float t = (float)i / (float)segments;
		float angle = startAngle + (endAngle - startAngle) * t;
		float x = cosf(angle);
		float y = sinf(angle);

		glNormal3f(x, y, 0.0f);
		glTexCoord2f(t, 0.0f);
		glVertex3f(baseRadius * x, baseRadius * y, 0.0f);
		glTexCoord2f(t, 1.0f);
		glVertex3f(topRadius * x, topRadius * y, height);
	}
	glEnd();

	// flat cut face
	if (closeFlatSide)
	{
		glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(baseRadius, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-baseRadius, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-topRadius, 0.0f, height);
        glTexCoord2f(0.0f, 1.0f);
		glVertex3f(topRadius, 0.0f, height);
		glEnd();
	}

	if (capBottom)
	{
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		for (int i = 0; i <= segments; i++)
		{
			float t = (float)i / (float)segments;
			float angle = startAngle + (endAngle - startAngle) * t;
			glTexCoord2f(0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle));
			glVertex3f(baseRadius * cosf(angle), baseRadius * sinf(angle), 0.0f);
		}
		glEnd();
	}

	if (capTop)
	{
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(0.0f, 0.0f, height);
		for (int i = 0; i <= segments; i++)
		{
			float t = (float)i / (float)segments;
			float angle = endAngle - (endAngle - startAngle) * t;
			glTexCoord2f(0.5f + 0.5f * cosf(angle), 0.5f + 0.5f * sinf(angle));
			glVertex3f(topRadius * cosf(angle), topRadius * sinf(angle), height);
		}
		glEnd();
	}
}

void drawLightVisualizer(float x, float y, float z, const GLfloat* color)
{
	GLboolean wasLightingEnabled = glIsEnabled(GL_LIGHTING);
	glPushMatrix();
	if (wasLightingEnabled)
		glDisable(GL_LIGHTING);
	glColor3fv(color);
	glTranslatef(x, y, z);
	gluSphere(sphere, 0.1f, slices, stacks);
	if (wasLightingEnabled)
		glEnable(GL_LIGHTING);
	glPopMatrix();
}

void drawTalismanRain()
{
    // Only start rain after summon is active and coffins fully reached target height.
	if (!isSummonActive || summonedCoffinLiftY < SUMMON_COFFIN_TARGET_Y)
		return;

   // Vertical range where talismans loop from top to bottom.
	const float spanY = TALISMAN_RAIN_TOP_Y - TALISMAN_RAIN_BOTTOM_Y;
   // Global time (seconds) used to animate continuous falling.
	const float timeSec = (float)GetTickCount() * 0.001f;

   // Spawn a fixed number of talismans in lanes/rows, then animate each one.
	for (int i = 0; i < TALISMAN_RAIN_COUNT; i++)
	{
        // Each talisman is placed in a 6-column grid
		float lane = (float)(i % 6);
		float row = (float)(i / 6);
		float x = -1.8f + lane * 0.72f;
		float z = -2.8f + row * 0.9f;


		// Per-talisman phase offset so they don't all fall in sync.
		float phase = ((float)i * 0.37f);
        // Wrap falling movement with fmodf so each talisman resets to top after reaching bottom.
		float y = TALISMAN_RAIN_TOP_Y - fmodf(timeSec * TALISMAN_RAIN_SPEED + phase, spanY);

		// Draw one talisman at computed position with slight animated Z-rotation.
		drawTalisman(x, y, z, -180.0f, lane * 22.0f, sinf(timeSec + phase) * 8.0f);
	}
}

void applyInnerPivotXRotate(float angle, float pivotIn)
{
	glTranslatef(0.0f, 0.0f, pivotIn);
	glRotatef(angle, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, -pivotIn);
}

void applyInnerPivotXRotateY(float angle, float pivotY)
{
	glTranslatef(0.0f, pivotY, 0.0f);
	glRotatef(angle, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -pivotY, 0.0f);
}

void ApplyArmPoseBase(float sign, float baseY)
{
	glTranslatef(sign * 0.06f, baseY, 0.0f);
	glRotatef(sign * -104.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(-80.0f, 1.0f, 0.0f, 0.0f);
}

void ApplyArmGroup1Pose(float sign, float baseY, bool isJumpPose)
{
	ApplyArmPoseBase(sign, baseY);
	if (isJumpPose)
	{
		glTranslatef(sign * 0.0f, 0.02f, 0.02f);
		glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(sign * -8.0f, 0.0f, 1.0f, 0.0f);
	}

}

void ApplyArmGroup2Pose(float sign, float baseY, bool isJumpPose)
{
	ApplyArmPoseBase(sign, baseY);
	if (isJumpPose) {
		glRotatef(48.0f, 1.0f, 0.0f, 0.0f);
		glTranslatef(sign * 0.02f, 0.06f, -0.04f); 
		glRotatef(sign * -12.0f, 0.0f, 1.0f, 0.0f);
	}

	applyInnerPivotXRotate(shoulderAngle, 0.10f);
	glTranslatef(0.0f, 0.0f, 0.10f);
}

void ApplyArmGroup3Pose(float sign, float baseY, bool isJumpPose)
{
	ApplyArmGroup2Pose(sign, baseY, isJumpPose);
    applyInnerPivotXRotate(elbowAngle, 0.15f);
	glTranslatef(0.0f, 0.0f, 0.15f);
	if (isJumpPose) {
		glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
		glTranslatef(sign * 0.00f, 0.00f, -0.02f); 
	} 

	if (showBell && sign > 0.0f)
	{
		float timeSec = (float)GetTickCount() * 0.001f;
		ApplySwingRotateX(timeSec, 4.5f, 14.0f);
	}
}

void ApplyLegPantGroup1Pose(float sign, bool isJumpPose)
{
	if (isJumpPose)
	{
		glTranslatef(sign * 0.09f, -0.36f, 0.02f);
		glRotatef(60.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(sign * 8.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		glTranslatef(sign * 0.09f, -0.34f, 0.0f);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	}
   applyInnerPivotXRotate(hipAngle, 0.12f);
}

void ApplyLegPantGroup2Pose(float sign, bool isJumpPose)
{
 ApplyLegPantGroup1Pose(sign, isJumpPose);
	if (isJumpPose)
	{
		glTranslatef(0.0f, 0.0f, 0.16f);
		glRotatef(75.0f, 1.0f, 0.0f, 0.0f);
	}
	else
	{
		glTranslatef(0.0f, 0.0f, 0.18f);
		glRotatef(2.0f, 1.0f, 0.0f, 0.0f);
	}
	applyInnerPivotXRotate(kneeAngle, 0.10f);
}

void drawBackground() {
	const float cubeLength = 30.0f;
	const float cubeWidth = 30.0f;
	const float cubeHeight = 30.0f;
	const float halfLength = cubeLength * 0.5f;
	const float halfWidth = cubeWidth * 0.5f;
	const float bottomY = 0.0f;
	const float topY = bottomY + cubeHeight;

	GLboolean wasLightingEnabled = glIsEnabled(GL_LIGHTING);
	if (wasLightingEnabled)
		glDisable(GL_LIGHTING);

	TextureBindState floorTextureState = BindTexture(T_concrete);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	// bottom quad at y = 0.0 (textured concrete floor)
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfLength, bottomY, -halfWidth);
	glTexCoord2f(6.0f, 0.0f); glVertex3f(halfLength, bottomY, -halfWidth);
	glTexCoord2f(6.0f, 6.0f); glVertex3f(halfLength, bottomY, halfWidth);
	glTexCoord2f(0.0f, 6.0f); glVertex3f(-halfLength, bottomY, halfWidth);
	glEnd();
	UnbindTexture(floorTextureState);

	// top
	glBegin(GL_QUADS);
	glColor3f(0.20f, 0.22f, 0.30f);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-halfLength, topY, -halfWidth);
	glVertex3f(-halfLength, topY, halfWidth);
	glVertex3f(halfLength, topY, halfWidth);
	glVertex3f(halfLength, topY, -halfWidth);
	glEnd();

	TextureBindState wallTextureState = BindTexture(T_bricks);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	// back
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfLength, bottomY, -halfWidth);
	glTexCoord2f(0.0f, 3.0f); glVertex3f(-halfLength, topY, -halfWidth);
	glTexCoord2f(6.0f, 3.0f); glVertex3f(halfLength, topY, -halfWidth);
	glTexCoord2f(6.0f, 0.0f); glVertex3f(halfLength, bottomY, -halfWidth);

	// front
	glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfLength, bottomY, halfWidth);
	glTexCoord2f(6.0f, 0.0f); glVertex3f(halfLength, bottomY, halfWidth);
	glTexCoord2f(6.0f, 3.0f); glVertex3f(halfLength, topY, halfWidth);
	glTexCoord2f(0.0f, 3.0f); glVertex3f(-halfLength, topY, halfWidth);

	// left
	glNormal3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfLength, bottomY, -halfWidth);
	glTexCoord2f(6.0f, 0.0f); glVertex3f(-halfLength, bottomY, halfWidth);
	glTexCoord2f(6.0f, 3.0f); glVertex3f(-halfLength, topY, halfWidth);
	glTexCoord2f(0.0f, 3.0f); glVertex3f(-halfLength, topY, -halfWidth);

	// right
	glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(halfLength, bottomY, -halfWidth);
	glTexCoord2f(0.0f, 3.0f); glVertex3f(halfLength, topY, -halfWidth);
	glTexCoord2f(6.0f, 3.0f); glVertex3f(halfLength, topY, halfWidth);
	glTexCoord2f(6.0f, 0.0f); glVertex3f(halfLength, bottomY, halfWidth);
	glEnd();
	UnbindTexture(wallTextureState);

	if (wasLightingEnabled)
		glEnable(GL_LIGHTING);
}

void drawText2D(float x, float y, const char* text)
{
	if (fontBase == 0 || text == NULL)
		return;

	glRasterPos2f(x, y);
	glListBase(fontBase - 32);
	glCallLists(lstrlenA(text), GL_UNSIGNED_BYTE, text);
}

void drawKeybindOverlay()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

    glPixelZoom(1.2f, 1.2f);

	glColor3f(1.0f, 1.0f, 1.0f);
	drawText2D(0.70f, 0.95f, "F1 - Help");
	char selectedPartLabel[64];
	wsprintfA(selectedPartLabel, "Selected: %s", GetSelectedBodyPartName());
	drawText2D(-0.95f, 0.95f, selectedPartLabel);
	char selectedFaceLabel[64];
	wsprintfA(selectedFaceLabel, "Face: %s", GetSelectedFaceName());
	drawText2D(-0.95f, 0.90f, selectedFaceLabel);
	char selectedCustomizationLabel[80];
	wsprintfA(selectedCustomizationLabel, "%s", GetCustomizationName());
	drawText2D(-0.95f, 0.85f, selectedCustomizationLabel);

	if (!showKeybindOverlay)
	{
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glEnable(GL_DEPTH_TEST);
		return;
	}

	const float keyX = -0.95f;
	const float descX = -0.58f;
	const float keybindYOffset = 0.16f;

	auto drawKeyRow = [&](float y, const char* key, const char* description)
	{
		drawText2D(keyX, y + keybindYOffset, key);
		drawText2D(descX, y + keybindYOffset, description);
	};

	drawText2D(keyX, 0.61f + keybindYOffset, "-------------- CAMERA & LIGHT MOVEMENT ----------------------------------------");
	drawKeyRow(0.56f, "W", "Forward");
	drawKeyRow(0.51f, "S", "Backward");
	drawKeyRow(0.46f, "A", "Left");
	drawKeyRow(0.41f, "D", "Right");
	drawKeyRow(0.36f, "Q", "Down");
	drawKeyRow(0.31f, "E", "Up");
	drawKeyRow(0.26f, "P / O", "X rotation");
	drawKeyRow(0.21f, "K / L", "Y rotation");
	drawKeyRow(0.16f, "M / N", "Z rotation");

	drawText2D(keyX, 0.10f + keybindYOffset, "-------------- CHARACTER --------------------------------------------------------------------");
	drawKeyRow(0.05f, "Arrow Keys", "Move character");
	drawKeyRow(0.00f, "Space", "Jump");
	drawKeyRow(-0.05f, "J", "Toggle jump pose");
	drawKeyRow(-0.10f, "B", "Summon");
	drawKeyRow(-0.15f, "G", "Switch cloth customization");
	drawKeyRow(-0.20f, "R", "Reset character");

    drawText2D(keyX, -0.26f + keybindYOffset, "-------------- BODY PART ADJUSTMENT -------------------------------------------");
	drawKeyRow(-0.31f, "[", "Select previous body part");
	drawKeyRow(-0.36f, "]", "Select next body part");
	drawKeyRow(-0.41f, "Z", "Increase angle");
	drawKeyRow(-0.46f, "X", "Decrease angle");
	drawKeyRow(-0.51f, "< ", "Previous face expression");
	drawKeyRow(-0.56f, ">", "Next face expression");

	drawText2D(keyX, -0.62f + keybindYOffset, "-------------- UTILITY --------------------------------------------------------------------------");
	drawKeyRow(-0.67f, "1", "Character");
	drawKeyRow(-0.72f, "2", "Weapon");
	drawKeyRow(-0.77f, "Esc", "Reset scene");
	drawKeyRow(-0.82f, "Shift", "Switch control mode (Cam/L1/L2/L3)");
	drawKeyRow(-0.87f, "Tab", "Switch projection");
	drawKeyRow(-0.92f, "CapsLock", "Toggle light visualizers");
	drawKeyRow(-0.97f, "F1", "Toggle help menu");
	drawKeyRow(-1.02f, "F2", "Toggle light");
	drawKeyRow(-1.07f, "F3", "Stop BGM");
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);


	glEnable(GL_DEPTH_TEST);
}

void drawBell()
{
	const float BELL_SWING_SPEED = 4.5f;
	const float BELL_SWING_MAX_ANGLE = 16;
	const float BELL_PEAK_TRIGGER_EPSILON = 0.2f;
	const float BELL_PEAK_RESET_MARGIN = 1.0f;
	float timeSec = (float)GetTickCount() * 0.001f;
	float bellSwingAngle = showBell ? ComputeSwingAngle(timeSec, BELL_SWING_SPEED, BELL_SWING_MAX_ANGLE) : 0.0f;

	if (showBell)
	{
		if (!hasBellPeakSoundPlayed && bellSwingAngle >= (BELL_SWING_MAX_ANGLE - BELL_PEAK_TRIGGER_EPSILON))
		{
			playSE(SE_BELL);
			hasBellPeakSoundPlayed = true;
		}
		else if (hasBellPeakSoundPlayed && bellSwingAngle < (BELL_SWING_MAX_ANGLE - BELL_PEAK_RESET_MARGIN))
		{
			hasBellPeakSoundPlayed = false;
		}
	}
	else
	{
		hasBellPeakSoundPlayed = false;
	}

	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricDrawStyle(disk, GLU_FILL);

	glPushMatrix();
	if (showBell)
		ApplySwingRotateZ(timeSec, BELL_SWING_SPEED, BELL_SWING_MAX_ANGLE);

	TextureBindState handleTextureState = BindTexture(T_copper);
	glColor3f(1.0f, 1.0f, 1.0f);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricTexture(cylinder, GL_TRUE);
	gluQuadricTexture(disk, GL_TRUE);

	// top ring
	glPushMatrix();
	glTranslatef(0.0f, 0.26f, 0.0f);
	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(45.0f * i, 0.0f, 1.0f, 0.0f);
		glScalef(1.0f, 1.2f, 1.0f);
		glTranslatef(0.0f, 0.0f, -0.01f);
		gluCylinder(cylinder, 0.06f, 0.06f, 0.02f, slices, stacks);
		glPopMatrix();
	}
	glPopMatrix();
	
	// handle
	glPushMatrix();
	glTranslatef(0.0f, 0.17f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, 0.015f, 0.04f, 0.04f, slices, stacks);
	glPopMatrix();

	// upper sphere
	glPushMatrix();
    glTranslatef(0.0f, 0.16f, 0.0f);
	glScalef(1.4f, 1.0f, 1.0f);
	gluSphere(sphere, 0.015f, slices, stacks);
	glPopMatrix();

	// upper short connector
	TextureBindState upperConnectorTextureState = BindTexture(T_copper2);
	glPushMatrix();
	glTranslatef(0.0f, 0.14f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, 0.015f, 0.015f, 0.010f, slices, stacks);
	glPopMatrix();
	UnbindTexture(upperConnectorTextureState);

	// long connector
	glPushMatrix();
    glTranslatef(0.0f, 0.12f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, 0.011f, 0.011f, 0.02f, slices, stacks);
	glPopMatrix();

	// lower short connector
	TextureBindState lowerConnectorTextureState = BindTexture(T_copper2);
	glPushMatrix();
    glTranslatef(0.0f, 0.11f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, 0.015f, 0.015f, 0.010f, slices, stacks);
	glPopMatrix();
	UnbindTexture(lowerConnectorTextureState);

	// lower sphere
	glPushMatrix();
	glTranslatef(0.0f, 0.10f, 0.0f);
	glScalef(1.4f, 1.0f, 1.0f);
	gluSphere(sphere, 0.015f, slices, stacks);
	glPopMatrix();

	UnbindTexture(handleTextureState);

	TextureBindState bodyTextureState = BindTexture(T_copper2);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricTexture(cylinder, GL_TRUE);
	gluQuadricTexture(disk, GL_TRUE);

	// top sphere
	glPushMatrix();
	gluSphere(sphere, 0.1f, slices, stacks);
	glPopMatrix();

	// middle cylinder
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glScalef(5.0f, 0.6f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	gluCylinder(cylinder, 0.1f, 0.11f, 0.07f, slices, stacks);
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// bottom wider cylinder
	glPushMatrix();
	glTranslatef(0.0f, -0.06f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(4.0f, 0.3f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	gluCylinder(cylinder, 0.11f, 0.14f, 0.04f, slices, stacks);
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// curve in
	glPushMatrix();
	glTranslatef(0.0f, -0.1f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glScalef(2.5f, 0.1f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	gluCylinder(cylinder, 0.14f, 0.13f, 0.02f, slices, stacks);
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// sphere ring surrounding bell body 
	glPushMatrix();
	glTranslatef(0.0f, -0.04f, 0.0f);
	for (int i = 0; i < 18; i++)
	{
		float angle = ((float)i / 18.0f) * (2.0f * 3.142f);
		float ringX = 0.11f * cosf(angle);
		float ringZ = 0.11f * sinf(angle);

		glPushMatrix();
		glTranslatef(ringX, 0.0f, ringZ);
		gluSphere(sphere, 0.015f, slices, stacks);
		glPopMatrix();
	}
	glPopMatrix();

   // deplay swing
	if (showBell)
		ApplySwingRotateZ(timeSec, BELL_SWING_SPEED, BELL_SWING_MAX_ANGLE + 5, 0.8f);

	// clapper body
	glPushMatrix();
	glTranslatef(0.0f, -0.08f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, 0.015f, 0.015f, 0.055f, slices, stacks);
	glPopMatrix();

	UnbindTexture(bodyTextureState);

	TextureBindState accentTextureState = BindTexture(T_copper);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricTexture(cylinder, GLU_TRUE);
	gluQuadricTexture(disk, GLU_TRUE);

	// clapper ball
	glPushMatrix();
	glTranslatef(0.0f, -0.155f, 0.0f);
	gluSphere(sphere, 0.03f, slices, stacks);
	glPopMatrix();

    UnbindTexture(accentTextureState);
	gluQuadricTexture(sphere, GL_FALSE);
	gluQuadricTexture(cylinder, GL_FALSE);
	gluQuadricTexture(disk, GL_FALSE);

	glPopMatrix();
}

void drawHair() {
	glColor3fv(COLOR_GREEN);

	// head base ring
	glPushMatrix();
	glTranslatef(0.0f, 0.55f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, 0.2f, 0.215f, 0.1f, slices, stacks);
	glPopMatrix();

	// front hair
	glPushMatrix();

	// front side hair group 
	// mid wide 
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.01f, 0.3f, 0.18f);   // bottom
	glVertex3f(-0.06f, 0.46f, 0.215f);  // left
	glVertex3f(0.03f, 0.46f, 0.215f);   // right
	glEnd();

	// mid narrow 
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.038f, 0.32f, 0.18f);   // bottom
	glVertex3f(0.02f, 0.46f, 0.215f);  // left
	glVertex3f(0.058f, 0.46f, 0.215f);   // right
	glEnd();

	glPopMatrix();

	// front side hair 
	float sideHairRotZ = 350;
	for (int side = 0; side < 2; side++) {
		float sign = (side == 0) ? -1.0f : 1.0f;
		float frontInnerX = (side == 0) ? 0.04f : 0.055f;

		// front side hair piece
		glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(sign * 0.7f, 0.0f, 0.7f);
		glVertex3f(sign * 0.19f, 0.46f, 0.115f);
		glVertex3f(sign * 0.16f, 0.26f, 0.10f);
		glVertex3f(sign * frontInnerX, 0.46f, 0.215f);
		glEnd();

		glPushMatrix();
		glTranslatef(sign * 0.16f, 0.26f, 0.11f);
		glRotatef(sign * sideHairRotZ, 0.0f, 0.0f, 1.0f);
		glTranslatef(-sign * 0.16f, -0.26f, -0.115f);

		// side hair upper 
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(sign * 0.85f, 0.0f, 0.52f);
		glVertex3f(sign * 0.14f, 0.33f, 0.10f);
		glVertex3f(sign * 0.10f, 0.255f, 0.16f);
		glVertex3f(sign * 0.16f, 0.28f, 0.10f);
		glEnd();

		// side hair lower 
		glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(sign * 0.85f, 0.0f, 0.52f);
		glVertex3f(sign * 0.16f, 0.29f, 0.10f);
		glVertex3f(sign * 0.08f, 0.21f, 0.16f);
		glVertex3f(sign * 0.16f, 0.23f, 0.10f);
		glEnd();

		glPopMatrix();
	}

	// side hair wrap
	float hairTopY = 0.46f;
	float hairBottomY = 0.23f;
	float hairRadius = 0.22f;
	float hairBottomRadius = 0.15f;
	int hairStepDeg = 24;

	glBegin(GL_QUAD_STRIP);
	float leftConnA = DegreeToRadian(146.0f);
	glNormal3f(cosf(leftConnA), 0.0f, sinf(leftConnA));
	// left front-side connection
	glVertex3f(-0.18f, hairTopY, 0.12f);
	glVertex3f(-0.14f, hairBottomY, 0.10f);

	for (int a = 146; a <= 394; a += hairStepDeg) {
		float rad = DegreeToRadian((float)a);
		float x = hairRadius * cosf(rad);
		float z = hairRadius * sinf(rad);
		glNormal3f(cosf(rad), 0.0f, sinf(rad));

		glVertex3f(x, hairTopY, z);
		glVertex3f(hairBottomRadius * cosf(rad), hairBottomY, hairBottomRadius * sinf(rad));
	}

	// right front-side connection
	float rightConnA = DegreeToRadian(394.0f);
	glNormal3f(cosf(rightConnA), 0.0f, sinf(rightConnA));
	glVertex3f(0.18f, hairTopY, 0.12f);
	glVertex3f(0.14f, hairBottomY, 0.10f);
	glEnd();

    // hair end: one triangle per wrap quad segment
	float hairTipDrop = 0.06f;
	float hairTipInset = 0.04f;

	glBegin(GL_TRIANGLES);
	for (int a = 146; a + hairStepDeg <= 394; a += hairStepDeg) {
		float radA = DegreeToRadian((float)a);
		float radB = DegreeToRadian((float)(a + hairStepDeg));
		float radMid = DegreeToRadian((float)a + (float)hairStepDeg * 0.5f);

		float xA = hairBottomRadius * cosf(radA);
		float zA = hairBottomRadius * sinf(radA);
		float xB = hairBottomRadius * cosf(radB);
		float zB = hairBottomRadius * sinf(radB);

		float tipRadius = hairBottomRadius - hairTipInset;
		float tipX = tipRadius * cosf(radMid);
		float tipZ = tipRadius * sinf(radMid);
		glNormal3f(cosf(radMid), -0.6f, sinf(radMid));

		glVertex3f(xA, hairBottomY, zA);
		glVertex3f(xB, hairBottomY, zB);
		glVertex3f(tipX, hairBottomY - hairTipDrop, tipZ);
	}
	glEnd();

    // ponytail
	gluQuadricDrawStyle(sphere, GLU_FILL);
	glPushMatrix();
	// first sphere 
	glTranslatef(0.0f, 0.31f, -0.18f);
	gluSphere(sphere, 0.054f, slices, stacks);

	float timeSec = (float)GetTickCount() * 0.001f;
	ApplySwingRotateZ(timeSec, 1.6f, 30);

	// second sphere 
	glTranslatef(0.0f, -0.075f, -0.04f);
	gluSphere(sphere, 0.052f, slices, stacks);

	// third sphere 
	glTranslatef(0.0f, -0.08f, -0.01f);
	gluSphere(sphere, 0.050f, slices, stacks);

	// straight down 
	glTranslatef(0.0f, -0.08f, 0.0f);
	gluSphere(sphere, 0.048f, slices, stacks);
	glTranslatef(0.0f, -0.08f, 0.0f);
	gluSphere(sphere, 0.046f, slices, stacks);

    // lower section has extra delayed swing for a more natural bend
	ApplySwingRotateZ(timeSec, 1.6f, 40, 0.8f);

	glTranslatef(0.0f, -0.08f, 0.0f);
	gluSphere(sphere, 0.044f, slices, stacks);
	glTranslatef(0.0f, -0.08f, 0.0f);
	gluSphere(sphere, 0.042f, slices, stacks);
	glTranslatef(0.0f, -0.08f, -0.02f);
	gluSphere(sphere, 0.042f, slices, stacks);

	ApplySwingRotateZ(timeSec, 1.6f, 60, 0.8f);

	glTranslatef(0.0f, -0.06f, -0.04f);
	gluSphere(sphere, 0.042f, slices, stacks);
	glTranslatef(0.0f, -0.06f, -0.05f);
	gluSphere(sphere, 0.040f, slices, stacks);

	ApplySwingRotateZ(timeSec, 1.6f, 60, 0.8f);

	// gold band + connected tip
	glPushMatrix();
	glTranslatef(0.0f, -0.01f, -0.02f);
	glRotatef(140.0f, 1.0f, 0.0f, 0.0f);
	glColor3fv(COLOR_GOLD);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, 0.012f, 0.014f, 0.05f, slices, stacks);

	// tip 
	glTranslatef(0.0f, 0.0f, 0.02f);
	glColor3fv(COLOR_GREEN);
	gluCylinder(cylinder, 0.01f, 0.046f, 0.12f, slices, stacks);
	glPopMatrix();
	glPopMatrix();


}

void drawTalisman(float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	glPushMatrix();
	glTranslatef(posX, posY, posZ);
	glRotatef(rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
	TextureBindState talismanTextureState = BindTexture(T_talisman);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.00f, 0.00f, 0.00f);
	glTexCoord2f(0.0f, 0.666f);
	glVertex3f(0.00f, -0.07f, 0.00f);
	glTexCoord2f(1.0f, 0.666f);
	glVertex3f(0.06f, -0.07f, 0.00f);
    glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.06f, 0.00f, 0.00f);

	glTexCoord2f(0.0f, 0.666f);
	glVertex3f(0.00f, -0.07f, 0.00f);
	glTexCoord2f(0.0f, 0.333f);
	glVertex3f(0.00f, -0.14f, 0.00f);
	glTexCoord2f(1.0f, 0.333f);
	glVertex3f(0.06f, -0.14f, 0.00f);
	glTexCoord2f(1.0f, 0.666f);
	glVertex3f(0.06f, -0.07f, 0.00f);

	glTexCoord2f(0.0f, 0.333f);
	glVertex3f(0.00f, -0.14f, 0.00f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0.00f, -0.21f, 0.00f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.06f, -0.21f, 0.00f);
	glTexCoord2f(1.0f, 0.333f);
	glVertex3f(0.06f, -0.14f, 0.00f);

	glEnd();
	UnbindTexture(talismanTextureState);
	glPopMatrix();
}



void drawHat()
{
    float hatScale = 0.95f;
	float hatTiltX = 352;
	float hatOffsetY = 0.45f;

    glPushMatrix();
	glTranslatef(0.0f, hatOffsetY, 0.0f);
	glRotatef(hatTiltX, 1.0f, 0.0f, 0.0f);
	glScalef(hatScale, hatScale, hatScale);
    glTranslatef(0.0f, -hatOffsetY, 0.0f);

	// red tip
	glPushMatrix();
	glTranslatef(0.0f, 0.80f, 0.0f);
    glColor3fv(GetCustomizationColor(0));
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluSphere(sphere, 0.07f, slices, stacks);
	glPopMatrix();

	// strip 
	glPushMatrix();
	glTranslatef(0.0f, 0.6f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glColor3fv(COLOR_GOLD);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, 0.35f, 0.35f, 0.01f, slices, stacks);
	glPopMatrix();

	// huge ball
	glPushMatrix();
	glScalef(1.0f, 0.8f, 1.0f);
	glTranslatef(0.0f, 0.75f, 0.0f);
 glColor3fv(GetCustomizationColor(1));
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluSphere(sphere, 0.25f, slices, stacks);
	glPopMatrix();

	// hat body
	glPushMatrix();
	glTranslatef(0.0f, 0.6f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
 glColor3fv(GetCustomizationColor(2));
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, 0.35f, 0.25f, 0.2f, slices, stacks);
	glPopMatrix();

	// close upper hat body
	glPushMatrix();
	glTranslatef(0.0f, 0.6f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
 glColor3fv(GetCustomizationColor(2));
	gluQuadricDrawStyle(disk, GLU_FILL);
	gluDisk(disk, 0.0f, 0.35f, slices, 1);
	glPopMatrix();

	// close bottom of hat body
	glPushMatrix();
	glTranslatef(0.0f, 0.4f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
 glColor3fv(GetCustomizationColor(2));
    gluQuadricDrawStyle(disk, GLU_FILL);
	gluDisk(disk, 0.0f, 0.25f, slices, 1);
	glPopMatrix();

	// talisman on hat body
	drawTalisman(-0.24f, 0.59f, 0.26f, 0.0f, 330.0f, 7.0f);

	glPopMatrix();

}

void drawHead()
{
	// head base
	glPushMatrix();
	glTranslatef(0.0f, 0.48f, 0.0f);
	glScalef(1.0f, 1.5f, 1.0f);
	glColor3fv(COLOR_SKIN_BEIGE);
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluSphere(sphere, 0.20f, slices + 2, stacks + 2);
	glPopMatrix();

	// ears
	for (int side = 0; side < 2; side++) {
		float sign = (side == 0) ? -1.0f : 1.0f;
		glPushMatrix();
		glTranslatef(sign * 0.18f, 0.34f, -0.01f);
		glRotatef(sign * 320, 0.0f, 0.0f, 1.0f);
		glScalef(0.55f, 0.90f, 0.35f);
		glColor3fv(COLOR_SKIN_BEIGE);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.06f, slices, stacks);
		glPopMatrix();
	}


}

void drawBody() {
	// neck 
	glPushMatrix();
	glTranslatef(0.0f, 0.2f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glColor3fv(COLOR_SKIN_BEIGE);
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluCylinder(cylinder, 0.06f, 0.07f, 0.08f, slices, stacks);
	glPopMatrix();

	// torso
	glPushMatrix();
    glTranslatef(0.0f, 0.115f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glColor3fv(COLOR_SKIN_BEIGE);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, 0.07f, 0.16f, 0.45f, slices, stacks);
	glPopMatrix();

	// close torso bottom
	glPushMatrix();
	glTranslatef(0.0f, 0.12f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.45f);
	glColor3fv(COLOR_SKIN_BEIGE);
	gluQuadricDrawStyle(disk, GLU_FILL);
	gluDisk(disk, 0.0f, 0.16f, slices, 1);
	glPopMatrix();

    // legs
	for (int side = 0; side < 2; side++) {
		float sign = (side == 0) ? -1.0f : 1.0f;
		bool isJumpPose = (isJumping || isJumpPoseToggle);

		// upper leg segment
		glPushMatrix();
		ApplyLegPantGroup1Pose(sign, isJumpPose);
		glColor3fv(COLOR_SKIN_BEIGE);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluCylinder(cylinder, 0.042f, 0.038f, 0.12f, slices, stacks);
		glPopMatrix();

		// leg joint
		glPushMatrix();
		ApplyLegPantGroup1Pose(sign, isJumpPose);
		glTranslatef(0.0f, 0.0f, 0.12f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.038f, slices, stacks);
		glPopMatrix();

		// lower leg segment
		glPushMatrix();
		ApplyLegPantGroup2Pose(sign, isJumpPose);
		glColor3fv(COLOR_SKIN_BEIGE);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		glTranslatef(0.0f, 0.0f, -0.02f);
		gluCylinder(cylinder, 0.038f, 0.034f, 0.18f, slices, stacks);
		glPopMatrix();
	}

	// arms (centered inside sleeve)
	for (int side = 0; side < 2; side++) {
		float sign = (side == 0) ? -1.0f : 1.0f;
		bool isJumpPose = (isJumping || isJumpPoseToggle);
		bool isBellArmPose = showBell && side == 1;
		bool isArmForwardPose = isJumpPose || isBellArmPose;

		// upper arm (matches sleeve group 2 pose)
		glPushMatrix();
		ApplyArmGroup2Pose(sign, 0.058f, isArmForwardPose);
		glColor3fv(COLOR_SKIN_BEIGE);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
        gluCylinder(cylinder, 0.042f, 0.040f, 0.15f, slices, stacks);
        glTranslatef(0.0f, 0.0f, 0.15f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.038f, slices, stacks);
		glPopMatrix();

		// lower arm (matches sleeve group 3 pose)
		glPushMatrix();
		ApplyArmGroup3Pose(sign, 0.058f, isArmForwardPose);
		glColor3fv(COLOR_SKIN_BEIGE);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluCylinder(cylinder, 0.040f, 0.036f, 0.30f, slices, stacks);

        glTranslatef(0.0f, 0.0f, 0.30f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.032f, slices, stacks);

		glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.05f);
		glColor3fv(COLOR_SKIN_BEIGE);
		glScalef(1.0f, 0.9f, 1.2f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.05f, slices, stacks);
		glPopMatrix();
		glPopMatrix();

		if (showBell && side == 1)
		{
			glPushMatrix();
			ApplyArmGroup3Pose(sign, 0.058f, isArmForwardPose);
			glTranslatef(0.16f, 0.0f, 0.4f);
			glRotatef(90, 0.0f, 0.0f, 1.0f);
			glScalef(0.5f, 0.5f, 0.5f);
			drawBell();
			glPopMatrix();
		}
	}

}

void drawCloth() {

    // upper robe
	glPushMatrix();
	glTranslatef(0.0f, 0.12f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glColor3fv(GetCustomizationColor(2));
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, 0.10f, 0.21f, 0.36f, slices, stacks);
	glPopMatrix();

	// close upper robe top
	glPushMatrix();
	glTranslatef(0.0f, 0.12f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glColor3fv(GetCustomizationColor(2));
	gluQuadricDrawStyle(disk, GLU_FILL);
	gluDisk(disk, 0.0f, 0.10f, slices, 1);
	glPopMatrix();

	// collar
	glPushMatrix();
	glTranslatef(0.0f, 0.16f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glColor3fv(COLOR_GOLD);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluCylinder(cylinder, 0.08f, 0.08f, 0.05f, slices, stacks);
	glPopMatrix();

    // necklace
    glColor3fv(GetCustomizationColor(0));
	glPushMatrix();
	glTranslatef(0.0f, 0.11f, 0.045f);
    glRotatef(105.0f, 1.0f, 0.0f, 0.0f);
    for (int i = 0; i < 28; i++)
	{
		float angle = ((float)i / 28.0f) * (2.0f * 3.142f);
		float beadX = 0.09f * cosf(angle);
		float beadY = 0.13f * sinf(angle);

		glPushMatrix();
		glTranslatef(beadX, beadY, 0.0f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.013f, slices, stacks);
		glPopMatrix();
	}
	glPopMatrix();


	// lower robe 
	TextureBindState clothTextureState = BindTexture(GetCustomizationTexture(), cylinder);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glScalef(3.0f, 0.2f, 1.0f);
	glRotatef(10, 0.0f, 1.0f, 0.0f);
	glMatrixMode(GL_MODELVIEW);
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	glTranslatef(0.0f, -0.24f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, 0.21f, 0.23f, 0.14f, slices, stacks);
	glPopMatrix();

	// lower robe - curve in
	glPushMatrix();
	glTranslatef(0.0f, -0.38f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(cylinder, 0.23f, 0.21f, 0.03f, slices, stacks);
	glPopMatrix();

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

    UnbindTexture(clothTextureState, cylinder);

	// close bottom robe 
	glPushMatrix();
	glTranslatef(0.0f, -0.36f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.03f);
    glColor3fv(GetCustomizationColor(0));
	gluQuadricDrawStyle(disk, GLU_FILL);
	gluDisk(disk, 0.0f, 0.21f, slices, 1);
	glPopMatrix();

	// embroidery
	GLuint currentEmbroideryTexture = GetCustomizationTexture();

	TextureBindState embroideryTextureState = BindTexture(currentEmbroideryTexture);
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.075f, 0.08f, 0.112f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, -0.2f, 0.198f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.1f, -0.2f, 0.198f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.075f, 0.08f, 0.112f);
	glEnd();
	glPopMatrix();

	UnbindTexture(embroideryTextureState);

	// talismans on front cloth 
	drawTalisman(-0.08f, 0.08f, 0.11f, -22, -8, -32);
	drawTalisman(0.08f, -0.18f, 0.199f, -22, 8, 24);
	// talismans on back cloth 
	drawTalisman(0.13f, 0.07f, -0.052f, 12, -16, 18);
	drawTalisman(-0.12f, -0.12f, -0.128f, 28, 46, -26);

}

void drawSleeve() {
	glPushMatrix();
	glTranslatef(0.00f, -0.04f, 0.0f);

	for (int side = 0; side < 2; side++) {
		float sign = (side == 0) ? -1.0f : 1.0f;
      bool isJumpPose = (isJumping || isJumpPoseToggle);
		bool isBellArmPose = showBell && side == 1;
		bool isArmForwardPose = isJumpPose || isBellArmPose;

		// 1st group: joint - shoulder connection, shoulder, joint - upper sleeve
		glPushMatrix();
       ApplyArmGroup1Pose(sign, 0.098f, isArmForwardPose);
 glColor3fv(GetCustomizationColor(1));
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.055f, slices, stacks);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluCylinder(cylinder, 0.07f, 0.06f, 0.12f, slices, stacks);
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.11f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.058f, slices, stacks);
		glPopMatrix();
		glPopMatrix();

		// 2nd group: upper sleeve, joint - sleeve
		glPushMatrix();
       ApplyArmGroup2Pose(sign, 0.098f, isArmForwardPose);
 glColor3fv(GetCustomizationColor(1));
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluCylinder(cylinder, 0.06f, 0.08f, 0.15f, slices, stacks);
		glTranslatef(0.0f, 0.0f, 0.15f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.078f, slices, stacks);
		glPopMatrix();

		// 3rd group: lower sleeve, gold cuff
		glPushMatrix();
		ApplyArmGroup3Pose(sign, 0.098f, isArmForwardPose);
		glColor3fv(GetCustomizationColor(1));
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluCylinder(cylinder, 0.08f, 0.10f, 0.15f, slices, stacks);
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.15f);

		TextureBindState sleeveGoldTextureState = BindTexture(GetCustomizationTexture(), cylinder);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glLoadIdentity();
		glScalef(1.2f, 0.5f, 0.0f);
		glMatrixMode(GL_MODELVIEW);

		glColor3f(1.0f, 1.0f, 1.0f);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluCylinder(cylinder, 0.10f, 0.14f, 0.15f, slices, stacks); // cuff
        glMatrixMode(GL_TEXTURE);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		UnbindTexture(sleeveGoldTextureState, cylinder);
		glPopMatrix();
		glPopMatrix();
	}

	glPopMatrix();
}

void drawPants() {
    for (int side = 0; side < 2; side++) {
		float sign = (side == 0) ? -1.0f : 1.0f;
		bool isJumpPose = (isJumping || isJumpPoseToggle);

		// upper + joint (move together)
		glPushMatrix();
		ApplyLegPantGroup1Pose(sign, isJumpPose);
    glColor3fv(GetCustomizationColor(0));
		gluQuadricDrawStyle(cylinder, GLU_FILL);
        gluCylinder(cylinder, 0.078f, 0.09f, 0.16f, slices, stacks);

		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.13f);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.0895f, slices, stacks);
		glPopMatrix();
		glPopMatrix();

		// lower pant + cuff + gold strip (move together)
		glPushMatrix();
		ApplyLegPantGroup2Pose(sign, isJumpPose);

		// lower pant
		glColor3fv(GetCustomizationColor(0));
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.08f);
		gluCylinder(cylinder, 0.09f, 0.09f, 0.12f, slices, stacks);
		glPopMatrix();

		//cuff
		glColor3fv(GetCustomizationColor(0));
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.11f, 6, 6);

		// shoes
		glPushMatrix();
        glTranslatef(0.0f, 0.04f, 0.17f);
		glRotatef(95.0f, 1.0f, 0.0f, 0.0f);
		glScalef(1.2f, 0.8f, 1.6f);
		glColor3fv(COLOR_CHARCOAL);
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluSphere(sphere, 0.07f, 8, 8);
		glPopMatrix();

		// gold strip
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.08f);
		glColor3fv(COLOR_GOLD);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		gluCylinder(cylinder, 0.05f, 0.05f, 0.045f, slices, stacks);
		glPopMatrix();

		glPopMatrix();
	}
}

void drawFace() {
	const float eyeZOffset = -0.03f;
	const float eyeZBase = 0.202f;
	const float eyeZFront = eyeZBase + 0.001f;
	const float eyeCenterX = 0.08f; 
	bool isBiteFace = (selectedFace == FACE_BITE);
	bool isQuestionFace = (selectedFace == FACE_QUESTION);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, eyeZOffset);

	auto drawMirroredEye = [&](float sign)
		{
            float eyeTiltX = 16;
			float eyeAnchorY = 0.34f;
			glPushMatrix();
			glTranslatef(sign * eyeCenterX, eyeAnchorY, eyeZBase);
			glRotatef(eyeTiltX, 1.0f, 0.0f, 0.0f);
			glTranslatef(-sign * eyeCenterX, -eyeAnchorY, -eyeZBase);
			glNormal3f(0.0f, 0.0f, 1.0f);

			// eyebrows
			glColor3fv(COLOR_GREEN);
			glLineWidth(2.5f);
			glBegin(GL_LINE_STRIP);
			glVertex3f(sign * (eyeCenterX - 0.045f), 0.375f, eyeZBase);
			glVertex3f(sign * (eyeCenterX - 0.020f), 0.38f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.055f), 0.385f, eyeZBase);
		
			glEnd();

			// double eyelid
			glColor3fv(COLOR_SKIN_BROWN);
			glLineWidth(1.2f);
			glBegin(GL_LINE_STRIP);
			glVertex3f(sign * (eyeCenterX - 0.03f), 0.368f, eyeZBase + 0.008f);
			glVertex3f(sign * (eyeCenterX - 0.015f), 0.37f, eyeZBase + 0.008f);
			glVertex3f(sign * (eyeCenterX - 0.005f), 0.37f, eyeZBase + 0.008f);
		
			glEnd();

			// eyeline
			glColor3fv(COLOR_CHARCOAL);
			glLineWidth(3.0f);
			glBegin(GL_LINE_STRIP);
			glVertex3f(sign * (eyeCenterX - 0.045f), 0.35f, eyeZBase);
			glVertex3f(sign * (eyeCenterX - 0.025f), 0.36f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.015f), 0.36f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.045f), 0.35f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.055f), 0.34f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.045f), 0.32f, eyeZBase);
			glEnd();

			glColor3fv(COLOR_CHARCOAL);
			glLineWidth(4.0f);
			glBegin(GL_LINE_STRIP);
			glVertex3f(sign * (eyeCenterX + 0.045f), 0.35f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.055f), 0.34f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.045f), 0.32f, eyeZBase);
			glEnd();

			// eye white
			glPushMatrix();
			glBegin(GL_POLYGON);
			glColor3fv(COLOR_WHITE);
			glVertex3f(sign * (eyeCenterX - 0.045f), 0.35f, eyeZBase);
			glVertex3f(sign * (eyeCenterX - 0.025f), 0.36f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.015f), 0.36f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.045f), 0.35f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.055f), 0.34f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.045f), 0.32f, eyeZBase);
			glVertex3f(sign * (eyeCenterX + 0.025f), 0.31f, eyeZBase);
			glVertex3f(sign * (eyeCenterX - 0.005f), 0.313f, eyeZBase);
			glVertex3f(sign * (eyeCenterX - 0.035f), 0.322f, eyeZBase);
			glEnd();
			glPopMatrix();

			// pupil
			glPushMatrix();
            if (isBiteFace) {
				TextureBindState spiralTextureState = BindTexture(T_beam, disk);
				glColor3f(1.0f, 1.0f, 1.0f);
				glTranslatef(sign * eyeCenterX, 0.333f, eyeZFront);
				glScalef(0.025f, 0.022f, 1.0f);
				gluQuadricDrawStyle(disk, GLU_FILL);
				gluDisk(disk, 0.0f, 1.0f, slices, 1);
				UnbindTexture(spiralTextureState, disk);
			}
			if (isQuestionFace) {
				TextureBindState spiralTextureState = BindTexture(T_spiral, disk);
				glColor3f(1.0f, 1.0f, 1.0f);
				glTranslatef(sign * eyeCenterX, 0.333f, eyeZFront);
				glScalef(0.025f, 0.022f, 1.0f);
				gluQuadricDrawStyle(disk, GLU_FILL);
				gluDisk(disk, 0.0f, 1.0f, slices, 1);
				UnbindTexture(spiralTextureState, disk);
			}
			else // default
			{
				glColor3fv(COLOR_CHARCOAL);
				glTranslatef(sign * eyeCenterX, 0.333f, eyeZFront);
				glScalef(0.022f, 0.022f, 1.0f);
				gluQuadricDrawStyle(disk, GLU_FILL);
				gluDisk(disk, 0.0f, 1.0f, slices, 1);
			}
			glPopMatrix();

			
			glPopMatrix();
		};

	drawMirroredEye(-1.0f);
	drawMirroredEye(1.0f);

	// blush
    if (isBiteFace || isQuestionFace) {
		
	}
	else
	{
    glColor3fv(COLOR_MAROON);
		for (int side = 0; side < 2; side++)
		{
			float sign = (side == 0) ? -1.0f : 1.0f;
			glPushMatrix();
			glTranslatef(sign * 0.09f, 0.28f, 0.19f);
			glRotatef(36.0f, 1.0f, 0.0f, 0.0f);
			gluQuadricDrawStyle(disk, GLU_FILL);
			gluDisk(disk, 0.0f, 0.03f, slices, 1);
			glPopMatrix();
		}
	}

   //  mouth
	glColor3fv(COLOR_CHARCOAL);
	if (isBiteFace)
	{
		glPushMatrix();
		glTranslatef(0.0f, 0.245f, 0.1545f);
		glRotatef(36, 1.0f, 0.0f, 0.0f);
		glScalef(1.0f, 1.2f, 1.0f);
		glColor3fv(COLOR_MAROON);
		gluQuadricDrawStyle(disk, GLU_FILL);
		gluDisk(disk, 0.0f, 0.035f, slices, 1);
		glPopMatrix();

		// teeth
		glColor3fv(COLOR_WHITE);
		for (int side = 0; side < 2; side++)
		{
			float sign = (side == 0) ? -1.0f : 1.0f;
			glPushMatrix();
			glTranslatef(sign * 0.018f, 0.245f, 0.178f);
			glRotatef(-104.0f, 1.0f, 0.0f, 0.0f);
			gluQuadricDrawStyle(cylinder, GLU_FILL);
			gluCylinder(cylinder, 0.0f, 0.006f, 0.03f, slices, 6);
			glPopMatrix();
		}
	}
    else if (isQuestionFace)
	{
		glPushMatrix();
		glTranslatef(0.0f, 0.245f, 0.16f);
		glRotatef(38, 1.0f, 0.0f, 0.0f);
        glColor3fv(COLOR_MAROON);
		glBegin(GL_QUADS);
		glVertex3f(-0.02f, 0.03f, 0.0f);	
		glVertex3f(-0.03f, -0.03f, 0.0f);
		glVertex3f(0.03f, -0.03f, 0.0f);
		glVertex3f(0.02f, 0.03f, 0.0f);
		glEnd();
		glPopMatrix();
	}
	else // default w
	{
		const float mouthY = 0.25f;
		const float mouthZ = 0.16f;
		const float archRadius = 0.013f;
		const int archSegments = 12;

		glLineWidth(3.0f);

		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= archSegments; i++)
		{
			float t = (float)i / (float)archSegments;
			float angle = 3.142f - (3.142f * t);
			float x = -0.013f + cosf(angle) * archRadius;
            float y = mouthY - sinf(angle) * archRadius;
			glVertex3f(x, y, mouthZ);
		}
		glEnd();

		glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= archSegments; i++)
		{
			float t = (float)i / (float)archSegments;
			float angle = 3.142f - (3.142f * t);
			float x = 0.013f + cosf(angle) * archRadius;
            float y = mouthY - sinf(angle) * archRadius;
			glVertex3f(x, y, mouthZ);
		}
		glEnd();
	}

	glPopMatrix();

}



void drawCoffin()
{
	float innerHalf = 0.28f;
	float wallRadius = innerHalf;
	const float bodyLength = 2.3f;

	glPushMatrix();
	glColor3fv(COLOR_BROWN);
	glTranslatef(0.0f, bodyLength, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	TextureBindState coffinTextureState = BindTexture(T_wood);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glColor3f(1.0f, 1.0f, 1.0f);


	// front
	glPushMatrix();
	glTranslatef(0.0f, innerHalf, 0.0f);
	drawHalfCylinder(wallRadius, wallRadius, bodyLength, slices, true, true, true);
	glPopMatrix();

	// back
	glPushMatrix();
	glTranslatef(0.0f, -innerHalf, 0.0f);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	drawHalfCylinder(wallRadius, wallRadius, bodyLength, slices, true, true, true);
	glPopMatrix();

	// right
	glPushMatrix();
	glTranslatef(innerHalf, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
	drawHalfCylinder(wallRadius, wallRadius, bodyLength, slices, true, true, true);
	glPopMatrix();

	// left
	glPushMatrix();
	glTranslatef(-innerHalf, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	drawHalfCylinder(wallRadius, wallRadius, bodyLength, slices, true, true, true);
	glPopMatrix();

	glBegin(GL_QUADS);
	// bottom cover 
	glNormal3f(0.0f, 0.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-innerHalf, -innerHalf, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(innerHalf, -innerHalf, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(innerHalf, innerHalf, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-innerHalf, innerHalf, 0.0f);

	// top cover 
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-innerHalf, -innerHalf, bodyLength);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-innerHalf, innerHalf, bodyLength);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(innerHalf, innerHalf, bodyLength);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(innerHalf, -innerHalf, bodyLength);
	glEnd();

	UnbindTexture(coffinTextureState);
	glPopMatrix();
}

void drawQuestionmark() {
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.2f);
	auto drawCube = [&](float cx, float cy, float cz, float size)
	{
		float h = size * 0.5f;
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(cx - h, cy - h, cz + h);
		glVertex3f(cx + h, cy - h, cz + h);
		glVertex3f(cx + h, cy + h, cz + h);
		glVertex3f(cx - h, cy + h, cz + h);

		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(cx + h, cy - h, cz - h);
		glVertex3f(cx - h, cy - h, cz - h);
		glVertex3f(cx - h, cy + h, cz - h);
		glVertex3f(cx + h, cy + h, cz - h);

		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(cx - h, cy - h, cz - h);
		glVertex3f(cx - h, cy - h, cz + h);
		glVertex3f(cx - h, cy + h, cz + h);
		glVertex3f(cx - h, cy + h, cz - h);

		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(cx + h, cy - h, cz + h);
		glVertex3f(cx + h, cy - h, cz - h);
		glVertex3f(cx + h, cy + h, cz - h);
		glVertex3f(cx + h, cy + h, cz + h);

		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(cx - h, cy + h, cz + h);
		glVertex3f(cx + h, cy + h, cz + h);
		glVertex3f(cx + h, cy + h, cz - h);
		glVertex3f(cx - h, cy + h, cz - h);

		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(cx - h, cy - h, cz - h);
		glVertex3f(cx + h, cy - h, cz - h);
		glVertex3f(cx + h, cy - h, cz + h);
		glVertex3f(cx - h, cy - h, cz + h);
		glEnd();
	};

	float cubeSize = 0.06f;
	float spacing = 0.06f;
	int blocks[][2] =
	{
		{0, 6}, {1, 6}, {2, 6},
		{0, 5},{2, 5},
		{1, 4}, {2, 4},
		{1, 3},
		{1, 1}
	};

	int count = sizeof(blocks) / sizeof(blocks[0]);
	for (int i = 0; i < count; i++)
	{
		float x = (blocks[i][0] - 1.0f) * spacing;
		float y = (blocks[i][1] - 3.0f) * spacing;
		drawCube(x, y, 0.0f, cubeSize);
	}

	glPopMatrix();
}

void drawSummonedCoffins()
{
	if (!isSummonActive)
		return;

	glPushMatrix();
	glTranslatef(-2.2f, summonedCoffinLiftY, -2.0f);
	drawCoffin();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, summonedCoffinLiftY, -0.8f);
	drawCoffin();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.2f, summonedCoffinLiftY, -2.0f);
	drawCoffin();
	glPopMatrix();
}


void drawJiangshi()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.75f, 0.40f);

	glPushMatrix();
	applyInnerPivotXRotateY(headAngle, 0.22f);
	drawHat();
	drawHead();
	drawFace();
	drawHair();
	glPopMatrix();

	drawBody();
	drawCloth();
	drawSleeve();
	drawPants();

	if (selectedFace == FACE_QUESTION)
	{
		glPushMatrix();
		glTranslatef(-0.15f, -0.3f, -0.4f);
		glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
		drawQuestionmark();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, -0.0f, -0.4f);
		drawQuestionmark();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.15f, -0.3f, -0.4f);
		glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
		drawQuestionmark();
		glPopMatrix();
	}

	glPopMatrix();
}


void Display()
{
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------
	glViewport(0, 0, windowWidth, windowHeight);

	// set up camera
	setupProjection();

	// Clear Screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	setupCameraView();
	setupLight();
	setupRenderState();

	drawBackground();

    // Shadow matrix 
	GLfloat lx3 = diffuseLightPosition3[0];
	GLfloat ly3 = diffuseLightPosition3[1];
	GLfloat lz3 = diffuseLightPosition3[2];

	GLfloat matrix3[16] =
	{
		ly3,	0,		0,		0,
		-lx3,   0,		-lz3,	-1,
		0,		0,		ly3,	0,
		0,		0,		0,		ly3
	};

	switch (questionNum)
	{

		 case 1:
			{
				float currentJumpOffsetY = isJumpPoseToggle ? JUMP_POSE_PREVIEW_Y : jumpOffsetY;
				glPushMatrix();
				glTranslatef(posX, posY + currentJumpOffsetY, posZ);
				glRotatef(rotX, 1.0f, 0.0f, 0.0f);
				glRotatef(rotY, 0.0f, 1.0f, 0.0f);
				glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
				glRotatef(characterFacingY, 0.0f, 1.0f, 0.0f);

				drawJiangshi();
				glPopMatrix();
				drawSummonedCoffins();
				drawTalismanRain();

				GLboolean wasTexture2DEnabled = glIsEnabled(GL_TEXTURE_2D);
				GLboolean wasLightingEnabled = glIsEnabled(GL_LIGHTING);
				GLboolean wasColorMaterialEnabled = glIsEnabled(GL_COLOR_MATERIAL);

				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_LIGHTING);
				glDisable(GL_COLOR_MATERIAL);
				glEnable(GL_COLOR_LOGIC_OP);
				glLogicOp(GL_CLEAR);

				// draw the shadow object (only light 3 casts shadow)
				if (isLightOn)
				{
					glPushMatrix();
					glTranslatef(0.0f, 0.001f, 0.0f); // to prevent z-fighting/ glitching
					glMultMatrixf(matrix3);
					glTranslatef(posX, posY + currentJumpOffsetY, posZ);
					glRotatef(rotX, 1.0f, 0.0f, 0.0f);
					glRotatef(rotY, 0.0f, 1.0f, 0.0f);
					glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
					glRotatef(characterFacingY, 0.0f, 1.0f, 0.0f);

					glColor3f(0.0f, 0.0f, 0.0f);
					drawJiangshi();
					glPopMatrix();
				}

             glDisable(GL_COLOR_LOGIC_OP);

				if (wasColorMaterialEnabled)
					glEnable(GL_COLOR_MATERIAL);
				if (wasLightingEnabled)
					glEnable(GL_LIGHTING);
				if (wasTexture2DEnabled)
					glEnable(GL_TEXTURE_2D);

				break;
		 }
		case 2:
		{
			float currentJumpOffsetY = isJumpPoseToggle ? JUMP_POSE_PREVIEW_Y : jumpOffsetY;
			glPushMatrix();
			glTranslatef(posX, posY + currentJumpOffsetY, posZ);
			glRotatef(rotX, 1.0f, 0.0f, 0.0f);
			glRotatef(rotY, 0.0f, 1.0f, 0.0f);
			glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
			glTranslated(0.0f, 1.0f, 0.0f);
			glScaled(2.0f, 2.0f, 2.0f);
			drawBell();

			glPopMatrix();
			break;
		}

	}

   if (showLightVisualizer)
	{
		drawLightVisualizer(lightX, lightY, lightZ, diffuseLightColour1);
		drawLightVisualizer(light2X, light2Y, light2Z, diffuseLightColour2);
		drawLightVisualizer(light3X, light3Y, light3Z, diffuseLightColour3);
	}

	drawKeybindOverlay();
	//--------------------------------
	//	End of OpenGL drawing
	//--------------------------------
}

//--------------------------------------------------------------------

int WINAPI WinMain(
	_In_ HINSTANCE hInst,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR cmdlparameter,
	_In_ int nCmdShow
)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = WindowProcedure;
	wc.lpszClassName = CLASS_TITLE;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wc)) return false;


	HWND hWnd = CreateWindow(CLASS_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	// DC = Display context (pass it to handler like a pointer)
	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	InitPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	// to tell OS which instance we are specifying at (we may open many different window)
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	fontBase = glGenLists(96);
	if (fontBase != 0)
		wglUseFontBitmapsA(hdc, 32, 96, fontBase);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	//glShadeModel(GL_SMOOTH);

	InitTextures();
	InitAudio();
	StartBgmLoop();

	//--------------------------------
	//	End initialization
	//--------------------------------

	nCmdShow = SW_MAXIMIZE; // maximize the window by default
	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (true) // message loop
	{
		// PeekMessage = to ask OS any message to the window
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg); // SEND THIS MESSAGE TO WindowProcedure
		}

		UpdateAnimation();

		Display();

		SwapBuffers(hdc);
	}

	StopBgm();

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
