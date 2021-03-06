#include "draw_char_sel.h"

#include "globals.h"
#include "packet.h"
#include "resources/prepared_graphics.h"
#include "sdl_user.h"
#include "widgets/sdl_animate_button.h"
#include "widgets/sdl_char_info.h"
#include "widgets/sdl_input_box.h"
#include "widgets/sdl_widget.h"

dcs_ptr::dcs_ptr(draw_char_sel *bla, enum dcs_funcs sel)
{
	ref = bla;
	what = sel;
}

dcs_ptr::dcs_ptr(draw_char_sel *bla, enum dcs_funcs sel, int* val)
{
	ref = bla;
	what = sel;
	which = val;
}

void dcs_ptr::go()
{
	switch(what)
	{
		case DCS_FUNC_SELCHAR:
			ref->select_char();
			break;
		case DCS_FUNC_PREVPAGE:
			ref->prevpage();
			break;
		case DCS_FUNC_NEXTPAGE:
			ref->nextpage();
			break;
		case DCS_FUNC_DELCHAR:
			ref->delete_char(*which);
			break;
		default:
			break;
	}
}

draw_char_sel::draw_char_sel(sdl_user *self)
	: sdl_drawmode(self)
{	
	draw_mtx = SDL_CreateMutex();
	ready = false;
	cur_char_slot = -1;
	page_num = 0;
	
	//1c1
	//0
	
	num_gfx = 1;
	gfx = new sdl_graphic*[num_gfx];
	gfx[0] = new sdl_graphic();
	gfx[0]->delay_load(815, 0, 0, GRAPH_PNG, owner);
		/** \TODO Replace with 105.img */
		//or 1763.img
		//1764.img locked
		//1769-1772 - numeric indicators
	gfx[0]->disable_clear();
	
	num_widgets = 12;
	widgets = new sdl_widget*[num_widgets];
	
	//character select animating buttons
	widgets[0] = new sdl_animate_button(0xf4, 0x013, 0, owner, 0);
	widgets[1] = new sdl_animate_button(0xf4, 0x0b0, 0, owner, 0);
	widgets[2] = new sdl_animate_button(0xf4, 0x14d, 0, owner, 0);
	widgets[3] = new sdl_animate_button(0xf4, 0x1ea, 0, owner, 0);
	widgets[0]->set_key_focus(true);
	widgets[1]->set_key_focus(true);
	widgets[2]->set_key_focus(true);
	widgets[3]->set_key_focus(true);
	widgets[0]->cursor_on();
	widget_key_focus = 0;
	
	widgets[4] = new sdl_plain_button(0x6e5, 0x0f7, 0x10b, owner, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_PREVPAGE));	//left arrow
	widgets[5] = new sdl_plain_button(0x6e7, 0x16c, 0x10b, owner, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_NEXTPAGE));	//left arrow
	widgets[6] = new sdl_plain_button(0x334, 0x20d, 0x185, owner, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_SELCHAR));	//login
	widgets[7] = new sdl_plain_button(0x336, 0x20d, 0x19a, owner, 0);	//cancel
	widgets[8] = new sdl_plain_button(0x134, 0x20d, 0x1b5, owner, 
		(funcptr*)new dcs_ptr(this, DCS_FUNC_DELCHAR, &cur_char_slot));	//delete
	widgets[4]->set_key_focus(true);
	widgets[5]->set_key_focus(true);
	widgets[6]->set_key_focus(true);
	widgets[7]->set_key_focus(true);
	widgets[8]->set_key_focus(true);
	
	widgets[9] = new sdl_widget(0x6e9, 0x127, 0x10f, owner);
	widgets[10] = new sdl_widget(0x6eb, 0x146, 0x10f, owner);
	widgets[11] = new sdl_char_info(owner);

	if (owner->check_login_chars() != 0)
	{
		get_login_chars();
	}
}

draw_char_sel::~draw_char_sel()
{
	SDL_DestroyMutex(draw_mtx);
}

bool draw_char_sel::quit_request()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	draw_scene = false;
	SDL_mutexV(draw_mtx);
	return true;
}

void draw_char_sel::nextpage()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (page_num < 1)
	{
		page_num++;
		get_login_chars();
	}
	SDL_mutexV(draw_mtx);
}

void draw_char_sel::prevpage()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (page_num > 0 )
	{
		page_num--;
		get_login_chars();
	}
	SDL_mutexV(draw_mtx);
}

