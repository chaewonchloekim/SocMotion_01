#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include "amazon2_sdk.h"
#include "graphic_api.h"

#define AMAZON2_GRAPHIC_VERSION		"v0.5"
#define HEIGHT 180
#define WIDTH 120
#define NEAR 0
#define MIDDLE 1
#define FALL 2
#define GRAY 0x0000
#define Y_NUMOROUS_PIXELS 80
#define X_NUMOROUS_PIXELS 110

#define Y_SMALL_PIXELS 20
#define X_SMALL_PIXELS 30 

typedef struct _Edge{
	int x;
	int y;
} Edge;


void show_help(void)
{
	printf("================================================================\n");
	printf("Graphic API Example (Ver : %s)\n", AMAZON2_GRAPHIC_VERSION);
	printf("================================================================\n");
	printf("h : show this message\n");
	printf("0 : flip \n");
	printf("a : direct camera display on\n");
	printf("d : direct camera display off\n");
	printf("1 : clear screen \n");
	printf("2 : draw_rectfill(10, 200, 100, 100, MAKE_COLORREF(255, 0, 0)); \n");
	printf("3 : draw_rectfill(110, 200, 100, 100, MAKE_COLORREF(0, 255, 0)); \n");
	printf("4 : draw_rectfill(210, 200, 100, 100, MAKE_COLORREF(0, 0, 255)); \n");
	printf("5 : read fpga video data \n");
	printf("6 : draw fpga video data < Original > (180 x 120)\n");
	printf("7 : draw fpga buffer data \n");
	printf("8 : bmp(/root/img/AMAZON2.bmp) load \n");
	printf("9 : bmp(/root/img/AMAZON2.bmp) draw \n");
	printf("m : Demo \n");
	printf("q : exit \n");
	printf("x : exit \n");
	printf("z : exit \n");
	printf("================================================================\n");
}

int IppContourTracing(U16* fpga_videodata, int sx, int sy, Edge* edgeCord)
{
	
	// 외곽선 좌표를 저장할 구조체 초기화
	memset(edgeCord,0x00,sizeof(edgeCord))
	// 외곽선 추적 시작 픽셀이 객체가 아니면 종료
	if (fpga_videodata[sx*WIDTH+sy] != GRAY)
		return;
	int Num_edge_pixel =0;
	int x, y, nx, ny;
	int d, cnt = 0;
	int  dir[8][2] = { // 진행 방향을 나타내는 배열
		{  1,  0 },
		{  1,  1 },
		{  0,  1 },
		{ -1,  1 },
		{ -1,  0 },
		{ -1, -1 },
		{  0, -1 },
		{  1, -1 }
	};

	x = sx;
	y = sy;
	d = cnt = 0;

	while (1)
	{
		nx = x + dir[d][0];
		ny = y + dir[d][1];

		if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT || fpga_videodata[nx*WIDTH+ny] == 0xffff)
		{
			// 진행 방향에 있는 픽셀이 객체가 아닌 경우,
			// 시계 방향으로 진행 방향을 바꾸고 다시 시도한다.

			if (++d > 7) d = 0;
			cnt++;

			// 8방향 모두 배경인 경우 
			if (cnt >= 8)
			{
				edgeCord[Num_edge_pixel].x = x;
				edgeCord[Num_edge_pixel].y = y;
				Num_edge_pixel++;
				break;  // 외곽선 추적을 끝냄.
			}
		}
		else
		{
			// 진행 방향의 픽셀이 객체일 경우, 현재 점을 외곽선 정보에 저장
			
			edgeCord.x[Num_edge_pixel] = x;
			edgeCord.y[Num_edge_pixel] = y;
			fpga_videodata[nx*WIDTH+ny] = 0x
			Num_edge_pixel++;
			// 진행 방향으로 이동
			x = nx;
			y = ny;
			

			// 방향 정보 초기화
			cnt = 0;
			d = (d + 6) % 8;	// d = d - 2 와 같은 형태
		}

		// 시작점으로 돌아왔고, 진행 방향이 초기화된 경우
		// 외곽선 추적을 끝낸다.
		if (x == sx && y == sy && d == 0)
			break;
	}
	return Num_edge_pixel;
}


