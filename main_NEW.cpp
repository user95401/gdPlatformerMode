#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <random>
#include <cocos2d.h>
#include <gd.h>
#include "mod_utils.hpp"
#include "hooks.hpp"
#include <MinHook.h>
#include <math.h>
using namespace std;

float deltaTime;

bool KEY_A_IS_PRESSED;
bool KEY_D_IS_PRESSED;
bool KEY_Left_IS_PRESSED;
bool KEY_Right_IS_PRESSED;
bool KEY_F3_IS_PRESSED;
int lastKey1;
int lastKey2;

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
                auto rotAction1 = CCRotateTo::create(0.1, lastKey1 == KEY_A ? 180 : 0);
                rotAction1->setTag(777);
                if (!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->runAction(rotAction1);
            }
        }
        if (!KEY_Left_IS_PRESSED && !KEY_Right_IS_PRESSED && !gameLayer->m_pPlayer2->m_isDashing) {
            gameLayer->m_pPlayer2->stopRotation(false);
            if (gameLayer->m_pPlayer2->m_isShip) {
                auto rotAction2 = CCRotateTo::create(0.1, lastKey2 == KEY_Left ? 180 : 0);
                rotAction2->setTag(778);
                if (!gameLayer->m_pPlayer2->getActionByTag(778)) gameLayer->m_pPlayer2->runAction(rotAction2);
            }
        }
        //bird
        if (gameLayer->m_pPlayer1->m_isBird) 
            gameLayer->m_pPlayer1->setRotation(0);
        if (gameLayer->m_pPlayer2->m_isBird || (gameLayer->m_pPlayer2->isCube() && gameLayer->m_pPlayer2->m_isOnGround)) 
            gameLayer->m_pPlayer2->setRotation(0);
        //bot and spider in 3d 4k october
        if (gameLayer->m_pPlayer1->m_isRobot || gameLayer->m_pPlayer1->m_isSpider && gameLayer->m_pPlayer1->m_isOnGround) {
            auto rotAction = CCRotateTo::create(0.1, 0);
            rotAction->setTag(777);
            if (!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->runAction(rotAction);
        }
        if (gameLayer->m_pPlayer2->m_isRobot || gameLayer->m_pPlayer2->m_isSpider && gameLayer->m_pPlayer2->m_isOnGround) {
            auto rotAction = CCRotateTo::create(0.1, 0);
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
            auto rotAction = CCRotateTo::create(0.1, torot);
            rotAction->setTag(777);
            if(!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->runAction(rotAction);
        }
        if ((gameLayer->m_pPlayer2->isCube()) && gameLayer->m_pPlayer2->m_isOnGround) {
            float rot = gameLayer->m_pPlayer2->getRotation();
            float torot = 0;
            if (gameLayer->m_pPlayer2->getRotation() < 0) rot = fabs(rot);
            if (rot < 90 - 50) torot = 0;
            else if (rot < 180 - 50) torot = 90;
            else if (rot < 360 - 50) torot = 180;
            else if (rot < 420) torot = 360;
            else torot = 0;
            if (gameLayer->m_pPlayer2->getRotation() < 0) torot = -torot;
            auto rotAction = CCRotateTo::create(0.1, torot);
            rotAction->setTag(777);
            if (!gameLayer->m_pPlayer2->getActionByTag(777)) gameLayer->m_pPlayer2->runAction(rotAction);
        }
        //negative rot
        if ((KEY_A_IS_PRESSED || KEY_Left_IS_PRESSED)) {
            float ballrotstep = (5.f + gameLayer->m_pPlayer1->m_playerSpeed);
            if (gameLayer->m_pPlayer1->m_isBall) {
                auto rotAction1 = CCRotateBy::create(0.0, gameLayer->m_pPlayer1->m_isUpsideDown ? fabs(ballrotstep) : -ballrotstep);
                rotAction1->setTag(777);
                if (!gameLayer->m_pPlayer1->getActionByTag(777)) gameLayer->m_pPlayer1->runAction(rotAction1);
            }
            if (gameLayer->m_pPlayer2->m_isBall) {
                auto rotAction2 = CCRotateBy::create(0.0, gameLayer->m_pPlayer2->m_isUpsideDown ? fabs(ballrotstep) : -ballrotstep);
                rotAction2->setTag(778);
                if (!gameLayer->m_pPlayer2->getActionByTag(778)) gameLayer->m_pPlayer2->runAction(rotAction2);
            }
            //patch by sai(founded by user666) 1E9CD8: org b4 00 00 00 // patch 4c ff ff ff
            ModUtils::write_bytes(base + 0x1E9CD8, { 0x4c, 0xff, 0xff, 0xff });
        }
        else ModUtils::write_bytes(base + 0x1E9CD8, { 0xb4, 0x00, 0x00, 0x00 });

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
    }
    inline static std::string lastAnim1;
    inline static std::string lastAnim2;
    inline static void runAnim1(std::string name) {
        lastAnim1 = name;
        gameLayer->m_pPlayer1->RobotSprite->runAnimation(name);
        if (name == "idle01" || name == "idle02") gameLayer->m_pPlayer1->SpiderSprite->runAnimation(name);
        else if (name == "run") gameLayer->m_pPlayer1->playDynamicSpiderRun();
    }
    inline static void runAnim2(std::string name) {
        lastAnim2 = name;
        gameLayer->m_pPlayer2->RobotSprite->runAnimation(name);
        if (name == "idle01" || name == "idle02") gameLayer->m_pPlayer2->SpiderSprite->runAnimation(name);
        else if (name == "run") gameLayer->m_pPlayer2->playDynamicSpiderRun();
    }
    inline static void animationsUpdate() {
        if (!KEY_A_IS_PRESSED && !KEY_D_IS_PRESSED) runAnim1("idle01");
        else if (lastAnim1 != "run") runAnim1("run");
        if (!KEY_Left_IS_PRESSED && !KEY_Right_IS_PRESSED) runAnim2("idle01");
        else if (lastAnim2 != "run") runAnim2("run");
    }
    inline static void ruin(GJBaseGameLayer* gameLayerArg) {
        gameLayer = gameLayerArg;
        if (gameLayer->m_pPlayer1->m_playerSpeed > 0.69) oldSpeed1 = gameLayer->m_pPlayer1->m_playerSpeed;
        if (gameLayer->m_pPlayer2->m_playerSpeed > 0.69) oldSpeed2 = gameLayer->m_pPlayer2->m_playerSpeed;


        //patch crash on dashing backwards (by cos8o): 0xE9, 0xA7, 0x00 0x1EEB92 // org is 0x0f, 0x84, 0xa6
        if (enableMe) ModUtils::write_bytes(base + 0x1EEB92, { 0xE9, 0xA7, 0x00 });
        else ModUtils::write_bytes(base + 0x1EEB92, { 0x0f, 0x84, 0xa6 });
        //conditions
        if (!enableMe) return;
        if (gameLayer->m_pPlayer1->m_isDashing || gameLayer->m_pPlayer2->m_isDashing) {
            gameLayer->m_pPlayer1->m_playerSpeed = oldSpeed1;
            gameLayer->m_pPlayer2->m_playerSpeed = oldSpeed2;
            return;
        }
        
        updateSpeed();
        updateRotation();
        particlesUpdate();
        scaleUpdate();
        animationsUpdate();
    }
};

