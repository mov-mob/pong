
/*
#include <stdio.h>
#include <stdlib.h> //random()
#include <locale.h> //idioma

#include <unistd.h>	//  POSIX flags
#include <time.h>	//  clock_gettime(), time()
#include <sys/time.h> //	gethrtime(), gettimeofday()

#include <ncurses.h> //entrada/salida de consola


#include <sys/select.h> //para definir kbhit
*/

/*
.PTR
mover_enemigo():
  si SUELO => movimiento_pala influye dirección hacia arriba

  sino mueve_en_diagonal //experimentar función dentro de función
*/

#include "gameutil.c"

#define clrscr() erase()
//#define xprintf(param) printw(param)
#define xprintf printw
#define gotoxy(x,y) move(y,x)

#define ESCAPE 27

#define SUELO 22
#define SCORE 23
//#define Espera(tiempo) Delay(tiempo)

#define PARED_IZQUIERDA 0   //Columna donde está la pala izquierda
#define PARED_DERECHA   79  //Columna donde está la pala derecha


//OBJETOS:
#define PALA "___"
#define PELOTA "O"
#define BORRA_PALA "   "

#define IZQUIERDA 1
#define DERECHA 2
#define ARRIBA 3
#define ABAJO 4

#define X_PALA_IZQUIERDA 0
#define Y_PALA_IZQUIERDA 10


#define X_PALA_DERECHA 79
#define Y_PALA_DERECHA 10

#define QUIETO 0
#define NINGUNO -1 //LA PALA ESTÁ ESTÁTICA AL PRINCIPIO

            /* Sentido y dirección */
#define S 1 /*   |v */
#define N 2 /*   |^ */
#define NE 3 /* \ ^*/
#define SE 4 /*  \ V */
#define SO 5 /*  / V */
#define NO 50 /*  \ ^ */

int juego=FALSE; //el juego no ha empezado aún


int tecla=32;
double tiempo_retardo=0.2;

int sentido_pala=QUIETO; //IZQUIERDA,DERECHA o QUIETO
int x_pelota=40, y_pelota=10;

//la coordenada X estará fijada para todo el juego, la coordenada Y será móvil y tendrá un valor inicial
int x_pala_1=X_PALA_IZQUIERDA, y_pala_1=Y_PALA_IZQUIERDA; //Coordenadas de la pala de la izquierda al principio
int x_pala_2=X_PALA_DERECHA, y_pala_2=Y_PALA_DERECHA; //Coordenadas de la pala de la derecha al principio

int incrementa_y_pala1=0; //sólo varía la coordenada Y
int incrementa_y_pala2=0;

//int mueve_jugador=0;
int mueve_jugador_1=QUIETO;
int mueve_jugador_2=QUIETO;

int mueve_enemigo=1;
int sentido_pelota=SE;
int x_inc_pelota=1;
int y_inc_pelota=1;


//SCORE
int puntos_izquierda=0, fallos_izquierda=0, puntos_derecha=0, fallos_derecha=0;
int bonus_izquierda=0, bonus_derecha=0;

void EsperarEvento();
void ProcesarTecla();
void MoverJugador();
void MoverEnemigo();
void VisualizaJugador();
void VisualizaEnemigo();
void ActualizaPantalla();
int choque();
void Score();

WINDOW *w_terminal;

