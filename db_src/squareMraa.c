#include <stdio.h>
#include <stdlib.h> 
#include <iostream>
#include <sys/signal.h>
#include "mraa.hpp"

#define GPIO_A 23
#define GPIO_B 24
#define GPIO_C 25
#define GPIO_D 26
#define GPIO_E 27
#define GPIO_F 28
#define GPIO_G 29
#define GPIO_H 30
#define GPIO_I 31
#define GPIO_J 32
#define GPIO_K 33
#define GPIO_L 34

#define LOW 0
#define HIGH 1

bool running = true;

// ------------------------------------------------
// SIGINT handler
// ------------------------------------------------
void sig_handler(int signo)
{
    if (signo == SIGINT)
        running = false;
}

// ------------------------------------------------
// Main
// ------------------------------------------------
using namespace std;
int main(int argc, char* argv[])
{
    mraa::Result ret;
    int touch;
	int tmp;

	// SIGINT handler
    signal(SIGINT, sig_handler);

	// GPIO start definition
    mraa::Gpio* gpio_oe = new mraa::Gpio(GPIO_K);
//    mraa::Gpio* gpio_digital = new mraa::Gpio(GPIO_I); // 1.8v
    mraa::Gpio* gpio_digital = new mraa::Gpio(GPIO_E); // 5v

	// GPIO start checking
    if (gpio_oe == NULL || gpio_digital == NULL){
        return mraa::ERROR_UNSPECIFIED;
    } 

	// Init OutputEnable GPIO
    if ((ret = gpio_oe->dir(mraa::DIR_OUT))!= mraa::SUCCESS){
        return ret;
    }
	
    gpio_oe->write(LOW);
	cout << "Press ENTER to enable level shifter" << endl;
	cin >> tmp;
	
	cout << "--- Powering up Level shifter" << endl;
    gpio_oe->write(HIGH);
	usleep(100000);
	
	// Init Digital GPIO
    if ((ret = gpio_digital->dir(mraa::DIR_OUT))!= mraa::SUCCESS){
        return ret;
    }
	
	//---------------------------------------------------------
	// Main START
	//---------------------------------------------------------
	cout << "Press ENTER for start toggeling" << endl;
	cin >> tmp;
	cout << "Entering main loop" << endl;
    while (running) {
        int interval=1000;
                gpio_digital->write(HIGH);
                usleep(interval);
                gpio_digital->write(LOW);
                usleep(interval);
    }
    delete gpio_oe;
    delete gpio_digital;
    return ret;
}

