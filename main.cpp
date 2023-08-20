#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
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
bool speedBased;

double oldSpeed;
double oldX;
double smoothingThing;

float deltaTime;
PlayerObject* p1;
PlayerObject* p2;

std::string lastAnim;

CCSprite* platformerBtn1;
CCSprite* platformerBtn2;
CCSprite* platformerBtn3;
CCMenuItem* leftBtn;
CCMenuItem* rightBtn;
CCLayer* platLay;

CCLayer* platformerControls();

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
//TOOOLSS
bool isLeftSide() {
    return bool(
        (AKeyPressed || leftBtn->isSelected())
        &&
        !(DKeyPressed || rightBtn->isSelected())
        );
}
bool isRightSide() {
    return bool(
        (DKeyPressed || rightBtn->isSelected())
        &&
        !(AKeyPressed || leftBtn->isSelected())
        );
}
bool isIdleControlls() {
    return bool(!isLeftSide() && !isRightSide());
}
bool OnGroundIdle() {
    return bool(isIdleControlls() && p1->m_isOnGround || p2->m_isOnGround);
}
void runAnim(std::string name) {
    lastAnim = name;

    GJRobotSprite* RobotSprite1 = p1->RobotSprite;
    GJSpiderSprite* SpiderSprite1 = p1->SpiderSprite;

    GJRobotSprite* RobotSprite2 = p2->RobotSprite;
    GJSpiderSprite* SpiderSprite2 = p2->SpiderSprite;

    RobotSprite1->runAnimation(name);
    RobotSprite2->runAnimation(name);

    if (name == "idle01" || name == "idle02") {
        SpiderSprite1->runAnimation(name);
        SpiderSprite2->runAnimation(name);
    }
    else {
        p1->playDynamicSpiderRun();
        p2->playDynamicSpiderRun();
    }
}
void particlesUpdate() {
    //deactivateParticle
    if (OnGroundIdle()) {
        p1->deactivateParticle();
        p2->deactivateParticle();
    }
    else if (isCube(p1) && !p1->m_isOnGround && isLeftSide()) {
        p1->deactivateParticle();
        p2->deactivateParticle();
    }
}
void rotationUpdate() {
    if (isIdleControlls()) {
        //stop rot
        if (!p1->m_isDart) { //p1
            p1->stopRotation(true);
        }
    }
    //negative rot
    if (isLeftSide()) {
        //patch by sai 1E9CD8: org b4 00 00 00 // patch 4c ff ff ff
        ModUtils::write_bytes(base + 0x1E9CD8, { 0x4c, 0xff, 0xff, 0xff });
        if (p1->m_isBall) {
            if (p1->m_isUpsideDown) p1->setRotation((p1->getRotation() + (deltaTime * 10)));
            else p1->setRotation((p1->getRotation() - (deltaTime * 10)));
        }
    }
    else ModUtils::write_bytes(base + 0x1E9CD8, { 0xb4, 0x00, 0x00, 0x00 });//unpatch negative rot
}
void scaleUpdate() {
    //isLeftSide moving now
    if (isLeftSide()) {
        //negative scaleX if plr is not dart
        if (!p1->m_isDart) p1->setScaleX(-p1->m_vehicleSize);
        //ship flipping 'fix'
        if (speedBased && p1->m_isShip) p1->setFlipY(true);
        if (speedBased && p1->m_isShip) p1->setFlipX(true);
    }
    //isRightSide moving now
    if (isRightSide()) {
        //restorte scaleX if plr is not dart
        if (!p1->m_isDart) p1->setScaleX(fabs(p1->m_vehicleSize));
        //ship flipping 'fix'
        if (speedBased && p1->m_isShip) p1->setFlipY(false);
        if (speedBased && p1->m_isShip) p1->setFlipX(false);
    }
    if (isIdleControlls()) {
        //ship flipping 'fix'
        if (speedBased && p1->m_isShip && LeftSideWasLastkey) p1->setScaleX(fabs(p1->m_vehicleSize));
        if (speedBased && p1->m_isShip && LeftSideWasLastkey) p1->setFlipY(false);
    }
}
void animationsUpdate() {
    if (OnGroundIdle()) runAnim("idle01");
    else if (!isIdleControlls() && lastAnim != "run") runAnim("run");
}

