//Conway's game of Life in Color for the Cromenco Dazzler!!  
//Charles DeCraene, 4 Oct 2015
#include <SDL/SDL.h> 
#include <stdio.h> 
#include <stdlib.h>  
#define CellSize 10  // In pixels 
#define XL 64 
#define YL 64 
SDL_Surface * screen ; 
Uint16 * raw_pixels;
char OldMap [XL + 1][ YL + 1], NewMap [XL + 1][ YL + 1] ; 

Uint16 
CreateHiColorPixel (SDL_PixelFormat * fmt, Uint8 red, Uint8 green, Uint8 blue) {
	Uint16 value;
	value = ((red>>fmt->Rloss)<<fmt->Rshift) |
		((green>>fmt->Gloss)<<fmt->Gshift) |
		((blue >>fmt->Bloss)<<fmt->Bshift) ;
	return value;
}
 
void 
LightCell(int x, int y, Uint16 Color) {
	int i, j, k, l ;
	if(x > XL || y > YL || x < 0 || y < 0) return ;
	for (j = 0; j < CellSize; ++j ) {
		for (i = 0; i < CellSize; ++i) { 
			k = x*CellSize + i;
			l = y*CellSize + j;
			int offset = screen->pitch / 2 * l + k ;
			raw_pixels [ offset ] = Color; 
		}
	}
}

int
Neighbors(int x, int y) {
	int Count=0, i, j, k, l  ;
	for (k=-1; k<2; ++k) {
		i = x + k;
		if (i>=0 && i < XL) {
			for (l = -1; l < 2; ++l) {
				j = y + l;
				if (j >=0 && j < YL && OldMap[i][j]) ++Count;
			}
		}
	}
	if(OldMap[x][y]) --Count;
	return Count;
} 
Uint16  Red, Yellow, Green, Blue, White, Black ;

SDL_Event event;
SDL_keysym keysym; 
int x=0, y=0 ;  

void
RandomPopulate(int P) {
int i,j;
	printf("Randomly populating with probability 1/%d \n",P);
	for(i=0;i<XL;i++) {
		for(j=0;j<YL;++j) {
			if(!(rand()%P)) {
				OldMap[i][j]=0xFF;
				LightCell(i,j,White);
			}
		}	
	}
}
void
Preset(int xc, int yc) {	
	if (event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) { 
		int t=xc; xc=yc; yc=t; //Swap x and y local coordinates 
	} 
	if(x+xc < XL && x+xc>=0 && y+yc < YL && y+yc >=0){
		OldMap[x+xc][y+yc] = 0xFF;
		LightCell(x+xc,y+yc,White);
	}
}
 
