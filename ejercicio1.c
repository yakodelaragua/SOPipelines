#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define error0 "Error al crear pipe\n"
#define error1 "Error open\n"
#define error2 "Error close\n"
#define error3 "Error al ejecutar\n"
#define error4 "Error al crear proceso hijo\n"
#define error5 "Error dup\n"

//exec: cat /etc/passwd | grep acaf00 | wc -l
int main() {  
   int pfd[2], pid, f;

   if (pipe(pfd) == -1) write(1, error0, sizeof(error0));

   switch (pid = fork()) {
      case -1: write(1, error4, sizeof(error4));
      case 0: //who
         if (close(1) == -1) write(1, error2, sizeof(error2));
         if (dup(pfd[1]) != 1) write(1, error5, sizeof(error5));
         close(pfd[0]); close(pfd[1]);
         execlp("cat", "cat", "/etc/passwd", NULL);
         write(1, error3, sizeof(error3));
   }

   switch (pid = fork()) {
      case -1: write(1, error4, sizeof(error4));
      case 0: //grep acaf00
         if (close(0) == -1) write(1, error2, sizeof(error2));;
         if (dup(pfd[0]) != 0) write(1, error5, sizeof(error5));
         close(pfd[0]); close(pfd[1]);
         close(1);
         if((f = open("fich", O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 1) write(1, error1, sizeof(error1));
         execlp("grep", "grep", "acaf00", NULL);
         write(1, error3, sizeof(error3));
   }
   close(pfd[0]); close(pfd[1]);


   while (wait(NULL) > 0);

   if (pipe(pfd) == -1) write(1, error0, sizeof(error0));

   switch (pid = fork()) {
      case -1: write(1, error4, sizeof(error4));
      case 0: //cat fich
         if (close(1) == -1) write(1, error2, sizeof(error2));
         if (dup(pfd[1]) != 1) write(1, error5, sizeof(error5));
         close(pfd[0]); close(pfd[1]);
         execlp("cat", "cat", "fich", NULL);
         write(1, error3, sizeof(error3));
   }

   switch (pid = fork()) {
      case -1: write(1, error4, sizeof(error4));
      case 0: //wc -l
         if (close(0) == -1) write(1, error2, sizeof(error2));
         if (dup(pfd[0]) != 0) write(1, error5, sizeof(error5));
         close(pfd[0]); close(pfd[1]);
         execlp("wc", "wc", "-l", NULL);
         write(1, error3, sizeof(error3));
   }
  
   close(pfd[0]); close(pfd[1]);

   while ((pid = wait(NULL)) != -1)

   close(f);
   unlink("fich");
   exit(0);
}