void moveLeftSide() {
    if (!p1->m_isDart && !speedBased) { //if not dart or not speedBased use postions method
        p1->m_playerSpeed = fabs(oldSpeed); //restore plr1 speed
        p1->m_position.x = oldX - (p1->m_playerSpeed * deltaTime * 0.9f * 397.5f); //move plr1 by position
        smoothingThing = 0.5; //restore smoothingThing
    }
    else p1->m_playerSpeed = (-oldSpeed); //if dart or speedBased use speed method
    LeftSideWasLastkey = true;
    RightSideWasLastkey = false;
}
void moveRightSide() {
    if (!p1->m_isDart && !speedBased && p1->m_position.x > -1) { //if not dart or not speedBased use postions method
        p1->m_playerSpeed = fabs(oldSpeed); //restore plr1 speed
        p1->m_position.x = oldX + (p1->m_playerSpeed * deltaTime * 0.9f * 397.5f); //move plr1 by position
        smoothingThing = 0.5; //restore smoothingThing
    }
    else p1->m_playerSpeed = fabs(oldSpeed); //if dart or speedBased use speed method
    LeftSideWasLastkey = false;
    RightSideWasLastkey = true;
}
void idle() {
    if (!p1->m_isDart && !speedBased) { //if not dart or not speedBased use postions method
        //smoothingThing
        if (fabs(smoothingThing) > 0.0) smoothingThing = fabs(smoothingThing) - deltaTime; //
        if (fabs(smoothingThing) < 0.01) smoothingThing = 0.0; //презерватив
        if (LeftSideWasLastkey) smoothingThing = fabs(smoothingThing);
        if (RightSideWasLastkey) smoothingThing = -fabs(smoothingThing);
        //motion block
        p1->m_playerSpeed = fabs(oldSpeed);//restore speed
        p1->m_position.x = oldX - smoothingThing;
    }
    else p1->m_playerSpeed = 0; //if dart or speedBased use speed method
    p2->m_playerSpeed = 0;
}

//PLATFORMER BASE
void ruin() {
    if (p1->m_playerSpeed > 0.1) oldSpeed = p1->m_playerSpeed;

    if (isIdleControlls()) idle();
    if (isLeftSide()) moveLeftSide();
    if (isRightSide()) moveRightSide();

    platformerControlsUpdate(isIdleControlls(), isLeftSide(), isRightSide());
    particlesUpdate();
    rotationUpdate();
    scaleUpdate();
    animationsUpdate();

    oldX = p1->m_position.x;
}

inline void(__thiscall* GameObject_activateObject)(gd::GameObject* self, gd::PlayerObject* player);
void __fastcall GameObject_activateObject_H(gd::GameObject* self, int, gd::PlayerObject* player) {
    GameObject_activateObject(self, player);
    if (enable_platformer) self->m_bHasBeenActivated = false;
    if (enable_platformer) self->m_bHasBeenActivatedP2 = false;
}
inline void(__thiscall* GJBaseGameLayer_bumpPlayer)(gd::GJBaseGameLayer* self, gd::PlayerObject* player, gd::GameObject* object);
void __fastcall GJBaseGameLayer_bumpPlayer_H(gd::GJBaseGameLayer* self, int e, PlayerObject* player, GameObject* object) {
    GJBaseGameLayer_bumpPlayer(self, player, object);
    if (enable_platformer) object->m_bHasBeenActivated = false;
    if (enable_platformer) object->m_bHasBeenActivatedP2 = false;
}

inline void(__thiscall* PlayLayer_update)(gd::PlayLayer* self, float deltaTime);
void __fastcall PlayLayer_update_H(PlayLayer* self, int edx, float dt) {
    PlayLayer_update(self, dt);
    if (enable_platformer && !self->m_isDead && !self->m_bIsPaused && !self->m_hasCompletedLevel) {
        p1 = self->m_pPlayer1;
        p2 = self->m_pPlayer2;
        deltaTime = dt;
        speedBased = false;
        ruin();
    }
}

