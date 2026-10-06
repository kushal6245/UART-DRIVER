#include <stdio.h>
#include "uart.h"

#define TX_READY 1
#define RX_READY 2

#define RX_BUFFER_SIZE 16

typedef struct
{
    char DATA;
    int STATUS;
    int CONTROL;
    int BAUD;

    UART_Config CONFIG;

    /* RX frame state */
    char RX_DATA;
    int RX_BIT_POSITION;
    int RX_DATA_BITS_RECEIVED;
    int RX_PARITY;

    int RX_PARITY_RECEIVED;
    int RX_STOP_BITS_RECEIVED;
    int RX_FRAME_ERROR;

    char RX_BUFFER[RX_BUFFER_SIZE];
    int RX_HEAD;
    int RX_TAIL;

} UART_Registers;

static UART_Registers UART;

static void UART_TransmitBit(int bit);
static void UART_ProcessReceivedBit(int bit);
static int UART_CalculateParity(char ch, int data_bits);
static void UART_StoreReceivedCharacter(char ch);
static int UART_ValidateConfig(const UART_Config *config);


void UART_Init(const UART_Config *config)
{
    if (!UART_ValidateConfig(config))
    {
        UART.STATUS = 0;
        return;
    }

    UART.BAUD = config->baud_rate;
    UART.CONFIG = *config;

    UART.STATUS = TX_READY;
    UART.CONTROL = 0;

    UART.RX_DATA = 0;
    UART.RX_BIT_POSITION = 0;
    UART.RX_DATA_BITS_RECEIVED = 0;
    UART.RX_PARITY = 0;

    UART.RX_PARITY_RECEIVED = 0;
    UART.RX_STOP_BITS_RECEIVED = 0;
    UART.RX_FRAME_ERROR = 0;

    UART.RX_HEAD = 0;
    UART.RX_TAIL = 0;
}


void UART_SendChar(char ch)
{
    while (!(UART.STATUS & TX_READY))
    {
        // Wait until TX is ready
    }

    UART.STATUS &= ~TX_READY;

    UART.DATA = ch;

    printf(COLOR_YELLOW "\nTX CHARACTER: %c\n" COLOR_RESET, ch);


    printf("TX START: 0\n");
    UART_TransmitBit(0);


    for (int i = 0; i < UART.CONFIG.data_bits; i++)
    {
        int bit = (ch >> i) & 1;

        printf(DARK_BLUE "TX DATA[%d]: %d\n" COLOR_RESET, i, bit);

        UART_TransmitBit(bit);
    }


    if (UART.CONFIG.parity != PARITY_NONE)
    {
        int parity = UART_CalculateParity(ch, UART.CONFIG.data_bits);

        if (UART.CONFIG.parity == PARITY_EVEN)
        {
            printf("TX PARITY: %d\n", parity);
            UART_TransmitBit(parity);
        }
        else
        {
            printf("TX PARITY: %d\n", !parity);
            UART_TransmitBit(!parity);
        }
    }


    for (int i = 0; i < UART.CONFIG.stop_bits; i++)
    {
        printf("TX STOP[%d]: 1\n", i + 1);

        UART_TransmitBit(1);
    }


    UART.STATUS |= TX_READY;
}


void UART_SendString(const char *str)
{
    while (*str != '\0')
    {
        UART_SendChar(*str);
        str++;
    }
}


static void UART_TransmitBit(int bit)
{
    UART_ProcessReceivedBit(bit);
}