int 
main() {

	int  i, j, k, l,  T, Generation = 0 ,Population, EditnRun=0   ; 
	int Born, Died ;
	float Temp; 
	for (j = 0; j<XL; ++j) {
		for (i = 0; i< YL; ++i) {
			OldMap[j][i] = 0  ;
		}
	} 
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Unable to initialize SDL: %s \n", SDL_GetError()) ;
		return 1;
	}
	atexit(SDL_Quit);
	screen = SDL_SetVideoMode(XL*CellSize ,YL*CellSize,16,0); 
	if (screen == NULL) {
		printf("Unable to set video mode: %s \n", SDL_GetError()) ; 
		return 1;
	}
	Red = CreateHiColorPixel(screen->format, 255, 0 , 0); 
	Yellow = CreateHiColorPixel(screen->format, 255, 255, 0);
	Green  = CreateHiColorPixel(screen->format, 0, 255, 0) ; 
	Blue   = CreateHiColorPixel(screen->format, 0, 0, 255) ;
	White  = CreateHiColorPixel(screen->format, 255,255,255);
	Black  = CreateHiColorPixel(screen->format, 0 , 0 , 0) ;

	SDL_LockSurface(screen); 
	raw_pixels = (Uint16 *) screen->pixels; 

	for (i=0; i<XL; ++i) {
		for(j=0; j<YL; ++j) if (OldMap[i][j]) LightCell(i,j,White);
	} 
	SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen,0,0,0,0); 
	SDL_Delay(1000);
	int Quit_Flag = 0;
	while( Quit_Flag == 0) {

		Population = 0;
		if (SDL_PollEvent(&event) ){
			switch(event.type) { 
			case SDL_QUIT: 
				Quit_Flag = 0xff;
				break;
			case SDL_KEYDOWN:
				keysym = event.key.keysym ;
				if (keysym.sym == SDLK_q) 
					Quit_Flag= 0xff;
				if (keysym.sym == SDLK_e) EditnRun = 0xff; 
				break;
			}
		}

		//Edit Mode 

		while (!Quit_Flag && EditnRun) { 
		SDL_EnableKeyRepeat(333,150); 
		SDL_WaitEvent(&event);  
		SDL_LockSurface(screen) ;
		raw_pixels = (Uint16 *) screen->pixels; 
		if(event.type == SDL_QUIT) Quit_Flag =0xff;
		if(event.type == SDL_KEYDOWN) {
		keysym =event.key.keysym; 

		switch (keysym.sym) {
		case (SDLK_UP): 
			if(y>0) {
				if(OldMap[x][y])LightCell(x,y,White);
				else LightCell(x,y,Black) ;  
				--y; 
				LightCell(x,y,Yellow);
				}
			break; 
		case (SDLK_DOWN): 
			if(y<YL) {
				if(OldMap[x][y])LightCell(x,y,White);
				else LightCell(x,y,Black);
				++y;
				LightCell(x,y,Yellow);
				}
			break;
		case (SDLK_LEFT):
			if (x>0) {
				if(OldMap[x][y])LightCell(x,y,White);
				else LightCell(x,y,Black);
				--x;
				LightCell(x,y,Yellow);
				}
			break;
		case (SDLK_RIGHT):
			if (x<XL) {
				if(OldMap[x][y])LightCell(x,y,White);
				else LightCell(x,y,Black);
				++x;
				LightCell(x,y,Yellow);
				}
			break;
		case (SDLK_INSERT): 
			OldMap[x][y]= 0xff;
			break;
		case (SDLK_DELETE):
			OldMap[x][y]= 0;
			break;
		case (SDLK_r):
			EditnRun = 0;
			break;
		case (SDLK_q):
			Quit_Flag = 0xff;
			break;
		case (SDLK_c): 
			for (i=0;i<XL;++i) {
				for(j=0;j<YL;++j) {
					OldMap[i][j] = 0;
					LightCell(i,j,Black);
				}
			}
			break;
		case (SDLK_1):
			RandomPopulate(11);
			break;
		case (SDLK_2): 
			RandomPopulate(12);
		case (SDLK_3): 
			RandomPopulate(13);
			break;
		case (SDLK_4):
			RandomPopulate(14);
			break;
		case (SDLK_5):
			RandomPopulate(5);
			break;
		case (SDLK_6):
			RandomPopulate(6);
			break;
		case (SDLK_7):
			RandomPopulate(7);
			break; 
		case (SDLK_8): 
			RandomPopulate(8);
			break;
		case SDLK_9:
			RandomPopulate(9);
			break;
		case SDLK_0:
			RandomPopulate(10);
			break; 
		case (SDLK_g):  //Glider
			printf("Glider at %d , %d \n",x,y);
			Preset(0,2);Preset(1,2);Preset(2,2);
			Preset(2,1); Preset(1,0);
			break;  	
		case SDLK_t: //Toad
			printf("Toad at %d , %d \n",x,y);  
			Preset(0,0);Preset(0,1);Preset(0,2);
			Preset(1,1);Preset(1,2);Preset(1,3);
			break; 
		case (SDLK_x): //Cross 
			printf("Cross at %d , %d\n",x,y);
			Preset(1,4);Preset(1,3);Preset(1,5);Preset(1,6);
			Preset(2,3);Preset(2,6);
			Preset(3,1);Preset(3,2);Preset(3,3);Preset(3,6);Preset(3,7);
			Preset(3,8);Preset(4,1);Preset(4,8);Preset(5,1);Preset(5,8);
			Preset(6,1);Preset(6,2);Preset(6,3);
			Preset(6,7);Preset(6,8);Preset(6,6);
			Preset(7,3);Preset(7,6);Preset(8,3);Preset(8,4);
			Preset(8,5);Preset(8,6); 
			break;
		case (SDLK_k): //Odd Keys
			printf("Odd Keys at %d , %d\n",x,y); 
			Preset(1,11);Preset(2,2);Preset(2,10);
			Preset(2,12);Preset(3,1);Preset(3,3);
			Preset(3,4);Preset(3,5);Preset(3,8);
			Preset(3,9);Preset(3,11);Preset(4,2);
			Preset(4,5);Preset(4,8);
			Preset(5,5);Preset(5,8);  
			break;
		case (SDLK_m): //Mold period 4 osc
			printf("Mold at %d , %d\n",x,y); 
			Preset(1,5);Preset(1,4);Preset(2,3);
			Preset(2,6);Preset(3,1);Preset(3,4);
			Preset(3,6);Preset(4,5);Preset(5,1);
			Preset(5,3);Preset(5,4);Preset(6,2);
			break;	
		case SDLK_l: //Light weight space ship!  
			printf("Lightweight Spaceship at %d, %d\n",x,y);
			Preset(2,1);Preset(5,1);Preset(1,2);
			Preset(1,3);Preset(5,3);Preset(1,4);
			Preset(2,4);Preset(3,4);Preset(4,4);
			break;
		case SDLK_h: //Help 
			printf("C - Clear grid, restore initial state\n");
			printf("G - Glider traverses diagonally \n");
			printf("H - This help page\n");
			printf("K - Odd Keys, oscillator\n");
			printf("L - Lightweight spaceship travels straight, turns into glider at edge of grid\n"); 
			printf("M - Mold, period 4 oscillator\n");
			printf("T - Toad, oscillator\n");
			printf("X - Cross, oscillator\n");
			printf("0-9 - randomly populate with probability given\n");
			break;	
		}
		}
		SDL_UnlockSurface(screen) ;
		SDL_UpdateRect(screen,0,0,0,0);
		SDL_Delay(100);	
		}	
		SDL_EnableKeyRepeat(0,150); //Disable key repeat	

		
//Run Mode
		Born = Died = 0;
		SDL_LockSurface(screen) ;
		raw_pixels = (Uint16 *) screen->pixels; 
		for (i=0; i < XL; ++i) {
			for(j=0; j < XL; ++j) {
				NewMap[i][j] = 0;
				T = Neighbors(i,j);
				if (T == 3) NewMap[i][j] = 0xff;
				if (T == 2 && OldMap[i][j] ) NewMap[i][j] = 0xff; 
				LightCell(i,j,Black);  

				if(OldMap[i][j]  && NewMap[i][j]) LightCell(i,j,Blue) ;//Survives to next generation
				if(OldMap[i][j]  &&!NewMap[i][j]) {
					LightCell(i,j,Red);// Cell dies 
					++Died ;
				}
				if(!OldMap[i][j] && NewMap[i][j]) {
					LightCell(i,j,Green); //Cell is born
					++Born ;
				}
				if(NewMap[i][j]) ++Population;
			}	
		} 
		SDL_UnlockSurface(screen) ;
		SDL_UpdateRect(screen,0,0,0,0);
		for (i=0; i<XL; ++i) {
			for(j=0; j<YL; ++j) OldMap[i][j] = NewMap[i][j] ;
		}
		printf ("%d Born, %d Died, Temp = %f. ",Born, Died, 
			(double) (Born+Died)/Population);
		printf ("Population %d.\n",Population);
		++Generation;
		SDL_Delay(800);
		//End of Run Mode
	} 
	printf("Quit after %d Generations!!\n",Generation) ;



}

