#ifndef CONFIG
#define CONFIG

#include <stdbool.h>
#include <stdio.h>

// Settings;

const char scls[] = "clear";    // command to clear screan
const int sshortstep = 5;       // value of short step
const int slongstep = 10;       // value of long step
inline bool soutputname = true; // default
const int sdefaultsound = 18;   // default sound
inline bool isloop = false;
const bool srewindaftswitch = true;
/* KEY binds;!

char / int / long...;

// equilalent

char KeyBind 71; // ASCII
int KeyBind = 71; // ASCII
char KeyBind = 'G';
*/

const char kshortback = ',';  // 44
const char kshortforwd = '.'; // 46
const char klongback = '<';   // 60
const char klongforwd = '>';  // 62
const char kquit = 'q';       // 113
const char ktobeg = '0';      // 48
const char koutname = 'N';    // 78
const char ksound_p = '=';    // 43
const char ksound_m = '-';    // 45
const char kmaxvol = 'm';     // 109
const char kmute = 'n';       // 110
const char kpause = ' ';      // 32
const char kloop = 'r';       // 114

#endif
