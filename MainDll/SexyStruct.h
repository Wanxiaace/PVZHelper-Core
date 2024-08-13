#pragma once
#include <iostream>
#include <list>
#include "Common.h"

namespace Sexy {
    class Image// Size=0x38 (Id=262)
    {
    public:
        unsigned char __align0[4];// Offset=0x0 Size=0x4
        bool mDrawn;// Offset=0x4 Size=0x1
        unsigned char __align1[3];// Offset=0x5 Size=0x3
        std::basic_string<char, std::char_traits<char>, std::allocator<char> > mFilePath;// Offset=0x8 Size=0x1c
        int mWidth;// Offset=0x24 Size=0x4
        int mHeight;// Offset=0x28 Size=0x4
        int mNumRows;// Offset=0x2c Size=0x4
        int mNumCols;// Offset=0x30 Size=0x4
        unsigned int mAnimInfo;// Offset=0x34 Size=0x4
    };

    class MemoryImage :Sexy::Image// Size=0x6c (Id=1210)
    {
    public:
        //unsigned char __align0[56];// Offset=0x0 Size=0x38
        unsigned long* mBits;// Offset=0x38 Size=0x4
        int mBitsChangedCount;// Offset=0x3c Size=0x4
        void* mD3DData;// Offset=0x40 Size=0x4
        unsigned long mD3DFlags;// Offset=0x44 Size=0x4
        unsigned long* mColorTable;// Offset=0x48 Size=0x4
        unsigned int* mColorIndices;// Offset=0x4c Size=0x4
        bool mForcedMode;// Offset=0x50 Size=0x1
        bool mHasTrans;// Offset=0x51 Size=0x1
        bool mHasAlpha;// Offset=0x52 Size=0x1
        bool mIsVolatile;// Offset=0x53 Size=0x1
        bool mPurgeBits;// Offset=0x54 Size=0x1
        bool mWantPal;// Offset=0x55 Size=0x1
        unsigned char __align1[2];// Offset=0x56 Size=0x2
        unsigned long* mNativeAlphaData;// Offset=0x58 Size=0x4
        unsigned int* mRLAlphaData;// Offset=0x5c Size=0x4
        unsigned int* mRLAdditiveData;// Offset=0x60 Size=0x4
        bool mBitsChanged;// Offset=0x64 Size=0x1
        unsigned char __align2[3];// Offset=0x65 Size=0x3
        unsigned int mApp;// Offset=0x68 Size=0x4

        unsigned long* GetBits();
    };

    class GraphicsState// Size=0x4c (Id=243)
    {
    public:
        //static Sexy::Image mStaticImage;// Offset=0x0 Size=0x38
        char align_0[4];
        Sexy::Image* mDestImage;// Offset=0x0 Size=0x4
        float mTransX;// Offset=0x4 Size=0x4
        float mTransY;// Offset=0x8 Size=0x4
        float mScaleX;// Offset=0xc Size=0x4
        float mScaleY;// Offset=0x10 Size=0x4
        float mScaleOrigX;// Offset=0x14 Size=0x4
        float mScaleOrigY;// Offset=0x18 Size=0x4
        int mClipRect[4];// Offset=0x1c Size=0x10
        int mColor[4];// Offset=0x2c Size=0x10
        void* mFont;// Offset=0x3c Size=0x4
        int mDrawMode;// Offset=0x40 Size=0x4
        bool mColorizeImages;// Offset=0x44 Size=0x1
        bool mFastStretch;// Offset=0x45 Size=0x1
        bool mWriteColoredString;// Offset=0x46 Size=0x1
        bool mLinearBlend;// Offset=0x47 Size=0x1
        bool mIs3D;// Offset=0x48 Size=0x1
    };

