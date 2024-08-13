#pragma once
#include "pvzclass.h"
#include "SexyStruct.h"

namespace Lawn {
    class _Zombie :public  Sexy::_GameObject// Size=0x15c (Id=403)
    {
    public:
        ZombieType::ZombieType mZombieType;// Offset=0x24 Size=0x4
        ZombieState::ZombieState mZombiePhase;// Offset=0x28 Size=0x4
        float mPosX;// Offset=0x2c Size=0x4
        float mPosY;// Offset=0x30 Size=0x4
        float mVelX;// Offset=0x34 Size=0x4
        int mAnimCounter;// Offset=0x38 Size=0x4
        int mGroanCounter;// Offset=0x3c Size=0x4
        int mAnimTicksPerFrame;// Offset=0x40 Size=0x4
        int mAnimFrames;// Offset=0x44 Size=0x4
        int mFrame;// Offset=0x48 Size=0x4
        int mPrevFrame;// Offset=0x4c Size=0x4
        bool mVariant;// Offset=0x50 Size=0x1
        bool mIsEating;// Offset=0x51 Size=0x1
        unsigned char __align1[2];// Offset=0x52 Size=0x2
        int mJustGotShotCounter;// Offset=0x54 Size=0x4
        int mShieldJustGotShotCounter;// Offset=0x58 Size=0x4
        int mShieldRecoilCounter;// Offset=0x5c Size=0x4
        int mZombieAge;// Offset=0x60 Size=0x4
        enum ZombieHeight mZombieHeight;// Offset=0x64 Size=0x4
        int mPhaseCounter;// Offset=0x68 Size=0x4
        int mFromWave;// Offset=0x6c Size=0x4
        bool mDroppedLoot;// Offset=0x70 Size=0x1
        unsigned char __align2[3];// Offset=0x71 Size=0x3
        int mZombieFade;// Offset=0x74 Size=0x4
        bool mFlatTires;// Offset=0x78 Size=0x1
        unsigned char __align3[3];// Offset=0x79 Size=0x3
        int mUseLadderCol;// Offset=0x7c Size=0x4
        int mTargetCol;// Offset=0x80 Size=0x4
        float mAltitude;// Offset=0x84 Size=0x4
        bool mHitUmbrella;// Offset=0x88 Size=0x1
        unsigned char __align4[3];// Offset=0x89 Size=0x3
        int mZombieRect[4];// Offset=0x8c Size=0x10
        int mZombieAttackRect[4];// Offset=0x9c Size=0x10
        int mChilledCounter;// Offset=0xac Size=0x4
        int mButteredCounter;// Offset=0xb0 Size=0x4
        int mIceTrapCounter;// Offset=0xb4 Size=0x4
        bool mMindControlled;// Offset=0xb8 Size=0x1
        bool mBlowingAway;// Offset=0xb9 Size=0x1
        bool mHasHead;// Offset=0xba Size=0x1
        bool mHasArm;// Offset=0xbb Size=0x1
        bool mHasObject;// Offset=0xbc Size=0x1
        bool mInPool;// Offset=0xbd Size=0x1
        bool mOnHighGround;// Offset=0xbe Size=0x1
        bool mYuckyFace;// Offset=0xbf Size=0x1
        int mYuckyFaceCounter;// Offset=0xc0 Size=0x4
        int mBlinkCounter;// Offset=0xc4 Size=0x4
        enum HelmType mHelmType;// Offset=0xc8 Size=0x4
        int mBodyHealth;// Offset=0xcc Size=0x4
        int mBodyMaxHealth;// Offset=0xd0 Size=0x4
        int mHelmHealth;// Offset=0xd4 Size=0x4
        int mHelmMaxHealth;// Offset=0xd8 Size=0x4
        enum ShieldType mShieldType;// Offset=0xdc Size=0x4
        int mShieldHealth;// Offset=0xe0 Size=0x4
        int mShieldMaxHealth;// Offset=0xe4 Size=0x4
        int mFlyingHealth;// Offset=0xe8 Size=0x4
        int mFlyingMaxHealth;// Offset=0xec Size=0x4
        bool mDead;// Offset=0xf0 Size=0x1
        unsigned char __align5[3];// Offset=0xf1 Size=0x3
        enum ZombieID mRelatedZombieID;// Offset=0xf4 Size=0x4
        enum ZombieID mFollowerZombieID[4];// Offset=0xf8 Size=0x10
        bool mPlayingSong;// Offset=0x108 Size=0x1
        unsigned char __align6[3];// Offset=0x109 Size=0x3
        int mParticleOffsetX;// Offset=0x10c Size=0x4
        int mParticleOffsetY;// Offset=0x110 Size=0x4
        enum AttachmentID mAttachmentID;// Offset=0x114 Size=0x4
        int mSummonCounter;// Offset=0x118 Size=0x4
        enum ReanimationID mBodyReanimID;// Offset=0x11c Size=0x4
        float mScaleZombie;// Offset=0x120 Size=0x4
        float mVelZ;// Offset=0x124 Size=0x4
        float mOrginalAnimRate;// Offset=0x128 Size=0x4
        enum PlantID mTargetPlantID;// Offset=0x12c Size=0x4
        int mBossMode;// Offset=0x130 Size=0x4
        int mTargetRow;// Offset=0x134 Size=0x4
        int mBossBungeeCounter;// Offset=0x138 Size=0x4
        int mBossStompCounter;// Offset=0x13c Size=0x4
        int mBossHeadCounter;// Offset=0x140 Size=0x4
        enum ReanimationID mBossFireBallReanimID;// Offset=0x144 Size=0x4
        enum ReanimationID mSpecialHeadReanimID;// Offset=0x148 Size=0x4
        int mFireballRow;// Offset=0x14c Size=0x4
        bool mIsFireBall;// Offset=0x150 Size=0x1
        unsigned char __align7[3];// Offset=0x151 Size=0x3
        enum ReanimationID mMoweredReanimID;// Offset=0x154 Size=0x4
        int mLastPortalX;// Offset=0x158 Size=0x4

        void UpdateAnimSpeed();
        bool IsOnBoard();
    };


}