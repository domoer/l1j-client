#include <SDL.h>

#include "globals.h"
#include "sdl_graphic.h"

sdl_graphic::sdl_graphic(int x, int y, int w, int h)
{
	img = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16,
		0x7C00, 0x03E0, 0x001F, 0);
	cleanup = false;
	pos = make_sdl_rect(x, y, w, h);
	mask = make_sdl_rect(0, 0, w, h);
	SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 255,255,255) );
	SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 255,255,255));
}

sdl_graphic::sdl_graphic(int x, int y, short *source, int type)
{
	switch(type)
	{
		case GRAPH_STIL:
		{
		//	printf("TranspTile %d\n", which);
			unsigned char *data = (unsigned char*)source;
			int data_offset = 0;
			int x, y;
			int width, height;
			x = data[data_offset++];
			y = data[data_offset++];
			width = data[data_offset++];
			height = data[data_offset++];
		//	printf("\tx: %d y:%d w:%d h:%d\n", x, y, width, height);
			img = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16,
				0x7C00, 0x03E0, 0x001F, 0);
			cleanup = false;
			pos = make_sdl_rect(x, y, width, height);
			mask = make_sdl_rect(0, 0, width, height);
			SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 255,255,255) );
			SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 255,255,255));
			short *temp = (short*)img->pixels;	//destination pointer
			int source_off = 0;
		//	printf("There are %d rows\n", height);
			for (int i = 0; i < height; i++)
			{
				int row_segs = data[data_offset++];
				short *row_temp = temp;
		//		printf("\tThere are %d segments in this row\n", row_segs);
				for (int j = 0; j < row_segs; j++)
				{
					int skip = data[data_offset++] / 2;
					int seg_width = data[data_offset++];
					row_temp = &row_temp[skip];
		//			printf("\t\tSkip %d, print %d\n", skip, seg_width);
					memcpy(row_temp, &data[data_offset], seg_width * 2);
					data_offset += (seg_width * 2);
					row_temp = &row_temp[seg_width];
				}
				temp = &temp[width];
			}

			break;
		}
		case GRAPH_LTIL:
		{
			img = SDL_CreateRGBSurface(SDL_SWSURFACE, 24, 24, 16,
				0x7C00, 0x03E0, 0x001F, 0);
			cleanup = false;
			pos = make_sdl_rect(0, 0, 24, 24);
			mask = make_sdl_rect(0, 0, 24, 24);
			SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 1,1,1) );
			SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 1,1,1));
			short *temp = (short*)img->pixels;	//destination pointer
			int source_off = 0;
			for (int i = 0; i < 24; i++)
			{
				int row_width;
				row_width = 2 * (i+1);
				if (i > 11)
				{
					row_width -= (4 * (i - 11));
				}
				memcpy(&temp[24 - row_width], source, row_width * 2);
				source = &source[24];
				temp = &temp[24];
			}
			break;
		}
		case GRAPH_RTIL:
		{
			img = SDL_CreateRGBSurface(SDL_SWSURFACE, 24, 24, 16,
				0x7C00, 0x03E0, 0x001F, 0);
			cleanup = false;
			pos = make_sdl_rect(10, 10, 24, 24);
			mask = make_sdl_rect(0, 0, 24, 24);
			SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 1,1,1) );
			SDL_FillRect(img, NULL, SDL_MapRGB(img->format, 1,1,1));
			short *temp = (short*)img->pixels;	//destination pointer
			int source_off = 0;
			for (int i = 0; i < 24; i++)
			{
				int row_width;
				row_width = 2 * (i+1);
				if (i > 11)
				{
					row_width -= (4 * (i - 11));
				}
				memcpy(temp, source, row_width * 2);
				source = &source[24];
				temp = &temp[24];
			}
			break;
		}
		default:
			break;
	}
}

sdl_graphic::sdl_graphic(int num, int x, int y, graphics_data *packs, int type)
{
	if (num == -1)
	{
		pos = 0;
		mask = 0;
		cleanup = false;
	}
	else
	{
		switch (type)
		{
			case GRAPH_IMG:
				img = get_img(num, packs->spritepack);
				if (img != 0)
				{
					pos = make_sdl_rect(x, y, img->w, img->h);
					mask = make_sdl_rect(0, 0, img->w, img->h);
					cleanup = true;
				}
				break;
			case GRAPH_PNG:
				img = get_png_image(num, packs->spritepack);
				if (img != 0)
				{
					SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 0));
					pos = make_sdl_rect(x, y, img->w, img->h);
					mask = make_sdl_rect(0, 0, img->w, img->h);
					cleanup = false;
				}
				break;
			default:
				break;
		}
		if (img == 0)
		{
			pos = 0;
			mask = 0;
			cleanup = false;
		}
	}
}

sdl_graphic::~sdl_graphic()
{
	if (pos != 0)
		delete pos;
	if (mask != 0)
		delete mask;
	if (cleanup && (img != 0) )
	{
		SDL_FreeSurface(img);
	}
}

void sdl_graphic::draw(SDL_Surface *display)
{
	if (img != 0)
	{
		SDL_BlitSurface(img, mask, display, pos);
	}
}

int sdl_graphic::getx()
{
	if (pos != 0)
	{
		return pos->x;
	}
	else
	{
		return 0;
	}
}

int sdl_graphic::gety()
{
	if (pos != 0)
	{
		return pos->y;
	}
	else
	{
		return 0;
	}
}

int sdl_graphic::getw()
{
	if (pos != 0)
	{
		return pos->w;
	}
	else
	{
		return 0;
	}
}

int sdl_graphic::geth()
{
	if (pos != 0)
	{
		return pos->h;
	}
	else
	{
		return 0;
	}
}

SDL_Surface *sdl_graphic::get_surf()
{
	return img;
}

Uint32 sdl_graphic::color(Uint8 r, Uint8 g, Uint8 b)
{
	SDL_MapRGB(img->format, r, g, b);
}

void sdl_graphic::mod_with(SDL_Surface *display)
{
	if (img != 0)
	{
		SDL_BlitSurface(display, mask, img, pos);
	}	
}