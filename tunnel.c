#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <mlx.h>
#include <mlx_int.h>

#define WIN_X   1200
#define	WIN_Y	 800
#define WIN_TITLE "tunnel"

#define SHAPE_SIZE   100
#define WIN_MARGIN   20
#define WIN_CENTER_X (WIN_X / 2)
#define WIN_CENTER_Y (WIN_Y / 2)
#define WIN_SIZE_X   ((WIN_X - SHAPE_SIZE) / 2 - WIN_MARGIN)
#define WIN_SIZE_Y   ((WIN_Y - SHAPE_SIZE) / 2 - WIN_MARGIN)

typedef struct    col_s
{
  unsigned char	  r;
  unsigned char	  g;
  unsigned char	  b;
}                 col_t;

void		*mlx;
void		*win;
void		*img;
char		*data;
unsigned char	buf[WIN_X * WIN_Y];
col_t		pal[256];
int		bpp;
int		sl;
int		endian;


int	init()
{
  if ((mlx = mlx_init()) == NULL)
    return (EXIT_FAILURE);
  if ((win = mlx_new_window(mlx, WIN_X, WIN_Y, WIN_TITLE)) == NULL)
    return (EXIT_FAILURE);

  if (!(img = mlx_new_image(mlx, WIN_X, WIN_Y)))
    return (EXIT_FAILURE);
  data = mlx_get_data_addr(img, &bpp, &sl, &endian);
  printf("OK (bpp1: %d, sizeline1: %d endian: %d type: %d)\n",
	 bpp, sl, endian, ((t_img *)img)->type);
  return (0);
}

int	key_win(int key, void *p)
{
  printf("Key in Win : %d\n",key);
  if (key==0xFF1B)
    exit(0);
}

void init_pal()
{
  for (int i = 0; i < 32; i++)
    {
      // 0 bleu  (0,0,255) -> violet (255,0,255) 31
      pal[i].r = i * 8;
      pal[i].g = 0;
      pal[i].b = 255;

      // 32 violet ->  rouge 63
      
      pal[i + 32].r = 255;
      pal[i + 32].g = 0;
      pal[i + 32].b = 255 - (i * 8);

      // 64 rouge (255,0,0) --> orange (255,128,0) 95
      
      pal[i + 64].r = 255;
      pal[i + 64].g = i * 4;
      pal[i + 64].b = 0;

      // 96 orange (255,128,0) --> jaune (255,255,0) 127
      
      pal[i + 96].r = 255;
      pal[i + 96].g = 128 + i * 4;
      pal[i + 96].b = 0;

      // 128 jaune (255,255,0) --> blanc (255,255,255) 159
      
      pal[i + 128].r = 255;
      pal[i + 128].g = 255;
      pal[i + 128].b = i * 8;

      // 160 blanc (255,255,255) --> vert (0,255,0) 191
      
      pal[i + 160].r = 255 - (i * 8);
      pal[i + 160].g = 255;
      pal[i + 160].b = 255 - (i * 8);

      // 192 vert (0,255,0) -->  bleu ciel (0,255,255) 223
      
      pal[i + 192].r = 0;
      pal[i + 192].g = 255;
      pal[i + 192].b = i * 8;

      // 224 bleu ciel (0,255,255) --> bleu (0,0,255) 255
      
      pal[i + 224].r = 0;
      pal[i + 224].g = 255 - (i * 8);
      pal[i + 224].b = 255;
    }

  // noir
  
  pal[0].r = 0;
  pal[0].g = 0;
  pal[0].b = 0;
}

void print_pal(int i)
{
  printf("%d: %d %d %d\n", i, pal[i].r, pal[i].g, pal[i].b);
}

void blit(int deca)
{
  int		a, i;
  unsigned char	c;

  for (i = 0; i < (WIN_X * WIN_Y); i++)
    {
      if (buf[i] == 0)
	c = 0;
      else
	{
	  c = (buf[i] + deca) % 256;
	  if (c == 0)
	    c = 1;
	}
      a = i << 2;
      data[a]   = pal[c].b;
      data[a+1] = pal[c].g;
      data[a+2] = pal[c].r;
    }
  mlx_put_image_to_window(mlx, win, img, 0, 0);
}

void buf_pixel_put(int x, int y, int c)
{
  buf[x + (y*WIN_X)] = c;
}

void trace_carre(int X, int Y, int s, int c)
{
  int x, y;

  x = X - s/2;
  y = Y - s/2;
  for (int i = 0; i < s; i++)
    {
      // haut
      buf_pixel_put(x + i, y, (c % 256));
      // gauche
      buf_pixel_put(x, y + i, (c % 256));
      // bas
      buf_pixel_put(x + i, y + s - 1, (c % 256));
      // droit
      buf_pixel_put(x + s - 1, y + i, (c % 256));
    }
}

void trace_carre_epais(int x, int y, int s, int e, int c)
{
  for (int i = 0; i < e; i++)
    {
      trace_carre(x, y, s - i, c);
    }
}

int loop(void *param)
{
  static int	i = 1;
  static double	a = 0;
  int		x;
  int		y;

  if (i >= 255)
    i = 1;

  x = round((sin(a*M_PI/200) * cos((a + 90)*M_PI/130)) * WIN_SIZE_X) + (WIN_CENTER_X);
  y = round((sin(a*M_PI/180) * cos((a + 45)*M_PI/250)) * WIN_SIZE_Y) + (WIN_CENTER_Y);
  
  trace_carre_epais(x, y, SHAPE_SIZE, 20, i);
  blit(255 - i);
  //usleep(10000);
  i++;
  a = a + 0.5;
}

int main(void)
{
  int		i;
  int		res;
  
  res = init();
  if (res != 0)
    return (res);
  
  init_pal();
  
  mlx_key_hook(win, key_win, 0);
  mlx_loop_hook(mlx, loop, NULL);
  mlx_loop(mlx);

  return (EXIT_SUCCESS);
}
