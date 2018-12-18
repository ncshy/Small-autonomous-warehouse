#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void logger(FILE *fp, const char* tag, const char* message) {
   char log_message[150];
   time_t now;
   time(&now);
 
   fprintf(fp , "%s [%s]: %s\n", ctime(&now), tag, message);
 //  sprintf(log_message , "%s [%s]: %s\n", ctime(&now), tag, message);
 //  fwrite(log_message, sizeof(log_message), 1, fp);
}
