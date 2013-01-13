

#define SPI_CHANNEL 0
#define LED_NUM 384

int spi_init();
void update();
void led_set(int id, unsigned char r, unsigned char g, unsigned char b);