inline void(__thiscall* updateVisibility)(PlayLayer*);
void __fastcall updateVisibility_H(PlayLayer* self) {
    updateVisibility(self); //self->m_pPlayer1->runBallRotation
    PlatformerMod::enableMe = self->m_level->m_twoPlayerMode;
    PlatformerMod::ruin(self);

}
inline bool(__thiscall* PlayLayer_init)(PlayLayer*, GJGameLevel*);
bool __fastcall PlayLayer_init_H(PlayLayer* self, void*, GJGameLevel* level) {
    if (!PlayLayer_init(self, level)) return false;
    PlatformerMod::enableMe = level->m_twoPlayerMode;
    if(PlatformerMod::enableMe){
        self->toggleDualMode(GameObject::createWithKey(12), bool(!self->m_bIsDualMode), self->m_pPlayer1, true);
        self->toggleDualMode(GameObject::createWithKey(12), bool(!self->m_bIsDualMode), self->m_pPlayer1, true);
    }
    return true;
}

inline bool(__thiscall* LevelEditorLayer_init)(LevelEditorLayer*, GJGameLevel*);
bool __fastcall LevelEditorLayer_init_H(LevelEditorLayer* self, int, GJGameLevel* level) {
    if (!LevelEditorLayer_init(self, level)) return false;
    PlatformerMod::enableMe = level->m_twoPlayerMode;
    return true;
}
inline void(__thiscall* LevelEditorLayer_update)(LevelEditorLayer*, float);
void __fastcall LevelEditorLayer_update_H(LevelEditorLayer* self, void*, float dt) {
    LevelEditorLayer_update(self, dt);
    deltaTime = dt;
    PlatformerMod::ruin(self);
}

