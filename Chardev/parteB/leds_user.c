#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static int fileDescriptor;

#define SLEEP_TIME 130 // number in seconds
#define PATH "/dev/leds" // Path to write
 
int setLed(int filedesc, char *buf);
void contador();

int main() {

	if((fileDescriptor = open(PATH, O_RDWR)) < 0) {
		return 1;
	}
	int num = 0;

	while(num > -1 && num < 7){
		printf("Introduce un numero entre 1-7 (resto para salir)\n");
		scanf("%d", &num);

		if(num != 0){
			contador(num);}
		else{
			num = -1;}
	}	

	if((setLed(fileDescriptor, "")) != 0) {
		puts("Problemas con los leds");
	}

	if (close(fileDescriptor) < 0) {
		puts("No se pude cerrar el archivo");
		return 1;
	} 

	return 0;
}


int setLed(int filedesc, char *buf) {
	int len = strlen(buf);
    int bytes_writed;
	if((bytes_writed = write(filedesc, buf, len)) < 0) {
		puts("No se puede escribir en el fichero\n");
		return -1;
	}
	return 0;
}


void contador(int i) {
	char* buff;
		
	if (i==0) buff = "0";
	else if (i==1) buff = "3"; 
	else if (i==2) buff = "2";
	else if (i==3) buff = "23";
	else if (i==4) buff = "1";
	else if (i==5) buff = "13";
	else if (i==6) buff = "12";
	else if (i==7) buff = "123";
		 
	if((setLed(fileDescriptor, buff)) != 0) {
		puts("Problemas con los leds");
	}
}