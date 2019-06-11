#include <stdio.h>
#include <gsl/gsl_fit.h>
#include <cstring> 　
int main (void)
{
  int n = 19;
FILE *fp = NULL;
char *line,*record;
char buffer[1024];
int j = 0;
double a[100][10];
int i = 0;
if ((fp = fopen("./data/spring_data.csv", "at+")) != NULL)
{
	fseek(fp, 22, SEEK_SET);  
	char delims[] = ",";
	char *result = NULL;
		while ((line = fgets(buffer, sizeof(buffer), fp))!=NULL)
		{
			record = strtok(line, ",");
			while (record != NULL)
			{
        a[i][j] = atof(record);
				if (j == 1) 
					break;
				record = strtok(NULL, ",");
				j++;
        i++;
			}
			j = 0;
		}
		fclose(fp);
		fp = NULL;
    }
  
  double x[19],y[19];
  for(int m = 0; m < 19; m++)
   {
     x[m]=a[m+1][1]*9.81;
     y[m]=a[m][0];
   }


  double c0, c1, cov00, cov01, cov11, sumsq;

  gsl_fit_linear (x, 1, y, 1, n,
                   &c0, &c1, &cov00, &cov01, &cov11,
                   &sumsq);
  
  printf ("best fit: Y = %g + %g X\n", c0, c1);
  printf ("covariance matrix:\n");
  printf ("[ %g, %g\n  %g, %g]\n",
          cov00, cov01, cov01, cov11);
  printf ("sumsq = %g\n", sumsq);
  printf ("k = %g\n", (1/c1));

  printf ("\n");

  // plot
  FILE *gnuplotPipe = popen("gnuplot -persist", "w"); // Open a pipe to gnuplot
  if (gnuplotPipe) // If gnuplot is found
  { 
     fprintf(gnuplotPipe, "set xlabel 'Force(Newtons)'\n");
     fprintf(gnuplotPipe, "set ylabel 'Distance(meters)'\n");
     fprintf(gnuplotPipe, "set title 'Measured Displacement of Spring'\n");
     fprintf(gnuplotPipe, "set key left\n");

     /* 
        1 sending gnuplot the plot '-' command 
        2 followed by data points 
        3 followed by the letter "e" 
     */
     
     // 1  sending gnuplot the plot '-' command
     fprintf(gnuplotPipe, "plot '-' title '<x,y>' with point pointtype 7 pointsize 1 lt 7, '-' title 'Line' with line linetype 6 linewidth 3 \n");
     
     // 2 followed by data points: <x,y>
     for (int i = 0; i < n; i++)
     {
        fprintf(gnuplotPipe, "%lf %lf\n", x[i], y[i]);
     }
     // 3 followed by the letter "e" 
     fprintf(gnuplotPipe, "e");
     
     // linear fit
     for (int i = -1; i < n; i++)   // i=0 -> i=-1
     {
        fprintf(gnuplotPipe, "%lf %lf\n", x[i], c0+c1*x[i]);
     }
     fprintf(gnuplotPipe, "e");
      
     fflush(gnuplotPipe);
     fprintf(gnuplotPipe, "exit \n"); // exit gnuplot
     pclose(gnuplotPipe);             //close pipe
  }
  
  return 0;
}

