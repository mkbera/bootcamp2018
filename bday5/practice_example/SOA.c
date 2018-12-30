#include<stdio.h>
#include <time.h>
#define ITE 1024*30
struct complete_sell{
    int eraser[ITE];
    int note_book[ITE];
    int skech_pens[ITE];
    int posters[ITE];           // structure of array.
    int pencil[ITE];
    int color_pen[ITE];
    int pen[ITE];
    int ink_pen[ITE];
    unsigned long long a[8];
};

char cache_size[(1<<20)*8];
int main(){
    struct complete_sell data;
    int i;
    for(i=0;i<ITE;i++){
      data.pencil[i]=10;
      data.eraser[i]=3;
      data.note_book[i]=40;        //inisializing the data.
      data.skech_pens[i]=20;
      data.posters[i]=5;
      data.color_pen[i]=15;
      data.pen[i]=5;
      data.ink_pen[i]=90;
    }
    
    
    
    
    int j;
    for(j=0;j<(1<<20)*8;j++){
        cache_size[j]='a';          //clearing the cache.
    }



    int sum=0;
    double start=clock();
    for(i=0;i<ITE;i++){             //adding the sum
        sum=sum+data.pencil[i];
    }
    double end=clock();
    double time_taken=((double) (end - start)) / CLOCKS_PER_SEC;
    




    printf("value of sum :: %d \ntime taken :: %lf \n",sum,time_taken);
    return 0;
}
