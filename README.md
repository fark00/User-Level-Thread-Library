# User-Level-Thread-Library
Simple thread library using c
with this library you must include "green.h" in your program. then you can use this functions:
* int green_create(green_t * thread , void *(*fun)(void *),void * arg);
* int green_yield();
* int green_join(green_t * thread, void ** val);   
 
project has two phazes. in green_phaze1 there are no timer interrupt and context switch accur only by users with green_yield() function but in green_phaze2 timer intrrupt raise every 1000 usec  
This project was done as a team by me and [@Farzaneh Koohestani](https://github.com/fark00).  
# Requirement  
* gcc compiler
* linux operating system 
# Usage
* make 
* ./green_test to execute green_test.c code
# Support:

**Contact me @:**

e-mail: r.abtahi1378@gmail.com

telegram id: @Reyhan_abt

