#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

//define pour l'affichage
#define CASE_SIZE 64
#define WIDTH 1300
#define HEIGTH 720
#define DX 0
#define DY 0

//define pour le déplacement
#define PLAYER 20
#define MV 10
#define DA 0.15
#define VARIATION_ANGLE 0.005
#define PI 3.14159265

//define pour l'envoie de rayons
#define ANGLE_FOCAL PI/4
#define T (int)((PI-2*ANGLE_FOCAL)/VARIATION_ANGLE)
#define N 10

//define pour l'affichage 3D
#define XX (N+1)*CASE_SIZE 
#define LARGEUR (1300 - XX)/T
#define CENTRE N*CASE_SIZE/2
#define HM (N)*CASE_SIZE



//tableau représentant la map
int map[10][10]= {{1,1,1,1,1,1,1,1,1,1},
            {1,0,1,0,0,0,0,0,0,1},
            {1,0,1,0,1,1,1,0,0,1},
            {1,0,1,0,1,0,0,0,0,0},
            {1,0,1,0,1,0,0,0,0,0},
            {1,0,1,0,1,1,0,0,0,1},
            {1,0,0,0,0,1,1,0,0,1},
            {1,0,0,0,0,1,1,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}};

void creer_fenetre(SDL_Window **window, SDL_Renderer **renderer)
{
    /*Initialise la SDL: crée la fenêtre et le rendu */

    //Initilise la SDL
    if (SDL_Init(SDL_INIT_VIDEO)==-1){
		SDL_Log("ERREUR: Initialisation echouee> %s\n",SDL_GetError()); //Affichage de l'erreur
		exit(EXIT_FAILURE);		//On quitte le programme
    }

    //taille de la fenêtre
	int L=WIDTH;
	int l=HEIGTH;
    *window=NULL;
	*window =SDL_CreateWindow("RAYCASTING",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED ,L,l,SDL_WINDOW_RESIZABLE);

    //Vérification de la création de la fenêtre
	if(*window == NULL){
		SDL_Log("ERREUR: Creation de fenetre echouee > %s\n",SDL_GetError());
		exit(EXIT_FAILURE);
	}

    //Création du rendu
    *renderer=NULL;
	*renderer= SDL_CreateRenderer(*window,-1,SDL_RENDERER_ACCELERATED);

    //Vérification de la création du rendu
	if(*renderer==NULL){
		SDL_Log("ERREUR: Creation de fenetre echouee > %s\n",SDL_GetError());
		exit(EXIT_FAILURE);
	}

    //Coloriage du fond
	SDL_SetRenderDrawColor(*renderer, 255, 255, 250, 255); 
	SDL_RenderClear(*renderer);

}

void fond_blanc(SDL_Renderer **renderer)
{
    /*Met le rendu en blanc*/
    //Coloriage du fond
	SDL_SetRenderDrawColor(*renderer, 255, 255, 250, 255); 
	SDL_RenderClear(*renderer);
}

void Dessine_joueur(int px, int py, SDL_Renderer **renderer)
{
    /*dessine  le joueur sur la fenêtre sous la forme d'un carré */

    SDL_Rect rect = {px -PLAYER/2,py -PLAYER/2,PLAYER/2,PLAYER};
    SDL_Rect rect2 = {px,py -PLAYER/2,PLAYER/2,PLAYER};

    SDL_SetRenderDrawColor(*renderer,255,215,0,255);
    SDL_RenderFillRect(*renderer,&rect);
    SDL_RenderFillRect(*renderer,&rect2);

}


