#ifndef _SDL_USER_H_
#define _SDL_USER_H_

#include <SDL/SDL.h>

#include "config.h"
#include "globals.h"
#include "drawmode/sdl_drawmode.h"
#include "resources/files.h"
#include "resources/music.h"
#include "resources/small_font.h"
#include "resources/tile.h"
class pack;
class packet_data;
class prepared_graphics;
class sdl_widget;
struct client_request;

//#define true 1
//#define false 0
#define COLORKEY 255, 0, 255 //Your Transparent colour

void test_func(void * arg);
/**
This class is the gui portion of a game instance. This object belongs to sdl_master. This object has a thread that runs the client portion of the instance. I haven't quite figured out how to consistently determine whether data should be stored here or in the client class so there is bound to be some inconsistencies in the placement depending on how I felt that day.
 */
class sdl_user
{
	public:
		sdl_user(Uint32 flags = SDL_SWSURFACE);
		~sdl_user();
		
		//for tracking mouse movements 
		void mouse_to(SDL_MouseMotionEvent *to);
		void mouse_from(SDL_MouseMotionEvent *from);
		void mouse_move(SDL_MouseMotionEvent *from, SDL_MouseMotionEvent *to);
		bool quit_request(); /**< Is it ok for me to quit? */
		bool mouse_leave();	/**< is it ok for the mouse to leave my drawing area? */
				
		void mouse_click(SDL_MouseButtonEvent *here);
		void key_press(SDL_KeyboardEvent *button);
		void init_client(client *clnt);	/**< setup the client object as a part of startup, because sdl_master does not know about this object */
		
		void draw(SDL_Surface *display);	/**< draw */

		void quit_client();	/**< Quit the game instance by triggering the client thread through the client object */
		void login(const char *name, const char *pass); /**< login to the game server */
		void send_packet(packet_data &sendme);	/**< Sends a packet to the server */
		
		tile *get_tiles();	/**< Retrieve the list of tiles. */
		config *get_config();	/**< Retrieve the config from the client */

		void wait_for_mode(enum drawmode wait, bool mutex);	/**< wait for the client to be in the given drawmode */
		bool is_in_mode(enum drawmode here, bool mutex);	/**< is the client in the given drawmode */
		sdl_drawmode *get_drawmode(bool mutex);	/**< Retrieve the drawmode object */
		void done_with_drawmode();	/**< Unlocks the drawmode mutex */
		bool are_you_ready();	/**< wait until the ready flag is set */
		void change_drawmode(enum drawmode chg);	/**< change to a different drawmode */
		void check_for_change_drawmode();	/**< This actually performs the drawmode change */
		volatile bool done;	/**<used to terminate the client */
		music game_music; /**< the game music object */

		int add_request(client_request obj);
		void cancel_request(int id);
		int init_tiles();	/**< Initializes the map_tiles object */
		
		int check_login_chars();	/**< Returns the number of characters stored */
		lin_char_info** get_login_chars();	/**< Returns the array of characters for character select */

		client *get_client();	/**< This should only be called by the sdl_master class */
		small_font smallfont;	/**< A font object */
	private:
		client *game;	/**< the client object we are attached to */
		volatile bool ready;	/**< is it ok to draw me */
		volatile enum drawmode draw_mode;	/**< what is the current drawing mode */
		enum drawmode change_draw;

		sdl_drawmode *drawmode;	/**< the object that draws everything for me */
		tile *map_tiles;	/**< The array of all possible tiles a map can use, not all of them are loaded though */
		int number_map_tiles;	/**< The number of map tiles covered by the map_tiles object */

		SDL_mutex *draw_mtx;	/**< the mutex the gui thread (sdl_master) uses to share data with the client thread */
		SDL_mutex *mode_mtx;	/**< this mutex is used to protect access to the drawmode */
};

#endif
