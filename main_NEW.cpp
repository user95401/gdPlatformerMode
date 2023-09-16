#include "mod_utils.hpp"
#include "hooks.hpp"
#include "SimpleIni.h"
using namespace std;
using namespace gd;
using namespace cocos2d;
using namespace cocos2d::extension;

float deltaTime;

bool dispatchKeyboardIsAlive;
bool KEY_A_IS_PRESSED;
bool KEY_D_IS_PRESSED;
bool KEY_Left_IS_PRESSED;
bool KEY_Right_IS_PRESSED;
bool KEY_F3_IS_PRESSED;
int lastKey1;
int lastKey2;

CSimpleIni cfg;
bool weHaveGeode;

class PlatformerMod {
public:
    inline static bool enableMe;// >:3
    inline static GJBaseGameLayer* gameLayer;
    inline static float oldSpeed1;
    inline static float oldSpeed2;
    inline static double smoothingThing1;
    inline static double moveSmoothingThing1;
    inline static double smoothingThing2;
    inline static double moveSmoothingThing2;
    inline static double camXoffset;
    inline static bool wasPlayedJumpAnim = false;// >:3
    inline static bool laterally = false;// >:3
    inline static void updateSpeed() {
        if (KEY_A_IS_PRESSED) {
            lastKey1 = KEY_A;
            //gameLayer->m_pPlayer1->m_playerSpeed = -fabs(oldSpeed1);
            smoothingThing1 = 0.6; //restore smoothingThing
            moveSmoothingThing1 = (fabs(moveSmoothingThing1) + (deltaTime * 5));
            if (fabs(moveSmoothingThing1) >= 0.6) gameLayer->m_pPlayer1->m_playerSpeed = (-oldSpeed1); //speed method
            else gameLayer->m_pPlayer1->m_playerSpeed = -fabs(moveSmoothingThing1);
        }
        else if (KEY_D_IS_PRESSED) {
            lastKey1 = KEY_D;
            //gameLayer->m_pPlayer1->m_playerSpeed = fabs(oldSpeed1);
            smoothingThing1 = 0.6; //restore smoothingThing
            moveSmoothingThing1 = (fabs(moveSmoothingThing1) + (deltaTime * 5));
            if (fabs(moveSmoothingThing1) >= 0.6) gameLayer->m_pPlayer1->m_playerSpeed = fabs(oldSpeed1); //speed method
            else gameLayer->m_pPlayer1->m_playerSpeed = fabs(moveSmoothingThing1);
        }
        if (KEY_Left_IS_PRESSED) {
            lastKey2 = KEY_Left;
            //gameLayer->m_pPlayer2->m_playerSpeed = -fabs(oldSpeed2);
            smoothingThing2 = 0.6; //restore smoothingThing
            moveSmoothingThing2 = (fabs(moveSmoothingThing2) + (deltaTime * 5));
            if (fabs(moveSmoothingThing2) >= 0.6) gameLayer->m_pPlayer2->m_playerSpeed = (-oldSpeed2); //speed method
            else gameLayer->m_pPlayer2->m_playerSpeed = -fabs(moveSmoothingThing2);
        }
        else if (KEY_Right_IS_PRESSED) {
            lastKey2 = KEY_Right;
            //gameLayer->m_pPlayer2->m_playerSpeed = fabs(oldSpeed2);
            smoothingThing2 = 0.6; //restore smoothingThing
            moveSmoothingThing2 = (fabs(moveSmoothingThing2) + (deltaTime * 5));
            if (fabs(moveSmoothingThing2) >= 0.6) gameLayer->m_pPlayer2->m_playerSpeed = fabs(oldSpeed2); //speed method
            else gameLayer->m_pPlayer2->m_playerSpeed = fabs(moveSmoothingThing2);
        }
        //idle
        if (!KEY_A_IS_PRESSED && !KEY_D_IS_PRESSED) {
            //gameLayer->m_pPlayer1->m_playerSpeed = 0;
            moveSmoothingThing1 = 0.0; //restore moveSmoothingThing
            //smoothingThing
            if (fabs(smoothingThing1) > 0.0) smoothingThing1 = (fabs(smoothingThing1) - (deltaTime * 5)); //
            if (fabs(smoothingThing1) < 0.1) smoothingThing1 = 0.0; //презерватив
            if (lastKey1 == KEY_A) smoothingThing1 = -fabs(smoothingThing1);
            if (lastKey1 == KEY_D) smoothingThing1 = fabs(smoothingThing1);
            gameLayer->m_pPlayer1->m_playerSpeed = smoothingThing1; //speed method
        }
        if (!KEY_Left_IS_PRESSED && !KEY_Right_IS_PRESSED) {
            //gameLayer->m_pPlayer2->m_playerSpeed = 0;
            moveSmoothingThing2 = 0.0; //restore moveSmoothingThing
            //smoothingThing
            if (fabs(smoothingThing2) > 0.0) smoothingThing2 = (fabs(smoothingThing2) - (deltaTime * 5)); //
            if (fabs(smoothingThing2) < 0.1) smoothingThing2 = 0.0; //презерватив
            if (lastKey2 == KEY_Left) smoothingThing2 = -fabs(smoothingThing2);
            if (lastKey2 == KEY_Right) smoothingThing2 = fabs(smoothingThing2);
            gameLayer->m_pPlayer2->m_playerSpeed = smoothingThing2; //speed method
        }
    }
    inline static void updateRotation() {
        //stop Rotation
        if (!KEY_A_IS_PRESSED && !KEY_D_IS_PRESSED && !gameLayer->m_pPlayer1->m_isDashing) {
            gameLayer->m_pPlayer1->stopRotation(false);
            if (gameLayer->m_pPlayer1->m_isShip) {
                if (fabs(gameLayer->m_pPlayer1->m_playerSpeed) > 0.01) {
                    auto rotAction1 = CCRotateTo::create(0.1, lastKey1 == KEY_A ? 180 : 0);
                    rotAction1->setTag(777);
                    if (!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->runAction(rotAction1);
                }
                else {
                    if (!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->setRotation(lastKey1 == KEY_A ? 180 : 0);
                }
            }
        }
        if (!KEY_Left_IS_PRESSED && !KEY_Right_IS_PRESSED && !gameLayer->m_pPlayer2->m_isDashing) {
            gameLayer->m_pPlayer2->stopRotation(false);
            if (gameLayer->m_pPlayer2->m_isShip) {
                if (fabs(gameLayer->m_pPlayer2->m_playerSpeed) > 0.01) {
                    auto rotAction2 = CCRotateTo::create(0.1, lastKey2 == KEY_Left ? 180 : 0);
                    rotAction2->setTag(778);
                    if (!gameLayer->m_pPlayer2->getActionByTag(778)) gameLayer->m_pPlayer2->runAction(rotAction2);
                }
                else {
                    gameLayer->m_pPlayer2->setRotation(lastKey2 == KEY_Left ? 180 : 0);
                }
            }
        }
        //bird
        if (gameLayer->m_pPlayer1->m_isBird) 
            gameLayer->m_pPlayer1->setRotation(0);
        if (gameLayer->m_pPlayer2->m_isBird || (gameLayer->m_pPlayer2->isCube() && gameLayer->m_pPlayer2->m_isOnGround)) 
            gameLayer->m_pPlayer2->setRotation(0);
        //bot and spider in 3d 4k october
        if (gameLayer->m_pPlayer1->m_isRobot || gameLayer->m_pPlayer1->m_isSpider && gameLayer->m_pPlayer1->m_isOnGround) {
            auto rotAction = CCRotateTo::create(0.05, 0);
            rotAction->setTag(777);
            if (!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->runAction(rotAction);
        }
        if (gameLayer->m_pPlayer2->m_isRobot || gameLayer->m_pPlayer2->m_isSpider && gameLayer->m_pPlayer2->m_isOnGround) {
            auto rotAction = CCRotateTo::create(0.05, 0);
            rotAction->setTag(777);
            if (!gameLayer->m_pPlayer2->getActionByTag(777)) gameLayer->m_pPlayer2->runAction(rotAction);
        }
        //fucks cube
        if ((gameLayer->m_pPlayer1->isCube()) && gameLayer->m_pPlayer1->m_isOnGround) {
            float rot = gameLayer->m_pPlayer1->getRotation();
            float torot = 0;
            if (gameLayer->m_pPlayer1->getRotation() < 0) rot = fabs(rot);
            if (rot < 90 - 50) torot = 0;
            else if (rot < 180 - 50) torot = 90;
            else if (rot < 360 - 50) torot = 180;
            else if (rot < 420) torot = 360;
            else torot = 0;
            if (gameLayer->m_pPlayer1->getRotation() < 0) torot = -torot;
            auto rotAction = CCRotateTo::create(0.05, torot);
            rotAction->setTag(777);
            if(!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->runAction(rotAction);
        }
        if ((gameLayer->m_pPlayer2->isCube()) && gameLayer->m_pPlayer2->m_isOnGround) {
            float rot = gameLayer->m_pPlayer2->getRotation();
            float torot = 0;
            if (gameLayer->m_pPlayer2->getRotation() < 0) rot = fabs(rot);
            if (rot < 90 - 50) torot = 0;
            else if (rot < 180 - 50) { torot = 90; }
            else if (rot < 360 - 50) torot = 180;
            else if (rot < 420) torot = 360;
            else torot = 0;
            if (gameLayer->m_pPlayer2->getRotation() < 0) torot = -torot;
            auto rotAction = CCRotateTo::create(0.05, torot);
            rotAction->setTag(777);
            if (!gameLayer->m_pPlayer2->getActionByTag(777)) gameLayer->m_pPlayer2->runAction(rotAction);
        }
        //update laterally boolean
        float rot42 = fabs(gameLayer->m_pPlayer1->getRotation());
        if (bool(rot42 > 80 && rot42 < 100) || bool(rot42 > 260 && rot42 < 280)) laterally = true;
        else laterally = false;
    }
    inline static void particlesUpdate() {
        //deactivateParticle
        if (!KEY_A_IS_PRESSED && !KEY_D_IS_PRESSED) gameLayer->m_pPlayer1->deactivateParticle();
        gameLayer->m_pPlayer1->m_vehicleGroundParticles->setVisible(!(!KEY_A_IS_PRESSED && !KEY_D_IS_PRESSED));
        if (!KEY_Left_IS_PRESSED && !KEY_Right_IS_PRESSED) gameLayer->m_pPlayer2->deactivateParticle();
        gameLayer->m_pPlayer2->m_vehicleGroundParticles->setVisible(!(!KEY_Left_IS_PRESSED && !KEY_Right_IS_PRESSED));
    }
    inline static void scaleUpdate() {
        if (KEY_A_IS_PRESSED && !gameLayer->m_pPlayer1->m_isDart) {
            if (!gameLayer->m_pPlayer1->m_isDart && !gameLayer->m_pPlayer1->m_isShip) gameLayer->m_pPlayer1->setScaleX(-fabs(gameLayer->m_pPlayer1->m_vehicleSize));
            //ship flipping
            if (gameLayer->m_pPlayer1->m_isShip && GameManager::sharedState()->getPlayerShip() != 170) {
                if (!gameLayer->m_pPlayer1->m_isUpsideDown) gameLayer->m_pPlayer1->setScaleY(-gameLayer->m_pPlayer1->m_vehicleSize);
                else gameLayer->m_pPlayer1->setScaleY(fabs(gameLayer->m_pPlayer1->m_vehicleSize));
            }
        }
        if (KEY_Left_IS_PRESSED && !gameLayer->m_pPlayer2->m_isDart) {
            if (!gameLayer->m_pPlayer2->m_isDart && !gameLayer->m_pPlayer2->m_isShip) gameLayer->m_pPlayer2->setScaleX(-fabs(gameLayer->m_pPlayer2->m_vehicleSize));
            //ship flipping
            if (gameLayer->m_pPlayer2->m_isShip && GameManager::sharedState()->getPlayerShip() != 170) {
                if (!gameLayer->m_pPlayer2->m_isUpsideDown) gameLayer->m_pPlayer2->setScaleY(-gameLayer->m_pPlayer2->m_vehicleSize);
                else gameLayer->m_pPlayer2->setScaleY(fabs(gameLayer->m_pPlayer2->m_vehicleSize));
            }
        }
        if (KEY_D_IS_PRESSED) {
            if (!gameLayer->m_pPlayer1->m_isDart && !gameLayer->m_pPlayer1->m_isShip) gameLayer->m_pPlayer1->setScaleX(fabs(gameLayer->m_pPlayer1->m_vehicleSize));
            //ship flipping
            if (gameLayer->m_pPlayer1->m_isShip && GameManager::sharedState()->getPlayerShip() != 170) {
                if (!gameLayer->m_pPlayer1->m_isUpsideDown) gameLayer->m_pPlayer1->setScaleY(fabs(gameLayer->m_pPlayer1->m_vehicleSize));
                else gameLayer->m_pPlayer1->setScaleY(-gameLayer->m_pPlayer1->m_vehicleSize);
            }
        }
        if (KEY_Right_IS_PRESSED) {
            if (!gameLayer->m_pPlayer2->m_isDart && !gameLayer->m_pPlayer2->m_isShip) gameLayer->m_pPlayer2->setScaleX(fabs(gameLayer->m_pPlayer2->m_vehicleSize));
            //ship flipping
            if (gameLayer->m_pPlayer2->m_isShip && GameManager::sharedState()->getPlayerShip() != 170) {
                if (!gameLayer->m_pPlayer2->m_isUpsideDown) gameLayer->m_pPlayer2->setScaleY(fabs(gameLayer->m_pPlayer2->m_vehicleSize));
                else gameLayer->m_pPlayer2->setScaleY(-gameLayer->m_pPlayer2->m_vehicleSize);
            }
        }
        //jumpscare cube
        if (gameLayer->m_pPlayer1->m_isOnGround) wasPlayedJumpAnim = false;
        if (!wasPlayedJumpAnim) {
            if (gameLayer->m_pPlayer1->m_isHolding && gameLayer->m_pPlayer1->isCube()) {
                wasPlayedJumpAnim = true;
                CCSequence* jumpAnimSeq = CCSequence::create(
                    gameLayer->m_pPlayer1->getScaleX() < 0.f //test for
                    ? //scale < 0
                    CCEaseSineInOut::create(CCScaleTo::create(0.1,
                        laterally ? gameLayer->m_pPlayer1->getScaleX() : gameLayer->m_pPlayer1->getScaleX() + 0.3f,
                        laterally ? gameLayer->m_pPlayer1->getScaleY() - 0.25f : gameLayer->m_pPlayer1->getScaleY()
                    ))
                    : //else
                    CCEaseSineInOut::create(CCScaleTo::create(0.1,
                        laterally ? gameLayer->m_pPlayer1->getScaleX() : gameLayer->m_pPlayer1->getScaleX() - 0.3f,
                        laterally ? gameLayer->m_pPlayer1->getScaleY() - 0.3f : gameLayer->m_pPlayer1->getScaleY()
                    )),
                    CCEaseSineInOut::create(CCScaleTo::create(0.2, gameLayer->m_pPlayer1->getScaleX(), gameLayer->m_pPlayer1->getScaleY())),
                    nullptr
                );
                jumpAnimSeq->setTag(684093);
                if(!gameLayer->m_pPlayer1->getActionByTag(684093)) gameLayer->m_pPlayer1->runAction(jumpAnimSeq);
            }
        }
    }
    inline static std::string lastAnim1;
    inline static std::string lastAnim2;
    inline static void runAnim1(std::string name) {
        lastAnim1 = name;
        //RobotSprite
        gameLayer->m_pPlayer1->RobotSprite->runAnimation(lastAnim1);
        //SpiderSprite
        if (lastAnim1 == cfg.GetValue("gdPlatformerMode", "idleAnimationName")) gameLayer->m_pPlayer1->SpiderSprite->runAnimation(lastAnim1);
        if (lastAnim1 == "run") gameLayer->m_pPlayer1->playDynamicSpiderRun();
    }
    inline static void runAnim2(std::string name) {
        lastAnim2 = name;
        //RobotSprite
        gameLayer->m_pPlayer2->RobotSprite->runAnimation(lastAnim2);
        //SpiderSprite
        if (lastAnim2 == cfg.GetValue("gdPlatformerMode", "idleAnimationName")) gameLayer->m_pPlayer2->SpiderSprite->runAnimation(lastAnim2);
        if (lastAnim2 == "run") gameLayer->m_pPlayer2->playDynamicSpiderRun();
    }
    inline static void animationsUpdate() {
        if (!KEY_A_IS_PRESSED && !KEY_D_IS_PRESSED && gameLayer->m_pPlayer1->m_isOnGround) runAnim1(cfg.GetValue("gdPlatformerMode", "idleAnimationName"));
        else if (lastAnim1 != "run") runAnim1("run");
        if (!KEY_Left_IS_PRESSED && !KEY_Right_IS_PRESSED && gameLayer->m_pPlayer2->m_isOnGround) runAnim2(cfg.GetValue("gdPlatformerMode", "idleAnimationName"));
        else if (lastAnim2 != "run") runAnim2("run");
    }
    inline static void camXoffsetUpdate() {
        if (KEY_A_IS_PRESSED) {
            camXoffset = camXoffset - (smoothingThing1* 15);
            if (camXoffset < -180.0) camXoffset = -180.0;
        }
        else if (KEY_D_IS_PRESSED) {
            camXoffset = camXoffset + (smoothingThing1 * 15);
            if (camXoffset > 0.0) camXoffset = 0.0;
        }
    }
    inline static void ruin(GJBaseGameLayer* gameLayerArg) {
        gameLayer = gameLayerArg;
        enableMe = gameLayer->m_levelSettings->m_twoPlayerMode;
        if (gameLayer->m_pPlayer1->m_playerSpeed > 0.69) oldSpeed1 = gameLayer->m_pPlayer1->m_playerSpeed;
        if (gameLayer->m_pPlayer2->m_playerSpeed > 0.69) oldSpeed2 = gameLayer->m_pPlayer2->m_playerSpeed;


        if (weHaveGeode) cfg.LoadFile("geode/config/gdPlatformerMode.ini");
        else             cfg.LoadFile("config/gdPlatformerMode.ini");

        //patch crash on dashing backwards (by cos8o): 0xE9, 0xA7, 0x00 0x1EEB92 // org is 0x0f, 0x84, 0xa6
        if (enableMe) ModUtils::write_bytes(base + 0x1EEB92, { 0xE9, 0xA7, 0x00 });
        else ModUtils::write_bytes(base + 0x1EEB92, { 0x0f, 0x84, 0xa6 });

        //conditions
        if (!enableMe) return;

        if (cfg.GetBoolValue("gdPlatformerMode", "updateRotation")) updateRotation();
        if (cfg.GetBoolValue("gdPlatformerMode", "particlesUpdate")) particlesUpdate();
        if (cfg.GetBoolValue("gdPlatformerMode", "scaleUpdate")) scaleUpdate();
        if (cfg.GetBoolValue("gdPlatformerMode", "animationsUpdate")) animationsUpdate();
        if (cfg.GetBoolValue("gdPlatformerMode", "camXoffsetUpdate")) camXoffsetUpdate();

        //playlayer
        if (gameLayer->m_pPlayer1->m_isInPlayLayer) {
            PlayLayer* playLayer = reinterpret_cast<PlayLayer*>(gameLayerArg);
            if (!playLayer->m_isDead && !playLayer->m_hasCompletedLevel && cfg.GetBoolValue("gdPlatformerMode", "camXoffsetUpdate")) {
                playLayer->m_cameraXLocked = true;
                playLayer->m_cameraX = (playLayer->m_pPlayer1->m_position.x - 180.0) + camXoffset;
            }
        }

        if (gameLayer->m_pPlayer1->m_isDashing || gameLayer->m_pPlayer2->m_isDashing) {
            gameLayer->m_pPlayer1->m_playerSpeed = oldSpeed1;
            gameLayer->m_pPlayer2->m_playerSpeed = oldSpeed2;
            return;
        }

        updateSpeed();
    }
};
CCLabelTTF* dbg;
inline void(__thiscall* updateVisibility)(PlayLayer*);
void __fastcall updateVisibility_H(PlayLayer* self) {
    updateVisibility(self); //self->m_pPlayer1->runBallRotation
    PlatformerMod::ruin(self);//sometimes return;

    //if (self->getChildByTag(59320)) dbg->setString( (   "dispatchKeyboardIsAlive: " + std::to_string(dispatchKeyboardIsAlive)     ).c_str());
}

inline bool(__thiscall* PlayLayer_init)(PlayLayer*, GJGameLevel*);
bool __fastcall PlayLayer_init_H(PlayLayer* self, void*, GJGameLevel* level) {
    if (!PlayLayer_init(self, level)) return false;
    PlatformerMod::enableMe = level->m_twoPlayerMode;
    if(PlatformerMod::enableMe){
        self->toggleDualMode(GameObject::createWithKey(12), bool(!self->m_bIsDualMode), self->m_pPlayer1, true);
        self->toggleDualMode(GameObject::createWithKey(12), bool(!self->m_bIsDualMode), self->m_pPlayer1, true);
    }
    dbg = CCLabelTTF::create("", "Arial", 16.f);
    dbg->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
    dbg->setAnchorPoint({ 0.01f, 0.01f });
    self->addChild(dbg, 100, 59320);
    return true;
}

inline bool(__thiscall* LevelEditorLayer_init)(LevelEditorLayer*, GJGameLevel*);
bool __fastcall LevelEditorLayer_init_H(LevelEditorLayer* self, int, GJGameLevel* level) {
    if (!LevelEditorLayer_init(self, level)) return false;
    return true;
}

inline void(__thiscall* LevelEditorLayer_update)(LevelEditorLayer*, float);
void __fastcall LevelEditorLayer_update_H(LevelEditorLayer* self, void*, float dt) {
    LevelEditorLayer_update(self, dt);
    deltaTime = dt;
    PlatformerMod::ruin(self);
}

inline void(__thiscall* GameObject_activateObject)(GameObject*, PlayerObject*);
void __fastcall GameObject_activateObject_H(GameObject* self, void*, PlayerObject* player) {
    GameObject_activateObject(self, player);
    if (!(cfg.GetBoolValue("gdPlatformerMode", "Objects Always Not Activated"))) return;
    //nulty acteve
    self->m_bHasBeenActivated = false;
    self->m_bHasBeenActivatedP2 = false;
}

inline void(__thiscall* PlayerObject_ringJump)(PlayerObject*, GameObject*);
void __fastcall PlayerObject_ringJump_H(PlayerObject* self, void*, GameObject* ring) {
    PlayerObject_ringJump(self, ring);
    if (!(cfg.GetBoolValue("gdPlatformerMode", "Objects_Always_Not_Activated"))) return;
    ring->m_bHasBeenActivated = false;
    ring->m_bHasBeenActivatedP2 = false;
}

inline void(__thiscall* GJBaseGameLayer_bumpPlayer)(GJBaseGameLayer*, PlayerObject*, GameObject*);
void __fastcall GJBaseGameLayer_bumpPlayer_H(GJBaseGameLayer* self, void*, PlayerObject* player, GameObject* object) {
    GJBaseGameLayer_bumpPlayer(self, player, object);
    if (!(cfg.GetBoolValue("gdPlatformerMode", "Objects_Always_Not_Activated"))) return;
    object->m_bHasBeenActivated = false;
    object->m_bHasBeenActivatedP2 = false;
}

inline void(__thiscall* GameManager_update)(GameManager*, float);
void __fastcall GameManager_update_H(GameManager* self, void*, float dt) {
    GameManager_update(self, dt);
    deltaTime = dt;
    KEY_A_IS_PRESSED = (GetKeyState('A') & 0x8000);
    KEY_D_IS_PRESSED = (GetKeyState('D') & 0x8000);
    KEY_Left_IS_PRESSED = (GetKeyState(VK_LEFT) & 0x8000);
    KEY_Left_IS_PRESSED = (GetKeyState(VK_RIGHT) & 0x8000); 
    if (PlatformerMod::enableMe) {
        //negative rot
        if ((KEY_A_IS_PRESSED || KEY_Left_IS_PRESSED)) {
            //patch by sai(founded by user666) 1E9CD8: org b4 00 00 00 // patch 4c ff ff ff
            ModUtils::write_bytes(base + 0x1E9CD8, { 0x4c, 0xff, 0xff, 0xff });//- rot
            //user666's patch: game + 1E9DED - C1 E0 06 // org: game + 1E9DED - C1 E0 06
            ModUtils::write_bytes(base + 0x1E9DED, { 0xC1, 0xE0, 0x06 });//-ball rot
        }
        if ((KEY_D_IS_PRESSED || KEY_Right_IS_PRESSED)) {
            ModUtils::write_bytes(base + 0x1E9CD8, { 0xb4, 0x00, 0x00, 0x00 });//normal rot
            ModUtils::write_bytes(base + 0x1E9DED, { 0xC1, 0xE0, 0x03 });//ball rot
        }
    }
    else {
        ModUtils::write_bytes(base + 0x1E9CD8, { 0xb4, 0x00, 0x00, 0x00 });//normal rot
        ModUtils::write_bytes(base + 0x1E9DED, { 0xC1, 0xE0, 0x03 });//ball rot
    }
    if (!(cfg.GetBoolValue("gdPlatformerMode", "Dont_Force_vsync")))
        GameManager::sharedState()->setGameVariable("0030", true);
}

inline CCLabelBMFont* (__cdecl* CCLabelBMFont_create)(const char*, const char*);
CCLabelBMFont* CCLabelBMFont_create_H(const char* str, const char* fntFile) {
    if (std::string(str) == "2-Player" && std::string(fntFile) == "goldFont.fnt") str = "Platformer";
    if (std::string(str) == "Settings for gdPlatformerMode") ShellExecute(NULL, ("open"), (CCFileUtils::sharedFileUtils()->fullPathForFilename("geode/config/gdPlatformerMode.ini", 0).c_str()), NULL, NULL, 1);

    return CCLabelBMFont_create(str, fntFile);
}

inline void(__thiscall* CCLabelBMFont_setString)(CCLabelBMFont*, const char*);
void __fastcall CCLabelBMFont_setString_H(CCLabelBMFont* self, void*, const char* str) {
    CCLabelBMFont_setString(self, str);
}

DWORD WINAPI thread_func(void* hModule) {
    
    // initialize minhook
    MH_SafeInitialize();

    HOOK(base + 0x205460, updateVisibility);
    HOOK(base + 0x1FB780, PlayLayer_init);
    HOOK(base + 0x15ee00, LevelEditorLayer_init);
    HOOK(base + 0x16a660, LevelEditorLayer_update);
    HOOK(base + 0xEF0E0, GameObject_activateObject);
    HOOK(base + 0x1F4FF0, PlayerObject_ringJump);
    HOOK(base + 0x10ed50, GJBaseGameLayer_bumpPlayer);

    HOOK(base + 0xce440, GameManager_update);
    CC_HOOK("?create@CCLabelBMFont@cocos2d@@SAPAV12@PBD0@Z", CCLabelBMFont_create);
    CC_HOOK("?setString@CCLabelBMFont@cocos2d@@UAEXPBD@Z", CCLabelBMFont_setString);

    // enable all hooks you've created with minhook
    MH_EnableHook(MH_ALL_HOOKS);

    weHaveGeode = CCFileUtils::sharedFileUtils()->isFileExist("Geode.dll");

    if (!bool(CCFileUtils::sharedFileUtils()->isFileExist("geode/config/gdPlatformerMode.ini") || CCFileUtils::sharedFileUtils()->isFileExist("config/gdPlatformerMode.ini")))
        if (weHaveGeode) CCMessageBox("Config created at geode/config/gdPlatformerMode.ini", "gdPlatformerMode");
        else             CCMessageBox("Config created at config/gdPlatformerMode.ini", "gdPlatformerMode");

    if (weHaveGeode) {
        cfg.LoadFile("geode/config/gdPlatformerMode.ini");
    }
    else {
        filesystem::create_directories("config");
        cfg.LoadFile("config/gdPlatformerMode.ini");
    }
    
    if (!(cfg.KeyExists("gdPlatformerMode", "updateRotation")))
        cfg.SetBoolValue("gdPlatformerMode", "updateRotation", true, "; allow all features related to rotation");
    if (!(cfg.KeyExists("gdPlatformerMode", "particlesUpdate")))
        cfg.SetBoolValue("gdPlatformerMode", "particlesUpdate", true, "; allow all features related to particles");
    if (!(cfg.KeyExists("gdPlatformerMode", "scaleUpdate")))
        cfg.SetBoolValue("gdPlatformerMode", "scaleUpdate", true, "; allow all features related to scale");
    if (!(cfg.KeyExists("gdPlatformerMode", "animationsUpdate")))
        cfg.SetBoolValue("gdPlatformerMode", "animationsUpdate", true, "; allow all features related to animations of robot or spider");
    if (!(cfg.KeyExists("gdPlatformerMode", "idleAnimationName")))
        cfg.SetValue("gdPlatformerMode", "idleAnimationName", "idle01", "; set \"idle\" if u have not 2.2 animations");
    if (!(cfg.KeyExists("gdPlatformerMode", "camXoffsetUpdate")))
        cfg.SetBoolValue("gdPlatformerMode", "camXoffsetUpdate", true, "; allow camera moving x feature");
    if (!(cfg.KeyExists("gdPlatformerMode", "Objects_Always_Not_Activated")))
        cfg.SetBoolValue("gdPlatformerMode", "Objects_Always_Not_Activated", false, "; set HasBeenActivated = false for every object (or its multi activate feature).\n; WARN: cube portal maybe imposter");
    if (!(cfg.KeyExists("gdPlatformerMode", "Dont_Force_vsync")))
        cfg.SetBoolValue("gdPlatformerMode", "Dont_Force_vsync", false, "; set true if u dont want ");

    SI_Error err;
    if (weHaveGeode) err = cfg.SaveFile("geode/config/gdPlatformerMode.ini");
    else err = cfg.SaveFile("config/gdPlatformerMode.ini");

    return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, thread_func, hModule, 0, 0);
    return TRUE;
}
