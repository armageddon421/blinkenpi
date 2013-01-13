



#define EFFECT_BUFSIZE 1000


#define EFFECT_TYPE_DELAY 		0
#define EFFECT_TYPE_STROBE 		10
#define EFFECT_TYPE_FADEIN 		20
#define EFFECT_TYPE_FADEOUT 		21
#define EFFECT_TYPE_EXPLOSION		100
#define EFFECT_TYPE_EXPLOSION_PLUS	101
#define EFFECT_TYPE_EXPLOSION_X		102

struct effect{
	int active;
	int type;
	int pos;
	int red;
	int green;
	int blue;
	int parameter;
	int starttime;
	int duration;
};




void effect_init();
void effect_reset();
int effect_create(int type, int pos, int red, int green, int blue, int parameter, int time, int duration);
int effect_draw(int time);