static void demo(void)
{
	int i = 400;
	U16* fpga_videodata = (U16*)malloc(180 * 120 * 2);
	Edge* edgeCord = (Edge*)malloc(180*2+120*2);
	int x = 0;
	int y = 0;
	int x_index,y_index=0;
	int Num_edge_pixel=0;
	printf("Demo Start\n");
	while (i--)
	{
		clear_screen();
		x = rand() % 300;
		y = rand() % 460;
		draw_rectfill(x, y, 20, 20, MAKE_COLORREF(255, 0, 0));
		x = rand() % 300;
		y = rand() % 460;
		draw_rectfill(x, y, 20, 20, MAKE_COLORREF(0, 255, 0));
		read_fpga_video_data(fpga_videodata);
		for (x_index=0;x_index < WIDTH; x_index++)
		{
			for(y_index=0;y_index<HEIGHT;y_index++)
			{
				Num_edge_pixel = IppContourTracing(fpga_videodata,x_index,y_index,Edge* edgeCord)
				if (Num_edge_pixel > 40)
					break;
				
			}
			break;
		}
		

		
		draw_fpga_video_data(fpga_videodata, 10, 10);
		flip();
		printf("%d\n", i);
	}
	free(fpga_videodata);
	printf("Demo End\n");
}

int main(int argc, char **argv)
{
	printf("Ver.Choi 1.0\n");
	show_help();
	BOOL b_loop = TRUE;
	if (open_graphic() < 0) {
		return -1;
	}

	SURFACE* bmpsurf = 0;
	U16* fpga_videodata = (U16*)malloc(180 * 120 * 2);

	while (b_loop)
	{
		int ch = getchar();
		switch (ch)
		{
		case 'q':
		case 'Q':
		case 'x':
		case 'X':
		case 'z':
		case 'Z':
			b_loop = FALSE;
			break;
		case 'a':
		case 'A':
			printf("direct camera display on\n");
			direct_camera_display_on();
			break;
		case 'd':
		case 'D':
			printf("direct camera display off\n");
			direct_camera_display_off();
			break;
		case '0':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			printf("flip\n");
			flip();
			break;
		case '1':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			printf("clear_screen\n");
			clear_screen();
			break;
		case '2':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			printf("draw_rectfill(10, 200, 100, 100, MAKE_COLORREF(255, 0, 0));\n");
			draw_rectfill(10, 200, 100, 100, MAKE_COLORREF(255, 0, 0));
			break;
		case '3':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			printf("draw_rectfill(110, 200, 100, 100, MAKE_COLORREF(0, 255, 0));\n");
			draw_rectfill(110, 200, 100, 100, MAKE_COLORREF(0, 255, 0));
			break;
		case '4':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			printf("draw_rectfill(210, 200, 100, 100, MAKE_COLORREF(0, 0, 255));\n");
			draw_rectfill(210, 200, 100, 100, MAKE_COLORREF(0, 0, 255));
			break;
		case '5':
			printf("read fpga video data\n");
			read_fpga_video_data(fpga_videodata);
			break;
		case '6':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			printf("draw fpga video data\n");
			draw_fpga_video_data(fpga_videodata,10,10);
			break;
		case '7':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			printf("draw fpga video\n");
			/*
			printf("Full < Expension(x2.66), Rotate(90) > (320 x 480)\n");
			draw_img_from_buffer(fpga_videodata, 320, 0, 0, 0, 2.67, 90);
			*/

			printf(" Double < Expension(x1.77), Rotate(0) > (320 x 480) - Default\n");
			draw_img_from_buffer(fpga_videodata, 0, 18, 0, 0, 1.77, 0);
			draw_img_from_buffer(fpga_videodata, 0, 250, 0, 0, 1.77, 0);
			break;
		case '8':
			if (bmpsurf != 0)
			{
				printf("image wad already loaded\n ");

			}
			else
			{
				if(direct_camera_display_stat() > 0) {
					printf("direct camera display on\n");
					printf("please direct camera diplay off\n");
					break;
				}
				printf("bmp(/root/img/AMAZON2.bmp) load\n");
				bmpsurf = loadbmp("/root/img/AMAZON2.bmp");
			}
			break;
		case '9':
			if (bmpsurf == 0)
			{
				printf("bmp is not loaded yet\n");
			}
			else
			{
				if(direct_camera_display_stat() > 0) {
					printf("direct camera display on\n");
					printf("please direct camera diplay off\n");
					break;
				}
				printf("bmp(/root/img/AMAZON2.bmp) draw\n");
				draw_surface(bmpsurf, 10, 300);
			}
			break;
		case 'm':
		case 'M':
			if(direct_camera_display_stat() > 0) {
				printf("direct camera display on\n");
				printf("please direct camera diplay off\n");
				break;
			}
			demo();
			break;
		case 'h':
		case 'H':
			show_help();
		default:
			break;
		}
	}
	free(fpga_videodata);
	if (bmpsurf != 0)
		release_surface(bmpsurf);
	close_graphic();
	return 0;
}

