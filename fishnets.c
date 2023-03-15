#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAX_INPUT_SIZE 1024
#define MAX_COMMAND_SIZE 256
#define MAX_FILENAME_SIZE 256
#define MAX_ADDRESS_SIZE 16

// Constants for the different operations
#define OP_READ 'R'
#define OP_WRITE 'W'
#define OP_EXECUTE 'E'

// Constants for the opening and closing characters
#define OPEN_CHAR '{'
#define CLOSE_CHAR '}'

// Struct for storing the start and end addresses for an operation
typedef struct {
    unsigned long long start;
    unsigned long long end;
} AddressRange;

// Function to parse an address range from a string in the format "{start-end}"
AddressRange parse_address_range(char *str) {
    AddressRange range;
    char *start_str = str + 1; // skip over the opening character
    char *end_str = strchr(start_str, '-');
    if (!end_str) {
        range.start = 0;
        range.end = 0;
        return range;
    }
    *end_str = '\0'; // null-terminate the start address string
    end_str++; // move the pointer past the '-'
    range.start = strtoull(start_str, NULL, 16);
    range.end = strtoull(end_str, NULL, 16);
    return range;
}

// Function to read a byte from the specified address
unsigned char read_byte(unsigned long long addr) {
    unsigned char byte;
    memcpy(&byte, (void*)addr, sizeof(byte));
    return byte;
}

// Function to write a byte to the specified address
void write_byte(unsigned long long addr, unsigned char byte) {
    memcpy((void*)addr, &byte, sizeof(byte));
}

// Function to execute machine code instructions at the specified address
void execute_instructions(unsigned long long addr) {
    void (*func)(void) = (void (*)(void))addr;
    func();
}

int main(int argc, char *argv[]) {
    char input[MAX_INPUT_SIZE];
    char command[MAX_COMMAND_SIZE];
    char filename[MAX_FILENAME_SIZE];
    AddressRange range;
    unsigned char byte;
    int bytes_read;
    int mem_fd;
    void *mem_map;

    // Check if running as root
    if (geteuid() != 0) {
        printf("Error: must be run as root.\n");
        exit(1);
    }

    // Open /dev/mem for read-write access
    mem_fd = open("/dev/mem", O_RDWR);
    if (mem_fd < 0) {
        printf("Error: could not open /dev/mem.\n");
        exit(1);
    }

    // Map entire physical address space into userland
    mem_map = mmap(NULL, 0xFFFFFFFFFFFFFFFF, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, mem_fd, 0);
    if (mem_map == MAP_FAILED) {
        printf("Error: could not map physical address space.\n");
        exit(1);
    }

    while (1) {
        // Read a line of input from standard in
        printf("Enter a Fishnets command: ");
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break;
        }

        // Parse the command from the input
       
