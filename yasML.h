#ifndef yasML_h
#define yasML_h

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/io.h>


typedef struct Matrix{
	int rows;
	int columns;
	float *numbers;
} Matrix;

Matrix *constructor(int r, int c);
void destructor(Matrix *m);
Matrix* create_from_pointer(int r, int c, float * pointer);
Matrix* add_bias_unit(Matrix* m);
void print(Matrix *m);
float relu(float x);
Matrix * apply_function(Matrix *m, float fun(float));
Matrix *transpose(Matrix *m);
float* getAddress(Matrix *m, int i, int j);
Matrix *multiply(Matrix *m1, Matrix *m2);

#endif // yasML

