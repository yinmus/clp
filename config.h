#ifndef CONFIG
#define CONFIG

#include <stdbool.h>
#include <stdio.h>

// Settings;

const char scls[] =           "clear";  // command to clear screan
const int sshortstep =        5;        // value of short step
const int slongstep =         10;       // value of long step
bool soutputname =            true;     // default
const int sdefaultsound =     18;       // default sound в cl.c расчет с float, но там погрешность в районе ~>1 значения 
bool isloop =                 false;    // кому надо может поставить цикл по дефолту
const bool srewindaftswitch = true;     // после переключения цикличного / обычного проигрывания перематывать на то место, когда было переключение
/* KEY binds;!

char / int / long - в теории все равно (если тебе все равно на размер бинарника (char - оптимально)!)

// equilalent

char KeyBind 71; // ASCII
int KeyBind = 71; // ASCII
char KeyBind = 'G';
*/


// кстати не все клавиши можно забиндить, мне лень, кому надо, думаю сам сделает
const char kshortback = ','; // 44
const char kshortforwd ='.'; // 46
const char klongback =  '<'; // 60
const char klongforwd = '>'; // 62
const char kquit =      'q'; // 113
const char ktobeg =     '0'; // 48

#endif