void draw_char_sel::delete_char(int which)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
//	lin_char_info **data = owner->get_login_chars();
	switch(which)
	{
		case -1:
			break;
		default:
			char_deleting = which;
			//TODO: implement timer for character deletion
//			owner->send_packet("csdd", CLIENT_DELETE_CHAR, 
//				data[which]->name, 0, 0);
			break;
	}
	SDL_mutexV(draw_mtx);
}

void draw_char_sel::do_delete()
{
//	lin_char_info **data = owner->get_login_chars();
//	sdl_animate_button **chars;
//	chars = (sdl_animate_button**)&widgets[0];
//	delete data[char_deleting];
//	data[char_deleting] = 0;
//	chars[char_deleting]->set_info(0);
//	cur_char_slot = -1;
//	chars[0]->animate(false);
//	chars[1]->animate(false);
//	chars[2]->animate(false);
//	chars[3]->animate(false);
}

void draw_char_sel::select_char()
{
	if (cur_char_slot != -1)
	{
		sdl_animate_button **chars;
		chars = (sdl_animate_button**)&widgets[0];
		lin_char_info *bob;
		if (chars[cur_char_slot]->char_info == 0)
		{
			owner->change_drawmode(DRAWMODE_NEWCHAR);
		}
		else
		{
			bob = chars[cur_char_slot]->get_info();
			packet_data sendme;
			sendme << (uint8_t)CLIENT_USE_CHAR << bob->name
					<< (uint32_t)0 << (uint32_t)0;
			owner->send_packet(sendme);
		}
	}
}

lin_char_info *draw_char_sel::get_selected_char()
{
	sdl_animate_button **chars;
	chars = (sdl_animate_button**)&widgets[0];
	lin_char_info *bob;
	if (cur_char_slot != -1)
	{
		if (chars[cur_char_slot]->char_info != 0)
		{
			bob = chars[cur_char_slot]->get_info();
		}
		else
		{
			bob = 0;
		}
	}
	else
	{
		bob = 0;
	}
	return bob;
}

void draw_char_sel::get_login_chars()
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	lin_char_info **data = owner->get_login_chars();
	sdl_animate_button **chars;
	chars = (sdl_animate_button**)&widgets[0];
	for (int i = 0; i < 4; i++)
	{
		chars[i]->set_info(data[i + (page_num*4)]);
	}
	ready = true;
	
	sdl_char_info *stuff;
	stuff = (sdl_char_info*)widgets[11];
	stuff->hand_info(chars[0]->get_info());
	cur_char_slot = 0;
	chars[0]->animate(true);
	chars[1]->animate(false);
	chars[2]->animate(false);
	chars[3]->animate(false);

	SDL_mutexV(draw_mtx);
}

bool draw_char_sel::mouse_leave()
{
	return false;
}

void draw_char_sel::mouse_click(SDL_MouseButtonEvent *here)
{
	if (num_widgets > 0)
	{
		int index = get_widget(here->x, here->y);
		sdl_animate_button *chars[4];
		sdl_char_info *stuff;
		stuff = (sdl_char_info*)widgets[11];
		chars[0] = (sdl_animate_button*)widgets[0];
		chars[1] = (sdl_animate_button*)widgets[1];
		chars[2] = (sdl_animate_button*)widgets[2];
		chars[3] = (sdl_animate_button*)widgets[3];
		switch (index)
		{
			case 0:
				stuff->hand_info(chars[0]->get_info());
				cur_char_slot = 0;
				chars[0]->animate(true);
				chars[1]->animate(false);
				chars[2]->animate(false);
				chars[3]->animate(false);
				break;
			case 1:
				stuff->hand_info(chars[1]->get_info());
				cur_char_slot = 1;
				chars[0]->animate(false);
				chars[1]->animate(true);
				chars[2]->animate(false);
				chars[3]->animate(false);
				break;
			case 2:
				stuff->hand_info(chars[2]->get_info());
				cur_char_slot = 2;
				chars[0]->animate(false);
				chars[1]->animate(false);
				chars[2]->animate(true);
				chars[3]->animate(false);
				break;
			case 3:
				stuff->hand_info(chars[3]->get_info());
				cur_char_slot = 3;
				chars[0]->animate(false);
				chars[1]->animate(false);
				chars[2]->animate(false);
				chars[3]->animate(true);
				break;
			default:
				break;
		}
		if (index != -1)
		{
			widgets[index]->mouse_click(here);
		}
	}
}

void draw_char_sel::draw(SDL_Surface *display)
{
	while (SDL_mutexP(draw_mtx) == -1) {};
	if (ready)
	{
		SDL_FillRect(display, NULL, 0);
		sdl_drawmode::draw(display);
	}
	SDL_mutexV(draw_mtx);	
}