#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define error0 "Error al abrir my_fifo\n"
#define error1 "Error open\n"
#define error2 "Error close\n"
#define error3 "Error al ejecutar\n"
#define error4 "Error al crear proceso hijo\n"

//exec: who | grep acaf00 | wc -l
int main() { 
   int pid;
   dev_t dev = 1;
   int f = 0;

   unlink("my_fifo");
   if (mknod("my_fifo", 0644 | S_IFIFO, dev) == -1)
      write(1, error0, sizeof(error0));
      
   switch (pid = fork()) { //who
      case -1: write(1, error4, sizeof(error4));
      case 0:
      close(1);
      if (close(1) == -1) write(1, error2, sizeof(error2));
      if (open("my_fifo", O_WRONLY) != 1) write(1, error1, sizeof(error1));
      execlp("who", "who", NULL);
      write(1, error3, sizeof(error3));
   }
   
   switch (pid = fork()) { //grep
      case -1: write(1, error4, sizeof(error4));;
      case 0: 
      if (close(0) == -1) write(1, error2, sizeof(error2));
      if (open("my_fifo", O_RDONLY) != 0) write(1, error1, sizeof(error1));
      close(1);
      if((f = open("fich", O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 1) write(1, error1, sizeof(error1));
      execlp("grep", "grep", "acaf00", NULL);
      write(1, error3, sizeof(error3));
   }

   while(wait(NULL) > 0);

   switch (pid = fork()) { //cat fich
      case -1: write(1, error4, sizeof(error4));
      case 0:
         if (close(1) == -1) write(1, error2, sizeof(error2));
         if (open("my_fifo", O_WRONLY) != 1) write(1, error1, sizeof(error1));
         execlp("cat", "cat", "fich", NULL);
         write(1, error3, sizeof(error3));
   }
   
   switch (pid = fork()) { //wc -l
      case -1: write(1, error4, sizeof(error4));
      case 0: 
      if (close(0) == -1) write(1, error2, sizeof(error2));
      if (open("my_fifo", O_RDONLY) != 0) write(1, error1, sizeof(error1));
      execlp("wc", "wc", "-l", NULL);
      write(1, error3, sizeof(error3));
   }

   while ((pid = wait(NULL)) != -1)

   unlink("my_fifo");
   close(f);
   unlink("fich");

   exit(0);
}