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

using namespace cocos2d;
using namespace gd;
using namespace cocos2d::extension;

bool AKeyPressed;
bool DKeyPressed;
bool LeftSideWasLastkey;
bool RightSideWasLastkey;
bool enable_platformer;

double oldSpeed;
double oldX;
double smoothingThing;
double moveSmoothingThing;

float deltaTime;
PlayerObject* PlayerObj;

std::string lastAnim;

CCSprite* platformerBtn1;
CCSprite* platformerBtn2;
CCSprite* platformerBtn3;
CCMenuItem* leftBtn;
CCMenuItem* rightBtn;
CCLayer* platLay;

CCLabelTTF* fugg;

CCLayer* platformerControls();

//TOOOLSS
bool isLeftSide() {
    return bool(
        (AKeyPressed || leftBtn->isSelected())
        &&
        !(DKeyPressed || rightBtn->isSelected())
        ) && !PlayerObj->m_isDashing;
}
bool isRightSide() {
    return bool(
        (DKeyPressed || rightBtn->isSelected())
        &&
        !(AKeyPressed || leftBtn->isSelected())
        ) || PlayerObj->m_isDashing;
}
bool isIdleControlls() {
    return bool(!isLeftSide() && !isRightSide()) && !PlayerObj->m_isDashing;
}
void runAnim(std::string name) {
    lastAnim = name;
    PlayerObj->RobotSprite->runAnimation(name);
    if (name == "idle01" || name == "idle02") PlayerObj->SpiderSprite->runAnimation(name);
    else if(name == "run") PlayerObj->playDynamicSpiderRun();
}
static bool isCube(gd::PlayerObject* player) {
    if (!player->m_isShip && !player->m_isBall && !player->m_isBird && !player->m_isDart && !player->m_isRobot && !player->m_isSpider)
        return true;
    return false;
}

//PLATFORMER CONTROLS ThiNG
CCLayer* platformerControls() {
    platLay = CCLayer::create();

    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("game001Sheet.plist");

    CCPoint platformerBtnPos = { 95.0f, 38.000f };

    //pbtn1
    platformerBtn1 = ModUtils::createSprite("Dpad_Btn.png");
    platformerBtn1->setPosition(platformerBtnPos);
    platLay->addChild(platformerBtn1, 5, 666);
    //pbtn2
    platformerBtn2 = ModUtils::createSprite("Dpad_Btn_Dwn.png");
    platformerBtn2->setPosition(platformerBtnPos);
    platformerBtn2->setVisible(false);
    platLay->addChild(platformerBtn2, 5, 666);
    //pbtn3
    platformerBtn3 = ModUtils::createSprite("Dpad_Btn_Dwn.png");
    platformerBtn3->setPosition(platformerBtnPos);
    platformerBtn3->setFlipX(true);
    platformerBtn3->setVisible(false);
    platLay->addChild(platformerBtn3, 5, 666);
    if (GameManager::sharedState()->getGameVariable("3913")) {
        platformerBtn1->setVisible(false);
        platformerBtn2->setVisible(false);
        platformerBtn3->setVisible(false);
    }

    CCMenu* platMenu = CCMenu::create();
    platMenu->setPosition({ 0.f,0.f });
    platLay->addChild(platMenu);

    leftBtn = CCMenuItem::create();
    leftBtn->setContentSize({ 190.000f,220.000f });
    platMenu->addChild(leftBtn);

    rightBtn = CCMenuItem::create();
    rightBtn->setContentSize({ 140.000f,220.000f });
    rightBtn->setPositionX(165.000f);
    platMenu->addChild(rightBtn);

    return platLay;
}
//UPDATE PLATFORMER CONTROLS ThiNG
void platformerControlsUpdate(bool isIdleControlls, bool leftside, bool rightside) {
    if (!GameManager::sharedState()->getGameVariable("3913")) {
        if (leftside && LeftSideWasLastkey && !RightSideWasLastkey) platformerBtn2->setVisible(true);
        else platformerBtn2->setVisible(false);
        if (rightside && RightSideWasLastkey && !LeftSideWasLastkey) platformerBtn3->setVisible(true);
        else platformerBtn3->setVisible(false);
        if (isIdleControlls) platformerBtn1->setVisible(true);
        else platformerBtn1->setVisible(false);
    }
    else {
        platformerBtn1->setVisible(false);
        platformerBtn2->setVisible(false);
        platformerBtn3->setVisible(false);
    }
}

