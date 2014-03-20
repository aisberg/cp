#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

double f (double a) {
	return a*a;
}

double integrate (double a, double b, double dx, double (*f)(double)) {
	double x, result = 0;
	for (x = a; x < b; x += dx) 
		result += ((f(x) + f(x + dx))/2)*dx;
	return result;
}

/*void mp_integrate (double a, double b, double dx, double(*f)(double), int procNum) {
	double delta = (b - a) / (double)procNum;
        int i;
	for (i = 0; i < procNum; ++i) {
		int pid = fork();
		if (pid == 0) {
			int fd = open ("tmp", O_WRONLY | O_CREAT | O_APPEND, 0666);
			double res = integrate (a + delta * (double)i, a + delta * (double)(i + 1), dx, f);
			write (fd, &res, sizeof(res));
			close(fd);
			exit(0);
		}
		else {
			if (pid == -1)
				printf ("error\n");
		}
	}
	double res_total = 0., res;
	int fd = open ("tmp", O_RDONLY);
	for (i = 0; i < procNum; ++i) {
		while (read (fd, &res, sizeof(res)) != sizeof(res))
			sleep(1);
		res_total += res;
	}
	close(fd);
	unlink ("tmp");
	printf("%f\n", res_total);
}*/

double mp_integrate (double a, double b, double dx, double(*f)(double), int procNum) {
	double delta = (b - a) / (double)procNum;
	int fd[2];
	pipe(fd);
int i;
	for (i = 0; i < procNum; ++i) {
		int pid = fork();
		if (pid == 0) {
			close(fd[0]);
			double res = integrate (a + delta * (double)i, a + delta * (double)(i + 1), dx, f);
			write(fd[1], &res, sizeof(res));
			close(fd[1]);
		}
		else {
			if (pid < 0)
				perror("ERROR");
		}
	}
	close(fd[1]);
	double res_total = 0., res;
	for (int i = 0; i < procNum; ++i) {
		read (fd[0], &res, sizeof(res));
		res_total += res;
	}
	close(fd[0]);
	return res_total;
}

int main (int argc, char **argv) {
	double res = mp_integrate (0., 5., 0.2, f, 4);
	printf("%lf\n", res);
	return 0;
}
