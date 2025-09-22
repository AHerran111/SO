
#ifndef format
#define format
#define BLACK(string) "\x1b[30m" string "\x1b[0m"
#define RED(string) "\x1b[31m" string "\x1b[0m"
#define GREEN(string) "\x1b[32m" string "\x1b[0m"
#define YELLOW(string) "\x1b[33m" string "\x1b[0m"
#define BLUE(string) "\x1b[34m" string "\x1b[0m"
#define MAGENTA(string) "\x1b[35m" string "\x1b[0m"
#define CYAN(string) "\x1b[36m" string "\x1b[0m"
#define WHITE(string) "\x1b[37m" string "\x1b[0m"

#define BG_BLACK(string) "\x1b[40m" string "\x1b[0m"
#define BG_RED(string) "\x1b[41m" string "\x1b[0m"
#define BG_GREEN(string) "\x1b[42m" string "\x1b[0m"
#define BG_YELLOW(string) "\x1b[43m" string "\x1b[0m"
#define BG_BLUE(string) "\x1b[44m" string "\x1b[0m"
#define BG_MAGENTA(string) "\x1b[45m" string "\x1b[0m"
#define BG_CYAN(string) "\x1b[46m" string "\x1b[0m"
#define BG_WHITE(string) "\x1b[1m" string "\x1b[0m"

#endif

