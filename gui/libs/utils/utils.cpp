#include "utils.hpp"

void configureTermios(int* fileDescriptor) {

    struct termios options;
    tcgetattr(*fileDescriptor, &options);               // Get the current options
    cfsetispeed(&options, B9600);                       // Set input baud rate to 9600
    cfsetospeed(&options, B9600);                       // Set output baud rate to 9600
    options.c_cflag &= ~PARENB;                         // Disable parity checking
    options.c_cflag &= ~CSTOPB;                         // Use 1 stop bit
    options.c_cflag &= ~CSIZE;                          // Mask the character size
    options.c_cflag |= CS8;                             // Select 8-bit data size
    options.c_cflag |= (CLOCAL | CREAD);                // Enable receiver and ignore modem control lines
    options.c_cflag &= ~CRTSCTS;                        // Disable hardware flow control
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Disable canonical, echoing, signal generation (raw input)
    options.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable software flow control
    options.c_oflag &= ~OPOST;                          // Select raw output
    options.c_cc[VMIN]  = 0;                            // Return immediately if data is available
    options.c_cc[VTIME] = 10;                           // Return 0 if no data arrives within 1 second
    tcsetattr(*fileDescriptor, TCSANOW, &options);      // Set the new options

    return;
}