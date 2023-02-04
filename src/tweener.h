#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct Tween Tween;

void abortTween(Tween* tween);
void completeTween(Tween* tween);
Tween* tween(void* context, void (*updateFunc)(void* context, float progress), int frames, void (*completeFunc)(void* context));
void updateTweens(void);

float easeOutQuad(float p);
float easeOutQuint(float p);