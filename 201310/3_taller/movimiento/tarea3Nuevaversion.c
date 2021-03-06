#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>

FILE * opendata(char *filename);
void countdata(FILE *data, int *n_x, int *n_y);
double *loaddata(FILE *data, int x, int y);

int main(int argc, char **argv){
  
  FILE *data;
  double *matriz;
  int x,y,i,j;

  data = opendata(argv[1]);
  countdata(data, &x, &y);
  

  matriz = loaddata(data,x,y);
  
  gsl_matrix_view P =gsl_matrix_view_array(matriz,y,x);
  // diferenciar datos
  gsl_vector *M=gsl_vector_alloc(y);
  gsl_vector *Y=gsl_vector_alloc(y);
  gsl_matrix_get_col(M, &P.matrix,1);
  gsl_matrix_get_col(Y, &P.matrix,0);
  
  // operar los elementos de la matriz M para obtener la matriz X de datos
  gsl_matrix *X= gsl_matrix_alloc(y,3);
  for(i=0;i<3;i++){
    for(j=0;j<y;j++){
      if(i==0){
	gsl_matrix_set(X,i,j,1);
      }
      if(i==1){
	double number= gsl_vector_get(M,j);
	gsl_matrix_set(X,i,j,number);
      }
      if(i==2){
	double number= gsl_vector_get(M,j);
	double cnumber= (0.5)*number*number;
	gsl_matrix_set(X,i,j,cnumber);
      }
    }
  }
  // hacer la transpuesta de la matriz X, se crea XT
  gsl_matrix *XT= gsl_matrix_alloc(3,y);
  gsl_matrix_transpose_memcpy(XT,X);
  
  // se multiplican las matrices creando una nueva matrix PRO
  gsl_matrix *PRO= gsl_matrix_alloc(y,y);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, *XT, *X, 0.0, *PRO); 
  
  // sacar la inversa de X*XT (PRO) creando matriz INV  
  gsl_matrix *INV= gsl_matrix_alloc(3,3);
  gsl_permutation *b= gsl_permutation_alloc(3);
  int *z= (int*) y;
  gsl_linalg_LU_decomp (PRO, b,z);
  gsl_linalg_LU_invert (PRO, b, INV);

  gsl_permutation_free(b);

  //hacer la multiplicacion 
  gsl_matrix *A= gsl_matrix_alloc(x,y);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, *INV, *XT, 0.0, *A);
  
  gsl_matrix *BETA= gsl_matrix_alloc(3,1);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, *A, *Y, 0.0, *BETA);
  float beta1 =(float) gsl_matrix_get(BETA,0,0);
  float beta2 =(float) gsl_matrix_get(BETA,1,0);
  float beta3 =(float) gsl_matrix_get(BETA,2,0);
  
  FILE *fileout;
  fileout=fopen("parametro_movimiento.dat","w");
  write(fileout, "%f %f %f\n",beta1,beta2,beta3);
  close(fileout);
  return 0;
}

/*cargar el array*/
double *loaddata(FILE* data, int x, int y){
  int j;
  double *array;
  float a,b;
  
  if(!(array = malloc(sizeof(double)*x *y))){
    fprintf(stderr, "Problem with memory allocation");
    exit(1);
  }
  for(j=0;j<y; j++)
    {
      fscanf(data, "%f %f \n",&a,&b);
	array[x*j + 0]=(double) a; 
	array[x*j + 1]=(double) b;
    }
  return array;
}

/* abrir el archivo y guardar el archivo */
FILE * opendata(char *filename){
  FILE *data;
  data =fopen(filename, "r");
  close(filename);
  return data;
}

/* contar el numero de filas y columnas del archivo*/
void countdata(FILE *data, int *n_x, int *n_y){
  int y=0, test;
  char line;
  do{
    test = fgetc(data); 
    if(test=='\n'){
      y++;
    }
  }while(test!=EOF); 
  *n_x = 2;
  *n_y = y;
}

