#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "lcd.h"
#include "yasML.h"
#include "layers.h"

Matrix* first_layer_weights;
Matrix* second_layer_weights;

void init_ml() {
    first_layer_weights = create_from_pointer(82,20,first_layer);
    second_layer_weights = create_from_pointer(21,10, second_layer);
}


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

    init_lcd();
    init_ml();
    int res = predict(test_image);
    printf("result is: %d\n",res);
    display_image(test_image);


}