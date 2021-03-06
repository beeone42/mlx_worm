#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#ifdef OSX
# include <OpenGL/gl.h>
#endif
#include <mlx.h>

#define CLICK_TO_START 0

#ifndef DestroyNotify
# define DestroyNotify 17
#endif

#ifndef StructureNotifyMask
# define StructureNotifyMask (1L<<17)
#endif

#define WIN_X	     1200
#define	WIN_Y	     800
#define WIN_TITLE    "tunnel"

#define SHAPE_SIZE   120
#define SHAPE_THICK  30
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
static int      first = 1;

int	init()
{
  int	bpp;
  int	sl;
  int	endian;

  if ((mlx = mlx_init()) == NULL)
    return (EXIT_FAILURE);
  if ((win = mlx_new_window(mlx, WIN_X, WIN_Y, WIN_TITLE)) == NULL)
    return (EXIT_FAILURE);

  if (!(img = mlx_new_image(mlx, WIN_X, WIN_Y)))
    return (EXIT_FAILURE);
  data = mlx_get_data_addr(img, &bpp, &sl, &endian);
  return (0);
}

int	close_win(void *p)
{
  printf("Goodbye\n");
  exit(0);
  return (0);
}

int	mouse_win(int button, int x, int y, void *p)
{
  first = 0;
  printf("Mouse in Win, button %d at %dx%d.\n", button, x, y);
  return (0);
}

int	key_win(int key, void *p)
{
  printf("Key in Win : %d\n",key);
  if (key==0xFF1B) // linux
    exit(0);
  if (key==53) // MacOsX
    exit(0);
  return (0);
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

void trace_circle(int x, int y, int s, int c)
{
  for (double a = 0; a < 360; a++)
    {
      buf_pixel_put(x + ((s/2) * cos(a*M_PI/180)),
		    y + ((s/2) * sin(a*M_PI/180)),
		    (c % 256));
    }
}

void trace_circle_epais(int x, int y, int s, int e, int c)
{
  for (int i = 0; i < e; i++)
    {
      trace_circle(x, y, s - i, c);
    }
}

int loop(void *param)
{
  static int	i = 1;
  static double	a = 0;
  int		x;
  int		y;

  if (CLICK_TO_START && (first == 1))
    return (0);

  if (i >= 255)
    i = 1;

  // random
  //  x = round((sin(a*M_PI/200) * cos((a + 90)*M_PI/130)) * WIN_SIZE_X) + (WIN_CENTER_X);
  //  y = round((sin(a*M_PI/180) * cos((a + 45)*M_PI/250)) * WIN_SIZE_Y) + (WIN_CENTER_Y);

  // rolling
  //  x = round((sin(a*M_PI/200) * cos((a + 90)*M_PI/130)) * WIN_SIZE_X) + (WIN_CENTER_X);
  //  y = round((sin(a*M_PI/180) * cos((a + 45)*M_PI/150)) * WIN_SIZE_Y) + (WIN_CENTER_Y);
  
  // fill
  //  x = round((sin(a*M_PI/130) * cos((a + 90)*M_PI/200)) * WIN_SIZE_X) + (WIN_CENTER_X);
  //  y = round((sin(a*M_PI/180) * cos((a + 45)*M_PI/210)) * WIN_SIZE_Y) + (WIN_CENTER_Y);

  // up n down
  x = round((sin(a*M_PI/130) * cos((a + 90)*M_PI/200)) * WIN_SIZE_X) + (WIN_CENTER_X);
  y = round((sin(a*M_PI/180) * cos((a + 45)*M_PI/84)) * WIN_SIZE_Y) + (WIN_CENTER_Y);
  
  trace_circle_epais(x, y, SHAPE_SIZE, SHAPE_THICK, i);
  if (i % 3 == 0)  // blit only every 3 frames
    blit(255 - i); // roll palette in sync
  i++;
  a = a + 0.3;
  return (0);
}

int main(void)
{
  int	i;
  int	res;
  
  res = init();
  if (res != 0)
    return (res);
  
  init_pal();
  
  mlx_mouse_hook(win, mouse_win, 0);
  mlx_key_hook(win, key_win, 0);
  mlx_hook(win, DestroyNotify, StructureNotifyMask, close_win, 0);
  mlx_loop_hook(mlx, loop, NULL);
  mlx_loop(mlx);

  return (EXIT_SUCCESS);
}
