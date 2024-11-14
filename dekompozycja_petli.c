#include<stdio.h>
#include<pthread.h>
#include<math.h>

double funkcja ( double x );

double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w);

static int l_w_global=0;

static double calka_global=0.0;
static double a_global;
static double b_global;
static double dx_global;
static int N_global;

pthread_mutex_t mutex; 

void* calka_fragment_petli_w(void* arg_wsk);

double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w){

  int N = ceil((b-a)/dx);
  double dx_adjust = (b-a)/N;
  l_w_global = l_w;
  a_global = a;
  b_global = b;
  dx_global = dx;
  N_global = N;

  printf("Obliczona liczba trapezów: N = %d, dx_adjust = %lf\n", N, dx_adjust);

  // tworzenie struktur danych do obsługi wielowątkowości
  int indeksy[l_w]; 
  for(int i=0;i<l_w;i++) indeksy[i]=i;
  
  pthread_t watki[l_w];

  // tworzenie wątków
  for(int i=0; i<l_w; i++ ) 
    pthread_create( &watki[i], NULL, calka_fragment_petli_w, (void *) &indeksy[i] );

  // oczekiwanie na zakończenie pracy wątków
  for(int i=0; i<l_w; i++ ) pthread_join( watki[i], NULL );

  return(calka_global);
}

void* calka_fragment_petli_w(void* arg_wsk){

  int my_id;
  my_id = *( (int *) arg_wsk ); 

  double a, b, dx;
  int N, l_w;      

  a = a_global; 
  b = b_global;
  dx = dx_global;
  l_w = l_w_global;
  N = N_global;
  
  // dekompozycja cykliczna
  int my_start = my_id;
  int my_end = N;
  int my_stride = l_w;

  printf("\nWątek %d: my_start %d, my_end %d, my_stride %d\n", 
	 my_id, my_start, my_end, my_stride);
  int i;
  double calka = 0.0;
  for(i=my_start; i<my_end; i+=my_stride){

    double x1 = a + i*dx;
    calka += 0.5*dx*(funkcja(x1)+funkcja(x1+dx));
  }

  pthread_mutex_lock(&mutex);
  calka_global += calka;
  pthread_mutex_unlock(&mutex);
}

