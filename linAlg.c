#include "linAlg.h"

Matrix *constructor(int r, int c){
	unsigned int i;
	Matrix *m;
	if(r <= 0 || c <= 0){
		return NULL;
	}
	m = malloc(sizeof(Matrix));
	m->rows = r;
	m->columns = c;
	m->numbers = malloc(sizeof(float)*c*r);
	for(i = 0; i < c; i++)
	{
		for(int j = 0; j < r; j++) {
			*((m->numbers) + i*c + j)=0;
		}
	}
	return m;
}

void destructor(Matrix *m){
	if(m == NULL)
		return;
	free(m->numbers);
	free(m);
}

Matrix* create_from_pointer(int r, int c, float * pointer) {
    Matrix *m;
	if(r <= 0 || c <= 0){
		return NULL;
	}

	m = malloc(sizeof(Matrix));
	m->rows = r;
	m->columns = c;
	m->numbers = pointer;
	return m;
}

Matrix* add_bias_unit(Matrix* m) {
    Matrix* retVal = constructor(m->rows, m->columns+1);
    for(int i = 0; i < m->columns+1; i++) {
        for(int j = 0; j < m->rows; j++) {
            if(i==0) {
                retVal->numbers[j*retVal->columns + i] = 1;
            }
            else {
                retVal->numbers[j*retVal->columns + i] = m->numbers[j*m->columns + i-1];
            }
        }
    }
	
    return retVal;
}

Matrix * apply_function(Matrix *m, float fun(float)) {
    for(int i = 0; i <m->columns; i++) {
        for(int j = 0; j<m->rows; j++) {
            m->numbers[j*m->columns + i]=fun(m->numbers[j*m->columns + i]);
        }
    }
    return m;
}

float relu(float x) {
    if(x<=0) {
        return 0;
    } else {
        return x;
    }
}

float* getAddress(Matrix* m, int i, int j) { 
    return &(m->numbers[j*m->columns + i]);
}

Matrix *multiply(Matrix *m1, Matrix *m2){
	Matrix *product, *trans;
	unsigned int i, j;
	if(m1 == NULL || m2 == NULL)
		return NULL;
	if(m1->columns != m2->rows)
		return NULL;
    float accumulator;
	product = constructor(m1->rows, m2->columns);
    for(j = 0; j < product->rows; j++){
        for(i = 0; i < product->columns; i++){
            accumulator = 0.0;
            for(int z = 0; z< m1->columns; z++) {
                accumulator+= ((float) (*getAddress(m1,z,j))) * ((float) pgm_read_float(getAddress(m2,i,z)));
            }
			
			*(getAddress(product,i,j)) = accumulator;
		}
	}

	return product;
}