void moveLeftSide() {
    LeftSideWasLastkey = true;
    RightSideWasLastkey = false;
    smoothingThing = 0.6; //restore smoothingThing
    moveSmoothingThing = (fabs(moveSmoothingThing) + (deltaTime / 20));
    if (fabs(moveSmoothingThing) >= 0.6) PlayerObj->m_playerSpeed = (-oldSpeed); //speed method
    else PlayerObj->m_playerSpeed = -fabs(moveSmoothingThing);
}
void moveRightSide() {
    LeftSideWasLastkey = false;
    RightSideWasLastkey = true;
    smoothingThing = 0.6; //restore smoothingThing
    moveSmoothingThing = (fabs(moveSmoothingThing) + (deltaTime / 20));
    if (fabs(moveSmoothingThing) >= 0.6) PlayerObj->m_playerSpeed = fabs(oldSpeed); //speed method
    else PlayerObj->m_playerSpeed = fabs(moveSmoothingThing);
}
void idle() {
    moveSmoothingThing = 0.0; //restore moveSmoothingThing
    //smoothingThing
    if (fabs(smoothingThing) > 0.0) smoothingThing = (fabs(smoothingThing) - (deltaTime/25)); //
    if (fabs(smoothingThing) < 0.1) smoothingThing = 0.0; //презерватив
    if (LeftSideWasLastkey) smoothingThing = -fabs(smoothingThing);
    if (RightSideWasLastkey) smoothingThing = fabs(smoothingThing);
    PlayerObj->m_playerSpeed = smoothingThing; //speed method
}

void scaleUpdate() {
    //isLeftSide moving now
    if (isLeftSide()) {
        //negative scaleX if plr is not dart
        if (!PlayerObj->m_isDart && !PlayerObj->m_isShip) PlayerObj->setScaleX(-PlayerObj->m_vehicleSize);
        //ship flipping
        if (PlayerObj->m_isShip && GameManager::sharedState()->getPlayerShip() != 170) {
            if (!PlayerObj->m_isUpsideDown) PlayerObj->setScaleY(-PlayerObj->m_vehicleSize);
            else PlayerObj->setScaleY(fabs(PlayerObj->m_vehicleSize));
        }
    }
    //isRightSide moving now
    if (isRightSide()) {
        //restorte scaleX if plr is not dart
        if (!PlayerObj->m_isDart && !PlayerObj->m_isShip) PlayerObj->setScaleX(fabs(PlayerObj->m_vehicleSize));
        //ship flipping
        if (PlayerObj->m_isShip && GameManager::sharedState()->getPlayerShip() != 170) {
            if (!PlayerObj->m_isUpsideDown) PlayerObj->setScaleY(fabs(PlayerObj->m_vehicleSize));
            else PlayerObj->setScaleY(-PlayerObj->m_vehicleSize);
        }
    }
}
//Particle
void particlesUpdate() {
    //deactivateParticle
    if (isIdleControlls() && fabs(smoothingThing) <= 0.2) PlayerObj->deactivateParticle();
    PlayerObj->m_vehicleGroundParticles->setVisible(!bool(isIdleControlls() && fabs(smoothingThing) <= 0.2));
}

//PLATFORMER BASE
void ruin() {
    if (PlayerObj->m_playerSpeed > 0.69) oldSpeed = PlayerObj->m_playerSpeed;
    if (isIdleControlls()) idle();
    if (isLeftSide()) moveLeftSide();
    if (isRightSide()) moveRightSide();
    platformerControlsUpdate(isIdleControlls(), isLeftSide(), isRightSide());
    //Rotation
    if (isIdleControlls() && !PlayerObj->m_isDashing) PlayerObj->stopRotation(false);
    //Particle
    particlesUpdate();
    //negative rot
    if (isLeftSide() && !PlayerObj->m_isDashing) {
        //patch by sai 1E9CD8: org b4 00 00 00 // patch 4c ff ff ff
        ModUtils::write_bytes(base + 0x1E9CD8, { 0x4c, 0xff, 0xff, 0xff });
        if (PlayerObj->m_isBall) {
            PlayerObj->stopAllActions();
            if (PlayerObj->m_isUpsideDown) PlayerObj->setRotation((PlayerObj->getRotation() + (deltaTime * 10)));
            else PlayerObj->setRotation((PlayerObj->getRotation() - (deltaTime * 10)));
        }
    }
    else ModUtils::write_bytes(base + 0x1E9CD8, { 0xb4, 0x00, 0x00, 0x00 });
    //Scale
    scaleUpdate();
    //animationsUpdate
    if (isIdleControlls() && fabs(smoothingThing) <= 0.2 && !PlayerObj->m_isHolding) runAnim("idle01");
    else if (!isIdleControlls() && lastAnim != "run") runAnim("run");
    //patch crash on dashing backwards (by cos8o): 0xE9, 0xA7, 0x00 0x1EEB92 // org is 0x0f, 0x84, 0xa6
    if (isIdleControlls() || isLeftSide()) ModUtils::write_bytes(base + 0x1EEB92, { 0xE9, 0xA7, 0x00 });
    else ModUtils::write_bytes(base + 0x1EEB92, { 0x0f, 0x84, 0xa6 });
}

inline void(__thiscall* PlayerObject_update)(PlayerObject* self, float);
void __fastcall PlayerObject_update_H(PlayerObject* self, void*, float dtidk){
    PlayerObject_update(self, dtidk);
    if (enable_platformer) {
        PlayerObj = self;
        deltaTime = dtidk;
        ruin();
    }
}