int move_player(int *px, int *py,float *angle,int tab[][N])
{
/*Fonction qui permet au joueur de saisir une direction*/
    SDL_Event event;
    int ty=0;
    int tx=0;

    while(SDL_WaitEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                return -1;

            case SDL_KEYDOWN :
                switch(event.key.keysym.sym){
                    case SDLK_UP:
                        tx=*px +MV*cos(*angle);
                        ty=*py +MV*sin(*angle);
                        tx=(int)(tx/CASE_SIZE);
                        ty=(int)(ty/CASE_SIZE);

                        if(tab[ty][tx]!=1){
                            *px=*px +MV*cos(*angle);
                            *py=*py +MV*sin(*angle);
                        }
                        return 1;

                    case SDLK_DOWN:
                        tx=*px -MV*cos(*angle);
                        ty=*py -MV*sin(*angle);
                        tx=(int)(tx/CASE_SIZE);
                        ty=(int)(ty/CASE_SIZE);

                        if(tab[ty][tx]!=1){
                            *px=*px -MV*cos(*angle);
                            *py=*py -MV*sin(*angle);
                        }
                        return 1;

                    case SDLK_LEFT:
                        *angle -= DA;
                        return 1;

                    case SDLK_RIGHT:
                        *angle += DA;
                        return 1;

                    case SDLK_ESCAPE:
                        return -1;
                }
        }
    }
    return 0;
}

void print_tab(int tab[][10])
{
    //déclaration des variables
    int i;
    int j;

    for(i=0;i<10;i++){
        for(j=0;j<10;j++){
            printf("%d",tab[i][j]);
        }
        printf("\n");
    }
}

void affiche_mur(int x,int y,int tab[][N],SDL_Renderer **renderer)
{
    /* Affiche un élément du décor en 2D sous la forme d'un carré */
    SDL_Rect rect = {DX + x * CASE_SIZE,DY +y * CASE_SIZE,CASE_SIZE,CASE_SIZE};
    if(tab[y][x]==1){
        SDL_SetRenderDrawColor(*renderer,0,0,0,255);
        SDL_RenderFillRect(*renderer,&rect);
    }
    if(tab[y][x]==0){
        SDL_SetRenderDrawColor(*renderer,0,0,0,255);
        SDL_RenderDrawRect(*renderer,&rect);
    }
}

void affiche_map(int tab[][N],SDL_Renderer **renderer)
{
    /*Affiche la map */

    //déclaration des variables
    int i;
    int j;

    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            affiche_mur(j,i,tab,renderer);
        }
    }
}


int rayon_unitaire(float alpha,int px,int py,SDL_Renderer **renderer,int tab[][N])
{
    /*trace le rayon et renvoie sa longueur en pixel */
    int d=0;
    int nx=px;
    int ny=py;
    int Nx=(nx -DY)/CASE_SIZE;
    int Ny=(ny -DY)/CASE_SIZE;
    while(Ny <N && Nx <N && tab[Ny][Nx]!=1){
        nx=px + d*cos(alpha);
        ny=py + d*sin(alpha);
        Nx=(nx -DY)/CASE_SIZE;
        Ny=(ny -DY)/CASE_SIZE;

        //On ajoute le point
        SDL_SetRenderDrawColor(*renderer, 0, 0, 255, 255); //en vert
        SDL_RenderDrawPoint(*renderer,nx,ny);
        d++;
    }
    return d;
}

void Init_tab(int D[])
{
    /*Initialise un tableau avec des 0 */
    int i;
    for(i=0;i<T;i++){
        D[i]=0;
    }
}

void multi_rayon(int D[],int map[][N],int px,int py,float rayon_centre,SDL_Renderer **renderer)
{
    /*Affiche T rayon et met les distances dans D*/
    float alpha= rayon_centre - PI/2 + ANGLE_FOCAL;
    int i=0;
    float fish=0;
    while(i<T){
        D[i]=rayon_unitaire(alpha,px,py,renderer,map);
        fish=alpha - rayon_centre;
        if(fish<0){
            fish+=2*PI;
        }
        if(fish>2*PI){
            fish -=2*PI;
        }
        D[i] *= cos(fish);
        D[i]++;
        i++;
        alpha += VARIATION_ANGLE;
    }
}

