#ifndef UART_H
#define UART_H

typedef enum
{
    PARITY_NONE,
    PARITY_EVEN,
    PARITY_ODD
} UART_Parity;

typedef struct
{
    int baud_rate;
    int data_bits;
    int stop_bits;
    UART_Parity parity;
} UART_Config;

void UART_Init(const UART_Config *config);

void UART_SendChar(char ch);
void UART_SendString(const char *str);

char UART_ReceiveChar(void);
int UART_ReceiveReady(void);

double UART_GetBitTime(void);
double UART_GetFrameTime(void);


#define COLOR_RESET   "\033[0m"
#define COLOR_ORANGE "\033[38;5;208m"
#define COLOR_YELLOW  "\033[1;33m"
#define LIGHT_BLUE  "\033[1;36m"
#define DARK_BLUE   "\033[34m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_RED     "\033[1;31m"

#endif