int main()
{
/* Algoritmo:
while(tecla != ESCAPE){
    if(kbhit(){ //EsperarEvento()
       tecla=getch();
    }
    ProcesarTecla();
    if(muevejugador)
        MoverJugador();
    if(mueveenemigo){
        MoverEnemigo();
    VisualizaJugador();
    VisualizaEnemigo();
    ActualizaPantalla();
    Espera(tiempo);
}
*/

    setlocale(LC_ALL,"es_ES");

//Inicio Modo ncurses:
    w_terminal=initscr(); // entering ncurses mode

    //raw();     // CTRL-C and others do not generate signals
    noecho();  // pressed symbols wont be printed to screen
    //cbreak();  // disable line buffering
    curs_set(0); //hide the cursor
//Fin inicio Modo ncurses ^__

//Comienza la acción

    clrscr();
    //gotoxy(79,0);printw("x");
    refresh();
    //move(23,10); printw("xpala=%d ypala=%d",x_pala,y_pala);
    //refresh();
    //fflush(stdin);
    //sentido_pala=NINGUNO;

    //MoverJugador();
    VisualizaJugador();
    while(tecla!=ESCAPE){


        EsperarEvento();
        //refresh();
        //ProcesarTecla();
        //if(mueve_jugador_1 || mueve_jugador_2) MoverJugador();
        refresh();
        if(mueve_enemigo) MoverEnemigo();
        //VisualizaJugador();
        VisualizaEnemigo();
        Score();
        //ActualizaPantalla();
        refresh();
        //Espera(tiempo_retardo);
    }

//Fin ncurses:
    curs_set(1);
    endwin();
return 0;
}

void Score(){
    //y_pelota==0
/*
    if(y_pelota==SUELO && juego){
        if(x_pelota>=x_pala && (x_pelota<=(x_pala+2))) ++puntos;
        else ++fallos;
    }
*/

    if(x_pelota==PARED_IZQUIERDA){
        if(y_pelota >= y_pala_1 && y_pelota <= y_pala_1+2) {
            bonus_izquierda+=1;
        }
        else{
            puntos_derecha+=1;
        }

    }
    if(x_pelota==PARED_DERECHA){
        if( ( y_pelota >= y_pala_2 ) && (y_pelota <= (y_pala_2+2))){
            bonus_derecha+=1;
        }
        else{
            puntos_izquierda+=1;
        }
    }

    gotoxy(10,SCORE);printw(" puntos: %d bonus: %d              puntos: %d bonus: %d",puntos_izquierda, bonus_izquierda,\
        puntos_derecha,bonus_derecha);
}

void EsperarEvento(){
  double startTime,endTime;
    startTime=GetRealTime();
    endTime=GetRealTime();
    do{
        endTime=GetRealTime();
        if(kbhit()){tecla=getch();ProcesarTecla();}
    }while((endTime-startTime)<tiempo_retardo);
}
void ProcesarTecla(){
    //mueve_jugador_1=QUIETO;
    //mueve_jugador_2=QUIETO;


    //while(tecla=getch()){

     //  if(tecla==27)break;
    //gotoxy(10,5);printf("%c",tecla);

    if(tecla=='A' || tecla=='a') mueve_jugador_1=ABAJO;
    if(tecla=='Q' || tecla=='q') mueve_jugador_1=ARRIBA;

    //MoverJugador();
    //VisualizaJugador();

    if(tecla=='O' || tecla=='o') mueve_jugador_2=ARRIBA;
    if(tecla=='L' || tecla=='l') mueve_jugador_2=ABAJO;
    MoverJugador();
    VisualizaJugador();


    //}
}
void MoverJugador(){
    incrementa_y_pala1=incrementa_y_pala2=0;


    if(mueve_jugador_1==ARRIBA){
        incrementa_y_pala1=(y_pala_1==0?0:-1);//-1;
        //if((y_pala+incrementa_x_pala)==-1) incrementa_x_pala=0;
    }
    if(mueve_jugador_1==ABAJO){
        incrementa_y_pala1=(y_pala_1==20?0:1);//1;
        //if((x_pala+incrementa_x_pala==78)) incrementa_x_pala=0;
    }
    if(mueve_jugador_1){VisualizaJugador();mueve_jugador_1=QUIETO;}

    if(mueve_jugador_2==ARRIBA){
        incrementa_y_pala2=(y_pala_2==0?0:-1);//-1;
        //if((y_pala+incrementa_x_pala)==-1) incrementa_x_pala=0;
    }
    if(mueve_jugador_2==ABAJO){
        incrementa_y_pala2=(y_pala_2==20?0:1);//1;
        //if((x_pala+incrementa_x_pala==78)) incrementa_x_pala=0;
    }
    if(mueve_jugador_2){VisualizaJugador();mueve_jugador_2=QUIETO;}
}

