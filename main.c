#include "os.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include "linAlg.h"
#include "layers.h"


#define LINE_BUFF_LEN 5
FIL File;    

int check_switches(int);
int collect_delta(int state);

Matrix* first_layer_weights;
Matrix* second_layer_weights;

char files [10][6] = {"1.txt","2.txt","3.txt","4.txt","5.txt","6.txt","7.txt","8.txt","9.txt","10.txt"};
float newImage[81];
int index = 0;

void init_ml() {
    first_layer_weights = create_from_pointer(82,20,first_layer);
    second_layer_weights = create_from_pointer(21,10, second_layer);
}
int position = 0;


int getMax(float * array, uint8_t len) {
    float res = array[0];
    int i=0;
    int retVal = 0;
    for(i = 0; i< len; i++) {
        if(array[i]>res) {
            res = array [i];
            retVal = i;
        }
    }
    return retVal;
}




int display_free_ram(int state) {

	extern int __heap_start, *__brkval; 
	int v;
	
	int free = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

	printf("Free memory: %d\n", free);
	return state;
}

void print(Matrix *m){
	unsigned int i, j;
	if(m == NULL)
		return;
	for(j = 0; j < m->rows; j++){
		for(i = 0; i < m->columns; i++){
			printf("%d ", (int) m->numbers[j*m->columns + i]);
		}
		printf("\n");
	}
}

int predict(float * inputData) {
    int retVal;
    Matrix* input = add_bias_unit(create_from_pointer(1,81,inputData));
    Matrix* afterFirstLayer = apply_function(multiply(input,first_layer_weights),relu);
    Matrix* beforeSecondLayer = add_bias_unit(afterFirstLayer);
    Matrix* afterSecondLayer = multiply(beforeSecondLayer, second_layer_weights);
    retVal = getMax(afterSecondLayer->numbers,10);
    destructor(afterSecondLayer);
    destructor(afterFirstLayer);
    destructor(beforeSecondLayer);
    destructor(input);
    return retVal;
}

uint16_t float_to_colour(float input) {
    return (((int)(input*63))<<11)+(((int)(input*127))<<5)+((int)(input*63));
}

void display_image(float * input) {
    rectangle rect;
    uint8_t counter = 0;
    for(int j = 40; j<210; j+=20) {
        for(int i = 70; i<250; i+=20) {
             rect.left=i;
             rect.right=i+20;
             rect.top=j;
             rect.bottom=j+20;
             fill_rectangle(rect, float_to_colour(input[counter]));
             counter++;
        }
    } 
}

void main(void) {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    
    init_ml();
    os_init();
    printf("Welcome to demo application for DeepLearning4AVR\n");
    printf("Use rotary encoder to switch between images and central button to run prediction\n");
    read_image(files[index], newImage);
    display_image(newImage);
    f_mount(&FatFs, "", 0);
    os_add_task( collect_delta,   500, 1);
    os_add_task( check_switches,  100, 1);
    sei();
    for(;;) {

    }


}

int collect_delta(int state) {
    int result = os_enc_delta();
    if(result!=0) {
        if(result>0) {
            index=(index+1)%10;
        }
    
        if(result<0)
        {
            index = (10+index-1)%10;
        }
        clear_screen();
        read_image(files[index], newImage);
        display_image(newImage);
    }
   
	position += result;
	return state;
}

int check_switches(int state) {


	if (get_switch_long(_BV(SWC))) {
        int res = predict(newImage);
        printf("result is: %d\n",res);
	}



	return state;
}

void read_image(char * name, float * destination) {
    char line[LINE_BUFF_LEN];
    int8_t i,j;
    int counter = 0;

    f_mount(&FatFs, "", 0);
    if (f_open(&File, name, FA_READ) == FR_OK) {
        i = 0;
        while (f_gets(line, LINE_BUFF_LEN, &File)) {
            for( int i = 0; i< LINE_BUFF_LEN; i++) {
                if(line[i]==',') {
                    line[i]='\0';
                    destination[counter] = ((float) atoi(line) / (float) 255.0);
                    counter++;
                    break;
                }
            }
            
         

        }

    } else {
        display_string("Can't read file! \n");
    }
}
