#include "pd_api.h"
#include "tweener.h"
#include "stdbool.h"

extern PlaydateAPI* pd;
extern int buttonsCurrent;
extern int buttonsPushed;
extern int buttonsReleased;


extern void update(void);
extern void start(void);
LCDFont* loadFont(const char* fontpath);
LCDBitmap* loadImage(const char* path);
int getRandomInRange(int start, int end);
int lerpInt(int a, int b, float progress);
void updateButtons(void);