inline bool(__thiscall* PlayLayer_init)(PlayLayer*, GJGameLevel*);
bool __fastcall PlayLayer_init_H(PlayLayer* self, void* _edx, GJGameLevel* level) {
    if (!PlayLayer_init(self, level)) return false;
    enable_platformer = level->m_twoPlayerMode;
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


inline void(__thiscall* updateProgressbar)(PlayLayer*);//HOOK(base + 0x208020, updateProgressbar, true);
void __fastcall updateProgressbar_H(PlayLayer* self) {
    updateProgressbar(self);
        updateProgressbar(self);
        self->removeChildByTag(777);
        self->addChild(CCLabelBMFont::create((std::string("                ") + std::to_string(deltaTime) + std::string("\n\n\n")).c_str(), "bigFont.fnt"), 500, 777);
    /*}
    inline void(__thiscall* PlayLayer_update)(gd::PlayLayer* self, float deltaTime);
    void __fastcall PlayLayer_update_H(PlayLayer* self, int edx, float dt) {
        PlayLayer_update(self, dt);*/
    if (enable_platformer && !self->m_isDead && !self->m_bIsPaused && !self->m_hasCompletedLevel) {
        p1 = self->m_pPlayer1;
        p2 = self->m_pPlayer2;
        speedBased = false;
        ruin();
    }
}

inline void(__thiscall* LevelEditorLayer_onStopPlaytest)(LevelEditorLayer*, CCObject*);
void __fastcall LevelEditorLayer_onStopPlaytest_H(LevelEditorLayer* self, void* a, CCObject* sender) {
    LevelEditorLayer_onStopPlaytest(self, sender);
    if (enable_platformer) {
        platLay->setVisible(false);
    }
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

inline CCLabelBMFont* (__cdecl* CCLabelBMFont_create)(const char*, const char*);
CCLabelBMFont* CCLabelBMFont_create_H(const char* str, const char* fntFile) {
    if (std::string(str) == "2-Player" && std::string(fntFile) == "goldFont.fnt") str = "Platformer";
    return CCLabelBMFont_create(str, fntFile);
}

inline void(__thiscall* GameManager_update)(GameManager* self, float);
void __fastcall GameManager_update_H(GameManager* self, void*, float dt) {
    GameManager_update(self, dt);
    deltaTime = dt;
}

inline void(__thiscall* dispatchKeyboardMSG)(cocos2d::CCKeypadDispatcher* self, int key, bool down);
void __fastcall dispatchKeyboardMSG_H(cocos2d::CCKeypadDispatcher* self, void*, int key, bool down) {
    dispatchKeyboardMSG(self, key, down);

    if (key == cocos2d::KEY_A) AKeyPressed = (key == cocos2d::KEY_A && !AKeyPressed);
    if (key == cocos2d::KEY_D) DKeyPressed = (key == cocos2d::KEY_D && !DKeyPressed);
}

DWORD WINAPI thread_func(void* hModule) {
    // initialize minhook
    MH_Initialize();

    std::random_device os_seed;
    const unsigned int seed = os_seed();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribute(250, 1000);
    int sleepMs = distribute(generator);
    Sleep(sleepMs);

    //GameObject
    HOOK(base + 0xEF0E0, GameObject_activateObject, false);
    //GJBaseGameLayer
    HOOK(base + 0x10ed50, GJBaseGameLayer_bumpPlayer, true);
    //PlayLayer
    HOOK(base + 0x2029C0, PlayLayer_update, true);
    HOOK(base + 0x1FB780, PlayLayer_init, true);
    HOOK(base + 0x20BF00, PlayLayer_resetLevel, true);
    //fk
    HOOK(base + 0x15ee00, LevelEditorLayer_init, true);
    HOOK(base + 0x208020, updateProgressbar, true);
    //HOOK(base + 0x16a660, LevelEditorLayer_update, true);
    HOOK(base + 0x876e0, LevelEditorLayer_onStopPlaytest, true);

    HOOK(base + 0x1DE8F0, MoreOptionsLayer_init, true);
    HOOK(base + 0x1907B0, MenuLayer_init, true);

    HOOK(base + 0xce440, GameManager_update, true);

    CC_HOOK("?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z", dispatchKeyboardMSG, false);
    CC_HOOK("?create@CCLabelBMFont@cocos2d@@SAPAV12@PBD0@Z", CCLabelBMFont_create, false);

    // enable all hooks you've created with minhook
    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, thread_func, hModule, 0, 0);
    return TRUE;
}