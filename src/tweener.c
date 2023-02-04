#include "tweener.h"


#define MAX_TWEEN_COUNT 10


static bool isInited = false;

struct Tween{
    void (*updateFunc)(void* context, float progress);
    void (*completeFunc)(void* context);
    void* context;
    int totalFrames;
    int currentFrame;
};

static Tween* tweens[MAX_TWEEN_COUNT];

static void init(void){
    for (int i = 0; i < MAX_TWEEN_COUNT; i++)
    {
        Tween* tween = malloc(sizeof(struct Tween));
        tween->updateFunc = NULL;
        tween->completeFunc = NULL;
        tween->context = NULL;
        tween->totalFrames = 0;
        tween->currentFrame = 0;
        tweens[i] = tween;
    }

    isInited = true;
}

void completeTween(Tween* tween){
    if (tween == NULL) return;
    tween->updateFunc(tween->context, 1);
    tween->totalFrames = 0;
    if (tween->completeFunc != NULL) tween->completeFunc(tween->context);
}
void stopTween(Tween* tween) {
    if (tween == NULL) return;
    tween->updateFunc = NULL;
    tween->completeFunc = NULL;
    tween->context = NULL;
    tween->totalFrames = 0;
    tween->currentFrame = 0;
}

static void callUpdate(Tween* tween){
    if (tween->currentFrame == tween->totalFrames){
        completeTween(tween);
        return;
    }
    float progress = tween->currentFrame / (float)tween->totalFrames;
    tween->updateFunc(tween->context, progress);
   
}

static Tween* getFirstEmpty(void){

    Tween* latest = NULL;

    for (int i = 0; i < MAX_TWEEN_COUNT; i++)
    {
        Tween* tween = tweens[i];
        if (tween->totalFrames == 0) return tween;

        if (latest == NULL){
            latest = tween;
            continue;
        }

        if(latest->currentFrame < tween->currentFrame){
            latest = tween;
        }
    }
    
    completeTween(latest);
    return latest;
    
}

Tween* tween(void* context, void (*updateFunc)(void* context, float progress), int frames, void (*completeFunc)(void* context)){
	
    if (!isInited) init();

    Tween* tween = getFirstEmpty();
    tween->context = context;
    tween->currentFrame = 0;
    tween->totalFrames = frames;
    tween->updateFunc = updateFunc;
    tween->completeFunc = completeFunc;
    return tween;
}

void updateTweens(void){

    if (!isInited) init();

    for (int i = 0; i < MAX_TWEEN_COUNT; i++)
    {
        Tween* tween = tweens[i];
        if (tween->totalFrames == 0) continue;
        tween->currentFrame++;
        callUpdate(tween);
    }
}

float easeOutQuad(float p){
   return -(p * (p - 2)); 
} 

float easeOutQuint(float p) {
    float f = (p - 1);
    return f * f * f * f * f + 1;
}

