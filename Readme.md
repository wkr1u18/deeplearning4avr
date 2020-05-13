# DeepLearning4Avr

![alt text](https://github.com/wkr1u18/deeplearning4avr/raw/master/demo.jpg "Demo of the DeepLearning4Avr")

## Overview

DL4AVR is a stub for a library allowing to run pre-trained deep learning models on low-capability AVR microcontrollers. In case of this project it is AT90USB1286 on LaFortuna board. So far, the only supported model is 3-layer MLP (which can be sigmoid/relu activated). As a demo, I implemented a classic "hello Wwrld" Computer Vision classifier, which recognizes handwritten characters. Because of the limited memory capabilities- input images must be in size 9x9.

For this project I pretrained a 2-layer MLP using Keras and tensorFlow using MNIST dataset (which was downsized to 9x9 using OpenCv); [Python part of this project can be found in deeplearning4avr-python repository](https://github.com/wkr1u18/deeplearning4avr-python) 

## How does it work?

Weights between layers (including bias) vectors from trained model are saved into txt file (where the floats are rounded up to 5 significant places - standard float takes 32-bits of memory on AVR and store up to 5 significant numbers). Then contents of this file must be copied to `layers.h` file (in a future python script could be extended to autogenerate C source).

The weights are stored as arrays in Flash memory and are accessed by using `avr/pgmspace.h`.

The core of the library is `linalg.c` which provides basic linear algebra functions. My matrix multiplication code takes one of the matrices from RAM, and the another one from Flash and produces a result in RAM memory. Also, this file contains code for adding bias unit, which is then multiplied with bias vector bit of next weight matrix.

The `main.c` file contains `predict()` function, which takes a pointer to 81 element array (consisting of 9x9 pixels) and returns an int with a predicted number. The core operation of the prediction is very simple, as it is just basic forward propagation code written using functions from `linalg.c`.

```c 
    Matrix* input = add_bias_unit(create_from_pointer(1,81,inputData));
    Matrix* afterFirstLayer = apply_function(multiply(input,first_layer_weights),relu);
    Matrix* beforeSecondLayer = add_bias_unit(afterFirstLayer);
    Matrix* afterSecondLayer = multiply(beforeSecondLayer, second_layer_weights);
    retVal = getMax(afterSecondLayer->numbers,10);
```

## Demo application
The demo aplication loads files from sdcard named 1.txt to 10.txt (they can be found in sdcard directory). Moving the rotary encoder switches between files, and clicking the central button runs the prediction.

Demo can be seen here:
[![video](https://img.youtube.com/vi/vCJctw01-vo/0.jpg)](https://www.youtube.com/watch?v=vCJctw01-vo)