    class Graphics : Sexy::GraphicsState// Size=0x68 (Id=240)
    {
    public:
        enum DRAWMODE
        {
            DRAWMODE_NORMAL = 0,
            DRAWMODE_ADDITIVE = 1
        };
        unsigned char __align0[80];// Offset=0x0 Size=0x50
        void* mPFActiveEdgeList;// Offset=0x50 Size=0x4
        int mPFNumActiveEdges;// Offset=0x54 Size=0x4
        void* mPFPoints;// Offset=0x0 Size=0x4
        unsigned char __align1[84];// Offset=0x4 Size=0x54
        int mPFNumVertices;// Offset=0x58 Size=0x4
        std::list<Sexy::GraphicsState, std::allocator<Sexy::GraphicsState> > mStateStack;// Offset=0x5c Size=0xc
    };

    class _GameObject 
    {
    public:
        class LawnApp* mApp;// Offset=0x0 Size=0x4
        class Board* mBoard;// Offset=0x4 Size=0x4
        int mX;// Offset=0x8 Size=0x4
        int mY;// Offset=0xc Size=0x4
        int mWidth;// Offset=0x10 Size=0x4
        int mHeight;// Offset=0x14 Size=0x4
        bool mVisible;// Offset=0x18 Size=0x1
        unsigned char __align0[3];// Offset=0x19 Size=0x3
        int mRow;// Offset=0x1c Size=0x4
        int mRenderOrder;// Offset=0x20 Size=0x4
    };

    class _GameButton// Size=0x128 (Id=964)
    {
    public:
        enum 
        {
            BUTTON_LABEL_LEFT = -1,
            BUTTON_LABEL_CENTER = 0,
            BUTTON_LABEL_RIGHT = 1
        };
        enum 
        {
            COLOR_LABEL = 0,
            COLOR_LABEL_HILITE = 1,
            COLOR_DARK_OUTLINE = 2,
            COLOR_LIGHT_OUTLINE = 3,
            COLOR_MEDIUM_OUTLINE = 4,
            COLOR_BKG = 5,
            NUM_COLORS = 6
        };

        class LawnApp* mApp;// Offset=0x0 Size=0x4
        class Widget* mParentWidget;// Offset=0x4 Size=0x4
        int mX;// Offset=0x8 Size=0x4
        int mY;// Offset=0xc Size=0x4
        int mWidth;// Offset=0x10 Size=0x4
        int mHeight;// Offset=0x14 Size=0x4
        bool mIsOver;// Offset=0x18 Size=0x1
        bool mIsDown;// Offset=0x19 Size=0x1
        bool mDisabled;// Offset=0x1a Size=0x1
        unsigned char __align0[1];// Offset=0x1b Size=0x1
        SexyColor mColors[NUM_COLORS];// Offset=0x1c Size=0x60
        int mId;// Offset=0x7c Size=0x4
        std::basic_string<char, std::char_traits<char>, std::allocator<char> > mLabel;// Offset=0x80 Size=0x1c
        int mLabelJustify;// Offset=0x9c Size=0x4
        class Font* mFont;// Offset=0xa0 Size=0x4
        Image* mButtonImage;// Offset=0xa4 Size=0x4
        Image* mOverImage;// Offset=0xa8 Size=0x4
        Image* mDownImage;// Offset=0xac Size=0x4
        Image* mDisabledImage;// Offset=0xb0 Size=0x4
        Image* mOverOverlayImage;// Offset=0xb4 Size=0x4
        int mNormalRect[4];// Offset=0xb8 Size=0x10
        int mOverRect[4];// Offset=0xc8 Size=0x10
        int mDownRect[4];// Offset=0xd8 Size=0x10
        int mDisabledRect[4];// Offset=0xe8 Size=0x10
        bool mInverted;// Offset=0xf8 Size=0x1
        bool mBtnNoDraw;// Offset=0xf9 Size=0x1
        bool mFrameNoDraw;// Offset=0xfa Size=0x1
        unsigned char __align1[5];// Offset=0xfb Size=0x5
        double mOverAlpha;// Offset=0x100 Size=0x8
        double mOverAlphaSpeed;// Offset=0x108 Size=0x8
        double mOverAlphaFadeInSpeed;// Offset=0x110 Size=0x8
        bool mDrawStoneButton;// Offset=0x118 Size=0x1
        int mTextOffsetX;// Offset=0x11c Size=0x4
        int mTextOffsetY;// Offset=0x120 Size=0x4
    };

}