void MoverEnemigo(){


//lanzar la pelota
/*
if(y_pelota==SUELO && mueve_jugador!=QUIETO){ //la pelota está sobre la pala cuando se mueve
    juego=TRUE;
    if(mueve_jugador==IZQUIERDA){
        sentido_pelota=NE;
        y_inc_pelota=-1;
        x_inc_pelota=1;

    }
    else{
        sentido_pelota=NO;
        y_inc_pelota=-1;
        x_inc_pelota=-1;
    }
}
else{ //la pelota está en el aire*/
    if(choque()){
        switch(sentido_pelota){
            case NE:
                if(y_pelota==0){
                    if(x_pelota==79){
                        sentido_pelota=SO;
                    }
                    else sentido_pelota=SE;
                }
                else{
                    if(x_pelota==79) sentido_pelota=NO;
                }
                break;
            case SE:
                if(y_pelota==SUELO){
                //printf("SUELO");
                    if(x_pelota==79) sentido_pelota=NO;
                    else sentido_pelota=NE;
                }
                else sentido_pelota=SO;
                break;
            case NO:/*
                if(y_pelota==0){
                    if(x_pelota==0) {sentido_pelota=SE; }
                    else {sentido_pelota=SO; printw("match");}
                }
                else sentido_pelota=NE;*/
                if(y_pelota==0){ //printw("%d",x_pelota);
                    if(x_pelota==0) sentido_pelota=SE;
                    else sentido_pelota=SO;
                }
                else sentido_pelota=NE;
                break;
            case SO:
                if(y_pelota==SUELO){
                    if(x_pelota==0) sentido_pelota=NE;
                    else sentido_pelota=NO;
                }
                else sentido_pelota=SE;

            default:;
        }
    }
//}
switch(sentido_pelota){
            case SO:
                x_inc_pelota=-1; y_inc_pelota=1;
                break;
            case SE:
                x_inc_pelota=1; y_inc_pelota=1;
                break;
            case NO:

                x_inc_pelota=-1; y_inc_pelota=-1;
                break;
            case NE:
                //printf("NE");
                x_inc_pelota=1;y_inc_pelota=-1;
                break;
        }


}
int choque(){
    if(y_pelota==0 || x_pelota==0 || x_pelota==79 || y_pelota==SUELO){
        //if(((mueve_jugador_1!=QUIETO) && y_pelota==SUELO))Score();
        return 1;
    }
    else{
        return 0;
    }
}

void VisualizaJugador(){
    int i;

//JUgador 1
    for(i=0;i<3;i++){
        gotoxy(x_pala_1,y_pala_1+i);
        xprintf(" ");
    }
    //gotoxy(x_pala_1,y_pala_1);
    //xprintf(BORRA_PALA);

    y_pala_1+=incrementa_y_pala1;

    for(i=0;i<3;i++){
        gotoxy(x_pala_1,y_pala_1+i);
        xprintf("|");
    }

//JUgador 2
    for(i=0;i<3;i++){
        gotoxy(x_pala_2,y_pala_2+i);
        xprintf(" ");
    }
    //gotoxy(x_pala_1,y_pala_1);
    //xprintf(BORRA_PALA);

    y_pala_2+=incrementa_y_pala2;

    for(i=0;i<3;i++){
        gotoxy(x_pala_2,y_pala_2+i);
        //xprintf("%c",250);
        xprintf("|");
        //addch(100+A_ALTCHARSET);

    }

    //gotoxy(10,5); xprintf("%c",tecla); //debug

    refresh();
    //incrementa_y_pala1=incrementa_y_pala2=0;

}
void VisualizaEnemigo(){
    gotoxy(x_pelota,y_pelota);xprintf(" ");
    x_pelota+=x_inc_pelota;
    y_pelota+=y_inc_pelota;
    gotoxy(x_pelota,y_pelota);
    printw(PELOTA);
    refresh();
}
void ActualizaPantalla(){
    refresh();
}
