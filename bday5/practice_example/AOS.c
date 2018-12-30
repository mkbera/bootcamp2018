#include<stdio.h>
#include <time.h>
#define ITE 1024*30
struct daily_sell{
    int eraser;
    int note_book;
    int skech_pens;
    int posters;            // structure.
    int pencil;
    int color_pen;
    int pen;
    int ink_pen;
    unsigned long long a[8];
};
char cache_size[(1<<20)*8];
int main(){
  struct daily_sell day[ITE];  //array of structure.
  int i;
  for(i=0;i<ITE;i++){
      day[i].pencil=10;
      day[i].eraser=3;         //inisializing the values.
      day[i].note_book=40;
      day[i].skech_pens=20;
      day[i].posters=5;
      day[i].color_pen=15;
      day[i].pen=5;
      day[i].ink_pen=90;
  }
    
    int j;
    for(j=0;j<(1<<20)*8;j++){
        cache_size[j]='a';             //clearing the cache.
    }
    
    int sum=0;
    double start=clock();
    for(i=0;i<ITE;i++){
        sum=sum+day[i].pencil;        //adding the sum.
    }
    double end=clock();
    double time_taken=((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("value of sum :: %d \ntime taken :: %lf \n",sum,time_taken);

}
