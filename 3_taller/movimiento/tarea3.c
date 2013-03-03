#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>

void open(char *filename);
void count(FILE *data);

int main(int argc, char **argv){
  
  FILE *data;
  size_t x,y,i,j;
  open(argv[1]);
  count(data);

  //hacer la matriz provisional
  gsl_matrix *P=gsl_matrix_alloc(y,x);
  gsl_matrix_fscanf(data, P);
  
  // diferenciar datos
  gsl_vector *M=gsl_vector_alloc(y);
  gsl_vector *Y=gsl_vector_alloc(y);
  gsl_matrix_get_col(M, P,1);
  gsl_matrix_get_col(Y, P,0);
  
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
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, *X, *XT, 0.0, *PRO); 
  /* gsl_cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, )*/
  
  // sacar la inversa de X*XT (PRO) creando matriz INV  
  gsl_matrix *INV= gsl_matrix_alloc(x,x);
  gsl_permutation *b= gsl_permutatio_alloc(x);
  
  gsl_linalg_LU_decomp (PRO, p, &y);
  gsl_linalg_LU_invert (PRO, p, INV);

  gsl_permutation_free(b)
  return 0;
}

/* abrir el archivo y guardar el archivo */
void open(char *filename){
  FILE *data;
  data =fopen(filename, "r");
  close(filename);
}

/* contar el numero de filas y columnas del archivo*/
void count(FILE *data){
  int x=0, y=0, test;
  char line;
  float num;
  char text[250];
  do{test= fscanf(data, "/n", &line); 
    if(test!=EOF){
      y++;
    }
  }while(test!=EOF); 
  
  /* do{fscanf(data," ", &line); 
    if(line==" "){
      x++;
    }
    }while(line!="/n");*/
  x=2;
}

/* /*hacer la matriz provisional X*/

/*  hacer el vector Y
  hacer la matriz con los datos bien 
  hacer la transpuesta
  sacar la inversa
*/
