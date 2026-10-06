#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "uart.h"

int main(void)
{
    UART_Config config =
    {
        .baud_rate = 9600,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = PARITY_EVEN
    };

    char message[100];
    char received[100];
    int received_count = 0;

    UART_Init(&config);

    printf(COLOR_MAGENTA "\n========================================\n");
    printf("        UART DRIVER SIMULATION\n");
    printf("========================================\n");

    printf("\nUART Configuration\n");
    printf("Baud Rate : %d\n", config.baud_rate);
    printf("Data Bits : %d\n", config.data_bits);
    printf("Parity    : NONE\n");
    printf("Stop Bits : %d\n", config.stop_bits);
    
    printf("Bit Time   : %.2f us\n", UART_GetBitTime() * 1000000);
    printf("Frame Time : %.2f ms\n" COLOR_RESET, UART_GetFrameTime() * 1000);

    printf(COLOR_YELLOW "\nEnter message: ");
    fgets(message, sizeof(message), stdin);

    message[strcspn(message, "\n")] = '\0';

    printf("\n");

    printf(COLOR_ORANGE "\rTransmitting.");
    fflush(stdout);
    usleep(400000);

    printf("\rTransmitting..");
    fflush(stdout);
    usleep(400000);

    printf("\rTransmitting...");
    fflush(stdout);
    usleep(400000);

    printf("\rTransmitting....\n" COLOR_RESET);
    fflush(stdout);

    UART_SendString(message);

    printf(COLOR_GREEN "\nTransmission Complete.\n" COLOR_RESET);

    printf(COLOR_ORANGE "\nReceiving....\n" COLOR_RESET);
    sleep(2);

    while (UART_ReceiveReady())
    {
        received[received_count++] = UART_ReceiveChar();
    }

    received[received_count] = '\0';

    printf(COLOR_YELLOW "\nReceived: %s\n" COLOR_RESET, received);

    printf(COLOR_MAGENTA "\n========================================\n" 
           "     UART DRIVER SIMULATION COMPLETE\n"
           "========================================\n" COLOR_RESET);

    return 0;
}