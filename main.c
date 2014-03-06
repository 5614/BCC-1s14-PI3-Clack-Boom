#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include "libs/libs.h"
#include "camera.h"

#define FPS 100
// #define LARGURA 800
// #define ALTURA 600

int main(){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_FONT *fonte1 = NULL;
	ALLEGRO_BITMAP *background = NULL;

//Iniciando

	camera *cam = camera_inicializa(0);
	if(!cam)
		erro("Erro ao iniciar a camera.");

	int LARGURA = cam->largura;
	int ALTURA = cam->altura;

	if(!al_init())
		erro("Falha ao iniciar o Allegro.");

	if(!al_init_image_addon())
		erro("Falha ao iniciar a biblioteca de imagens do Allegro.");

	if(!al_init_primitives_addon())
		erro("Falha ao carregar primitive_addons.");
	
	al_init_font_addon();

	if(!al_init_ttf_addon())
		erro("Falha ao iniciar a biblioteca de ttf do Allegro.");

	display = al_create_display(2*LARGURA, ALTURA);
	if(!display)
		erro("Falha ao criar display.");

	fonte1 = al_load_font("fontes/comic.ttf", 50, 0);
	if(!fonte1)
		erro("Falha ao carregar a fonte1.");

	background = al_create_bitmap(LARGURA, ALTURA);
	if(!background)
		erro("Falha ao criar bitmap brackground.");

	ALLEGRO_TIMER *temporizador = al_create_timer(1.0/FPS);
	if(!temporizador)
		erro("Falha ao criar temporizador.");

	ALLEGRO_EVENT_QUEUE *EventoQueue = al_create_event_queue();
	if(!EventoQueue)
		erro("Falha ao criar evento QUEUE");


	ArquivoLog("Sucesso ao carregar e criar fonte, imagens e eventos.");

	al_register_event_source(EventoQueue, al_get_timer_event_source(temporizador));
	al_register_event_source(EventoQueue, al_get_display_event_source(display));

	ArquivoLog("Registro de eventos!");

	al_start_timer(temporizador);

	ArquivoLog("Passou pelo start_timer");

	unsigned char ***matriz = camera_aloca_matriz(cam);

	ALLEGRO_BITMAP *buffer = al_get_backbuffer(display);
	ALLEGRO_BITMAP *esquerda = al_create_sub_bitmap(buffer, 0, 0, LARGURA, ALTURA);
	ALLEGRO_BITMAP *direita = al_create_sub_bitmap(buffer, LARGURA, 0, LARGURA, ALTURA);
	ArquivoLog("Sucesso ao criar buffers bitmap!");
	int laco = 0;
	int marca_rastreada = 0;

	while(1){
		ALLEGRO_EVENT evento;

		al_wait_for_event(EventoQueue, &evento);
		switch(evento.type){
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
			  laco = 1;
			ArquivoLog("Finalizado pelo usuario!");
			break;

			case ALLEGRO_EVENT_TIMER:
			marca_rastreada = 1;
			break;
			
			default:
			ArquivoLog("Evento desconhecido!");
		}

		if(laco == 1)
			break;
		
		if(marca_rastreada && al_is_event_queue_empty(EventoQueue)) {
			marca_rastreada = 0;
			camera_atualiza(cam);

			float marca_x = 0;
			float marca_y = 0;
			
			int cn = 0;
			int y = 0;
			int x = 0;

			for(y = 0; y < ALTURA; y++){
				for(x = 0; x < LARGURA; x++){
					int r = cam->quadro[y][x][0];
					int g = cam->quadro[y][x][1];
					int b = cam->quadro[y][x][2];

					if(r > g + b){
						marca_y += y;
						marca_x += x;
						cn++;

						matriz[y][x][0] = 255;
						matriz[y][x][1] = 255;
						matriz[y][x][2] = 255;
					}

					else{
						matriz[y][x][0] = 0;
						matriz[y][x][1] = 0;
						matriz[y][x][2] = 0;
					}

				}
			}
			camera_copia(cam, cam->quadro, esquerda);

			ALLEGRO_COLOR cor = al_map_rgb_f(1, 0, 1);
			
			if(cn > 0){
				al_draw_circle(marca_x / cn, marca_y / cn, 100, cor, 1);
			}
			
			camera_copia(cam, matriz, direita);
			
			al_flip_display();
		}
	}
	al_destroy_bitmap(direita);
	al_destroy_bitmap(esquerda);

	camera_libera_matriz(cam, matriz);	
	
	al_unregister_event_source(EventoQueue, al_get_timer_event_source(temporizador));
	al_unregister_event_source(EventoQueue, al_get_display_event_source(display));

	al_stop_timer(temporizador);
	
	al_destroy_event_queue(EventoQueue);
	al_destroy_display(display);
	al_destroy_timer(temporizador);

	al_shutdown_primitives_addon();
	al_shutdown_image_addon();
	al_uninstall_system();


	camera_finaliza(cam);
	ArquivoLog("Jogo finalizado com sucesso!");

	return 0;
}