void Dessine_cadre(SDL_Renderer **renderer)
{
    /*Dessine le cadre dans lequel l'affichafe 3d doit se faire */
    SDL_Rect rect ={XX , 0,(LARGEUR+1)*T,(N+1)*CASE_SIZE};

    //On déssine le rectangle
    SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255); //en vert
    SDL_RenderDrawRect(*renderer,&rect);
    
}
void draw3D_rect(SDL_Renderer **renderer,int hauteur,int largeur,int x,int ra)
{
    /*Dessine un des rectangles de l'affichagce 3D*/

    SDL_Rect rect ={XX + x,CENTRE - hauteur/2,largeur+1,hauteur};

    if((ra>0 && ra<PI) || (ra<0 && ra>-PI)){
        //On dessine le rectangle
        SDL_SetRenderDrawColor(*renderer, 100, 149, 193, 255); //en bleu
        SDL_RenderFillRect(*renderer,&rect);
    }
    else{
        //On dessine le rectangle
        SDL_SetRenderDrawColor(*renderer, 100, 149, 200, 255); //en bleu
        SDL_RenderFillRect(*renderer,&rect);    
    }
}

void draw3D(SDL_Renderer **renderer, int D[],float angle)
{
    /*Effectue l'affichage en 3D */
    //déclaration des variables
    int i;
    int hauteur=0;
    int x=0;
    int ra=0;
    // float fish=0;
    for(i=0;i<T;i++){
        // fish=angle - (i*VARIATION_ANGLE +ANGLE_FOCAL);
        hauteur = (int) (CASE_SIZE*HM)/D[i];
        if(hauteur>HM)
            hauteur=HM;
        x=i*LARGEUR;
        ra= PI - (abs(angle) - PI/2 + ANGLE_FOCAL + i*VARIATION_ANGLE + PI/2);
        draw3D_rect(renderer,hauteur,LARGEUR,x-1,ra);
    }
}

void background(SDL_Renderer **renderer)
{
    /*Affiche le ciel et le le sol de couleurs différentes */
    //déclaration des variables
    SDL_Rect rect1={XX,0,(LARGEUR+1)*T,CENTRE};
    SDL_Rect rect2={XX,CENTRE,(LARGEUR+1)*T,CENTRE};

    //On affiche le ciel
    SDL_SetRenderDrawColor(*renderer, 135, 206, 235, 255); //en bleu
    SDL_RenderFillRect(*renderer,&rect1);
    
    //On affiche le sol
    SDL_SetRenderDrawColor(*renderer, 128, 128, 128, 255); //en bleu
    SDL_RenderFillRect(*renderer,&rect2);

}

void Detruire_fenetre(SDL_Window **window,SDL_Renderer **renderer)
{
    /*Detruit la fenetre et le rendu*/
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
}


int main(int argc,char **argv)
{
    //Variable de la SDL
    SDL_Window *window=NULL;
    SDL_Renderer *renderer=NULL;

    //Initialisation des variables
    int px = 100;
    int py = 100;
    float angle= PI/2;
    int continuer=0;
    int D[T];

    //création de la fenêtre
    creer_fenetre(&window, &renderer);
    affiche_map(map,&renderer);
    Dessine_joueur(px,py,&renderer);
    Dessine_cadre(&renderer);
    SDL_RenderPresent(renderer);
    
    while(continuer !=-1){
        continuer = move_player(&px,&py,&angle,map);
        fond_blanc(&renderer);
	    affiche_map(map,&renderer);
        Dessine_cadre(&renderer);
        Dessine_joueur(px,py,&renderer);
        multi_rayon(D,map,px,py,angle,&renderer);
        background(&renderer);
        draw3D(&renderer,D,angle);
        SDL_RenderPresent(renderer);
    }

    //Destruction de la fenêtre et du rendu
    Detruire_fenetre(&window,&renderer);

    //fermeture de la SDL
    SDL_Quit();

    return 0;
}