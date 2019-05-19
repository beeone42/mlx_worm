#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mlx.h>

#define WIN_X	640
#define	WIN_Y	480
#define WIN_TITLE "tunnel"

typedef struct    col_s
{
  unsigned char	  r;
  unsigned char	  g;
  unsigned char	  b;
}                 col_t;

void		*mlx;
void		*win;
unsigned char	buf[WIN_X * WIN_Y];
col_t		pal[256];



int	init()
{
  if ((mlx = mlx_init()) == NULL)
    return (EXIT_FAILURE);
  if ((win = mlx_new_window(mlx, WIN_X, WIN_Y, WIN_TITLE)) == NULL)
    return (EXIT_FAILURE);
  return (0);
}

int	key_win(int key, void *p)
{
  printf("Key in Win : %d\n",key);
  if (key==0xFF1B)
    exit(0);
}

void print_pal(int i)
{
  printf("%d: %d %d %d\n", i, pal[i].r, pal[i].g, pal[i].b);
}

unsigned int	palette(int i)
{
  return (pal[i].r * 256 * 256 +
	  pal[i].g * 256 +
	  pal[i].b);
}

void trace_carre(int x, int y, int s, int c)
{
  for (int i = 0; i < s; i++)
    {
      // haut
      mlx_pixel_put(mlx, win, x + i, y, palette(c % 256));
      // gauche
      mlx_pixel_put(mlx, win, x, y + i, palette(c % 256));
      // bas
      mlx_pixel_put(mlx, win, x + i, y + s, palette(c % 256));
      // droit
      mlx_pixel_put(mlx, win, x + s, y + i, palette(c % 256));
    }
}

int main(void)
{
  int		i;
  int		res;
  
  res = init();
  if (res != 0)
    return (res);
  
  for (i = 0; i < 32; i++)
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
  
  for (i = 0; i < 256; i++)
    {
      trace_carre((random() % (WIN_X - 50)),
		  (random() % (WIN_Y - 50)),
		  50, i);
      usleep(100000);
    }
  mlx_key_hook(win, key_win, 0);
  mlx_loop(mlx);


  return (EXIT_SUCCESS);
}