inline bool(__thiscall* PlayLayer_init)(PlayLayer*, GJGameLevel*);
bool __fastcall PlayLayer_init_H(PlayLayer* self, void* _edx, GJGameLevel* level) {
    if (!PlayLayer_init(self, level)) return false;
    enable_platformer = level->m_twoPlayerMode;
    if(!enable_platformer) enable_platformer = bool(strstr(level->m_sLevelName.c_str(), "latformer"));
    if (enable_platformer) {
        self->addChild(platformerControls(), 15);
        self->toggleDualMode(GameObject::createWithKey(12), bool(!self->m_bIsDualMode), self->m_pPlayer1, false);
        self->toggleDualMode(GameObject::createWithKey(12), bool(!self->m_bIsDualMode), self->m_pPlayer1, false);
    }
    return true;
}

inline void(__thiscall* PlayLayer_resetLevel)(PlayLayer*);
void __fastcall PlayLayer_resetLevel_H(PlayLayer* self) {
    PlayLayer_resetLevel(self);
    oldX = 0;
    ModUtils::write_bytes(base + 0x1E9CD8, { 0xb4, 0x00, 0x00, 0x00 });//unpatch negative rot
    ModUtils::write_bytes(base + 0x1EEB92, { 0x0f, 0x84, 0xa6 });//restore dashing anim
}

inline bool(__thiscall* LevelEditorLayer_init)(LevelEditorLayer*, GJGameLevel*);
bool __fastcall LevelEditorLayer_init_H(LevelEditorLayer* self, int edx, GJGameLevel* level) {
    if (!LevelEditorLayer_init(self, level)) return false;
    enable_platformer = level->m_twoPlayerMode;
    if (enable_platformer) {
        self->addChild(platformerControls(), 110);
        platLay->setVisible(false);
    }
    return true;
}

inline void(__thiscall* LevelEditorLayer_update)(LevelEditorLayer*, float);
void __fastcall LevelEditorLayer_update_H(LevelEditorLayer* self, float dt) {
    LevelEditorLayer_update(self, dt);
    if (enable_platformer) {
        platLay->setVisible(self->m_ePlaybackMode != kPlaybackModeNot);
        platformerControlsUpdate(isIdleControlls(), isLeftSide(), isRightSide());
    }
}

inline void(__thiscall* LevelEditorLayer_onStopPlaytest)(LevelEditorLayer*, CCObject*);
void __fastcall LevelEditorLayer_onStopPlaytest_H(LevelEditorLayer* self, void* a, CCObject* sender) {
    LevelEditorLayer_onStopPlaytest(self, sender);
    if (enable_platformer) platLay->setVisible(false);
}

inline bool(__thiscall* MoreOptionsLayer_init)(MoreOptionsLayer*);
bool __fastcall MoreOptionsLayer_init_H(gd::MoreOptionsLayer* self) {
    if (!MoreOptionsLayer_init(self)) return false;
    self->addToggle("Hide platformer buttons", "3913", "Hide buttons <cy>texture</c> for platformer mode.\nButtons still will work!");
    return true;
}

inline bool(__thiscall* MenuLayer_init)(MenuLayer*);
bool __fastcall MenuLayer_init_H(MenuLayer* self) {
    enable_platformer = false;
    if (!MenuLayer_init(self)) return false;
    return true;
}

inline void(__thiscall* GameManager_update)(GameManager* self, float);
void __fastcall GameManager_update_H(GameManager* self, void*, float dt) {
    GameManager_update(self, dt);
    deltaTime = dt;
}

inline void(__thiscall* dispatchKeyboardMSG)(cocos2d::CCKeypadDispatcher* self, int key, bool down);
void __fastcall dispatchKeyboardMSG_H(cocos2d::CCKeypadDispatcher* self, void*, int key, bool down) {
    dispatchKeyboardMSG(self, key, down);
    if (key == cocos2d::KEY_A || key == cocos2d::KEY_Left) 
        AKeyPressed = ((key == cocos2d::KEY_A || key == cocos2d::KEY_Left) && !AKeyPressed);
    if (key == cocos2d::KEY_D || key == cocos2d::KEY_Right) 
        DKeyPressed = ((key == cocos2d::KEY_D || key == cocos2d::KEY_Right) && !DKeyPressed);
}

inline CCLabelBMFont* (__cdecl* CCLabelBMFont_create)(const char*, const char*);
CCLabelBMFont* CCLabelBMFont_create_H(const char* str, const char* fntFile) {
    if (std::string(str) == "2-Player") str = "Platformer";
    return CCLabelBMFont_create(str, fntFile);
}

DWORD WINAPI thread_func(void* hModule) {
    
    // initialize minhook
    MH_Initialize();

    HOOK(base + 0x1E8200, PlayerObject_update, false);//fk

    HOOK(base + 0x1FB780, PlayLayer_init, true);
    HOOK(base + 0x20BF00, PlayLayer_resetLevel, true);

    HOOK(base + 0x15ee00, LevelEditorLayer_init, true);
    HOOK(base + 0x16a660, LevelEditorLayer_update, true);
    HOOK(base + 0x876e0, LevelEditorLayer_onStopPlaytest, true);

    HOOK(base + 0x1DE8F0, MoreOptionsLayer_init, true);
    HOOK(base + 0x1907B0, MenuLayer_init, true);
    HOOK(base + 0xce440, GameManager_update, true);

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
