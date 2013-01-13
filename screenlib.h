

#define WIDTH 32
#define HEIGHT LED_NUM/WIDTH
#define FONT_WIDTH 6

void blit();
void draw_char(double xo, char c);
double draw_string(double ts, char *text);
void setpixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void aapixel(double x, double y, unsigned char r, unsigned char g, unsigned char b);