static void UART_ProcessReceivedBit(int bit)
{
    if (UART.RX_DATA_BITS_RECEIVED == 0)
    {
        if (bit == 0)
        {
            printf("RX START: 0\n");

            UART.RX_DATA = 0;
            UART.RX_BIT_POSITION = 0;
            UART.RX_DATA_BITS_RECEIVED = 1;
            UART.RX_PARITY = 0;
            UART.RX_PARITY_RECEIVED = 0;
            UART.RX_STOP_BITS_RECEIVED = 0;
            UART.RX_FRAME_ERROR = 0;

            int next_head = (UART.RX_HEAD + 1) % RX_BUFFER_SIZE;

            if (next_head == UART.RX_TAIL)
            {
                printf(COLOR_RED "RX ERROR: BUFFER FULL\n" COLOR_RESET);
                UART.RX_FRAME_ERROR = 1;
            }
        }
        else
        {
            printf(COLOR_RED "RX ERROR: Invalid START bit\n" COLOR_RESET);
        }

        return;
    }

    if (UART.RX_DATA_BITS_RECEIVED <= UART.CONFIG.data_bits)
    {
        if (UART.RX_FRAME_ERROR == 0)
        {
            printf(LIGHT_BLUE "RX DATA[%d]: %d\n" COLOR_RESET, UART.RX_BIT_POSITION, bit);

            if (bit)
            {
                UART.RX_DATA |= (1 << UART.RX_BIT_POSITION);
            }

            UART.RX_PARITY ^= bit;
        }

        UART.RX_BIT_POSITION++;
        UART.RX_DATA_BITS_RECEIVED++;

        return;
    }


    if (UART.CONFIG.parity != PARITY_NONE && UART.RX_PARITY_RECEIVED == 0)
    {
        int expected_parity;

        if (UART.CONFIG.parity == PARITY_EVEN)
        {
            expected_parity = UART.RX_PARITY;
        }
        else
        {
            expected_parity = !UART.RX_PARITY;
        }

        printf("RX PARITY: %d\n", bit);

        if (UART.RX_FRAME_ERROR == 0)
        {
            if (bit == expected_parity)
            {
                printf(COLOR_GREEN "RX PARITY: VALID\n" COLOR_RESET);
            }
            else
            {
                printf(COLOR_RED "RX ERROR: PARITY ERROR\n" COLOR_RESET);
                UART.RX_FRAME_ERROR = 1;
            }
        }

        UART.RX_PARITY_RECEIVED = 1;

        return;
    }

    if (UART.RX_STOP_BITS_RECEIVED < UART.CONFIG.stop_bits)
    {
        printf("RX STOP[%d]: %d\n", UART.RX_STOP_BITS_RECEIVED + 1, bit);

        if (bit == 1)
        {
            printf(COLOR_GREEN "RX STOP: VALID\n" COLOR_RESET);
        }
        else
        {
            printf(COLOR_RED "RX ERROR: INVALID STOP BIT\n" COLOR_RESET);
            UART.RX_FRAME_ERROR = 1;
        }

        UART.RX_STOP_BITS_RECEIVED++;

        if (UART.RX_STOP_BITS_RECEIVED == UART.CONFIG.stop_bits)
        {
            if (UART.RX_FRAME_ERROR == 0)
            {
                UART_StoreReceivedCharacter(UART.RX_DATA);
            }

            UART.RX_DATA = 0;
            UART.RX_BIT_POSITION = 0;
            UART.RX_DATA_BITS_RECEIVED = 0;
            UART.RX_PARITY = 0;
            UART.RX_PARITY_RECEIVED = 0;
            UART.RX_STOP_BITS_RECEIVED = 0;
            UART.RX_FRAME_ERROR = 0;
        }

        return;
    }
}


static void UART_StoreReceivedCharacter(char ch)
{
    int next_head;

    next_head = (UART.RX_HEAD + 1) % RX_BUFFER_SIZE;


    if (next_head == UART.RX_TAIL)
    {
        printf(COLOR_RED "RX ERROR: BUFFER FULL\n" COLOR_RESET);
        return;
    }


    UART.RX_BUFFER[UART.RX_HEAD] = ch;

    UART.RX_HEAD = next_head;

    UART.STATUS |= RX_READY;
}


char UART_ReceiveChar(void)
{
    while (UART.RX_HEAD == UART.RX_TAIL)
    {
        // Wait for received character
    }

    char ch = UART.RX_BUFFER[UART.RX_TAIL];

    UART.RX_TAIL = (UART.RX_TAIL + 1) % RX_BUFFER_SIZE;

    if (UART.RX_HEAD == UART.RX_TAIL)
    {
        UART.STATUS &= ~RX_READY;
    }

    return ch;
}


int UART_ReceiveReady(void)
{
    return UART.RX_HEAD != UART.RX_TAIL;
}


static int UART_CalculateParity(char ch, int data_bits)
{
    int parity = 0;

    for (int i = 0; i < data_bits; i++)
    {
        parity ^= (ch >> i) & 1;
    }

    return parity;
}


static int UART_ValidateConfig(const UART_Config *config)
{
    if (config == NULL)
    {
        printf(COLOR_RED "UART ERROR: NULL CONFIGURATION\n" COLOR_RESET);
        return 0;
    }

    if (config->baud_rate <= 0)
    {
        printf(COLOR_RED "UART ERROR: INVALID BAUD RATE\n" COLOR_RESET);
        return 0;
    }

    if (config->data_bits < 5 || config->data_bits > 8)
    {
        printf(COLOR_RED "UART ERROR: INVALID DATA BITS\n" COLOR_RESET);
        return 0;
    }

    if (config->stop_bits < 1 || config->stop_bits > 2)
    {
        printf(COLOR_RED "UART ERROR: INVALID STOP BITS\n" COLOR_RESET);
        return 0;
    }

    if (config->parity != PARITY_NONE &&
        config->parity != PARITY_EVEN &&
        config->parity != PARITY_ODD)
    {
        printf(COLOR_RED "UART ERROR: INVALID PARITY\n" COLOR_RESET);
        return 0;
    }

    return 1;
}


double UART_GetBitTime(void)
{
    return 1.0 / UART.BAUD;
}


double UART_GetFrameTime(void)
{
    int total_bits = 1 + UART.CONFIG.data_bits;

    if (UART.CONFIG.parity != PARITY_NONE)
    {
        total_bits++;
    }

    total_bits += UART.CONFIG.stop_bits;

    return (double)total_bits / UART.BAUD;
}