inline void(__thiscall* GameManager_update)(GameManager*, float);
void __fastcall GameManager_update_H(GameManager* self, void*, float dt) {
    GameManager_update(self, dt);
    deltaTime = dt;
}
inline void(__thiscall* dispatchKeyboardMSG)(cocos2d::CCKeypadDispatcher* self, int key, bool down);
void __fastcall dispatchKeyboardMSG_H(cocos2d::CCKeypadDispatcher* self, void*, int key, bool down) {
    dispatchKeyboardMSG(self, key, down);

    if (key == KEY_A) KEY_A_IS_PRESSED = (key == KEY_A && !KEY_A_IS_PRESSED);
    if (key == KEY_D) KEY_D_IS_PRESSED = (key == KEY_D && !KEY_D_IS_PRESSED);
    if (key == KEY_Left) KEY_Left_IS_PRESSED = (key == KEY_Left && !KEY_Left_IS_PRESSED);
    if (key == KEY_Right) KEY_Right_IS_PRESSED = (key == KEY_Right && !KEY_Right_IS_PRESSED);

    if (key == KEY_F3) KEY_F3_IS_PRESSED = (key == KEY_F3 && !KEY_F3_IS_PRESSED);
}
inline CCLabelBMFont* (__cdecl* CCLabelBMFont_create)(const char*, const char*);
CCLabelBMFont* CCLabelBMFont_create_H(const char* str, const char* fntFile) {
    if (std::string(str) == "2-Player" && std::string(fntFile) == "goldFont.fnt") str = "Platformer";
    return CCLabelBMFont_create(str, fntFile);
}

DWORD WINAPI thread_func(void* hModule) {
    
    // initialize minhook
    MH_Initialize();

    HOOK(base + 0x205460, updateVisibility, false);
    HOOK(base + 0x1FB780, PlayLayer_init, false);
    HOOK(base + 0x15ee00, LevelEditorLayer_init, true);
    HOOK(base + 0x16a660, LevelEditorLayer_update, true);

    HOOK(base + 0xce440, GameManager_update, false);
    CC_HOOK("?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z", dispatchKeyboardMSG, false);
    CC_HOOK("?create@CCLabelBMFont@cocos2d@@SAPAV12@PBD0@Z", CCLabelBMFont_create, false);

    // enable all hooks you've created with minhook
    MH_EnableHook(MH_ALL_HOOKS);

    //ModUtils::write_bytes(base + 0x41028c, { 0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20 });

    return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, thread_func, hModule, 0, 0);
    return TRUE;
}
