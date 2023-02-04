
#include "pd.h"


PlaydateAPI* pd;


LCDFont* loadFont(const char* fontpath){
	const char* err = NULL;

	LCDFont* result = pd->graphics->loadFont(fontpath, &err);
	if (err != NULL) pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err);
	return result;
}

LCDBitmap* loadImage(const char* path){
	const char* err = NULL;
	LCDBitmap* result = pd->graphics->loadBitmap(path, &err);
	if (err != NULL) pd->system->error("%s:%i Error loading image at path '%s': %s", __FILE__, __LINE__, path, err);
	return result;
}



static int updateHandler(void* userdata){
	updateTweens();
	update();
	return 0;
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
int eventHandler(PlaydateAPI* playDate, PDSystemEvent event, uint32_t arg)
{
	
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	pd = playDate;

	if ( event == kEventInit )
	{
		srand(pd->system->getSecondsSinceEpoch(NULL));
		start();
		pd->system->setUpdateCallback(updateHandler, pd);
	}
	
	return 0;
}

int getRandomInRange(int lower, int upper){
    return (rand() % (upper - lower + 1)) + lower;
}

int lerpInt(int a, int b, float progress){
	float midpoint = (float)a + progress * (float)(b - a);
	return (int)midpoint;
}