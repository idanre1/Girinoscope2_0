#include <stdio.h>
#include <stdlib.h>

#include "libsoc_gpio.h"
#include "libsoc_debug.h"
#include "libsoc_board.h"


/* This bit of code below makes this example work on all 96Boards */
unsigned int GPIO_OE;
unsigned int GPIO_DIGITAL;

int running = 1;

__attribute__((constructor)) static void _init() {
    board_config *config = libsoc_board_init();
    GPIO_OE     = libsoc_board_gpio_id(config, "GPIO-K");
//    GPIO_DIGITAL     = libsoc_board_gpio_id(config, "GPIO-I"); // 1.8v
    GPIO_DIGITAL     = libsoc_board_gpio_id(config, "GPIO-E"); // 5v
    libsoc_board_free(config);
}
/* End of 96Boards special code */

int tmp;

int main() {
	// Turn debug on
	libsoc_set_debug(0);

	// Init OutputEnable GPIO
	gpio *gpio_oe;
	gpio_oe = libsoc_gpio_request(GPIO_OE,LS_SHARED);
	if(gpio_oe == NULL) {
		goto fail;
	}
	libsoc_gpio_set_direction(gpio_oe,OUTPUT);
	if(libsoc_gpio_get_direction(gpio_oe) != OUTPUT) {
		goto fail;
	}
	libsoc_gpio_set_level(gpio_oe,0);
	printf("Press ENTER to enable level shifter\n");
	scanf("%d", &tmp);
	printf("--- Powering up Level shifter\n");
	libsoc_gpio_set_level(gpio_oe,1);
	usleep(100000);

	// Init OutputEnable GPIO
	gpio *gpio_digital;
	gpio_digital = libsoc_gpio_request(GPIO_DIGITAL,LS_SHARED);
	if(gpio_digital == NULL) {
			goto fail;
	}
	libsoc_gpio_set_direction(gpio_digital,OUTPUT);
	if(libsoc_gpio_get_direction(gpio_digital) != OUTPUT) {
			goto fail;
	}

	printf("--- Setting GPIO to LOW\n");
	libsoc_gpio_set_level(gpio_digital,LOW);

	printf("Press ENTER for start toggeling\n");
	scanf("%d", &tmp);
	//---------------------------------------------------------
	// Main START
	//---------------------------------------------------------
	printf("Entering main loop");
	while(running) {
	int interval=1000;
		libsoc_gpio_set_level(gpio_digital,HIGH);
		usleep(interval);
		libsoc_gpio_set_level(gpio_digital,LOW);
		usleep(interval);
	}
	//---------------------------------------------------------
	// Main END
	//---------------------------------------------------------
	

	fail:
		libsoc_gpio_free(gpio_oe);
		libsoc_gpio_free(gpio_digital);
	return EXIT_SUCCESS;
}

