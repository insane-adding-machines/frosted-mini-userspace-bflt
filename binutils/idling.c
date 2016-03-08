#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int pid;
    int led[4];
    int i, j;

# define LED0 "/dev/led0"
# define LED1 "/dev/led1"
# define LED2 "/dev/led2"
# define LED3 "/dev/led3"

    led[0] = open(LED0, O_RDWR, 0);
    led[1] = open(LED1, O_RDWR, 0);
    led[2] = open(LED2, O_RDWR, 0);
    led[3] = open(LED3, O_RDWR, 0);

    if (led[0] >= 0) {
        while(1) {
            for (i = 0; i < 9; i++) {
                if (i < 4) {
                    write(led[i], "0", 1);
                } else {
                    char val = (1 - (i % 2)) + '0';
                    for(j = 0; j < 4; j++)
                        write(led[j], &val, 1);
                }
                sleep(200);
            }
        }
    } else {
        while(1) { sleep(1000); } /* GPIO unavailable, just sleep. */
    }
}
