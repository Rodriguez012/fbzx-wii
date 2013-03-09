/*********************************************************************
 *
 * Copyright (C) 2012,  Fabio Olimpieri
 *
 * Filename:      menu_sdl.c
 * Author:        Fabio Olimpieri <fabio.olimpieri@tin.it>
 * Description:   a SDL Gui
 * This file is part of FBZX Wii
 *
 * FBZX Wii is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FBZX Wii is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *
 ********************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "menu_sdl.h"
#include "emulator.h"
#include "VirtualKeyboard.h"
#include "tape.h"
#include "menus.h"
#include "cargador.h"
#include "characters.h"

#define ID_BUTTON_OFFSET 0
#define ID_AXIS_OFFSET 32

#ifdef DEBUG
extern FILE *fdebug;
#define printf(...) fprintf(fdebug,__VA_ARGS__)
#else
 #ifdef GEKKO
 #define printf(...)
 #endif
#endif

#define MAX_POKE 20
#define MAX_TRAINER 50

extern int countdown_buffer;
extern unsigned int beeper;

void clean_screen();


static const char *main_menu_messages[] = {
		/*00*/		"Tape",
		/*01*/		"^|Insert|Load|Play|Stop|Rewind|Create|Delete",
		/*02*/		"Snapshot",
		/*03*/		"^|Load|Save|Delete",
		/*04*/		"#1---------------------------------------------",
		/*05*/		"Wiimote configuration",
		/*06*/		"^|Wiimote1|Wiimote2",
		/*07*/		"Emulation settings",
		/*08*/		"Screen settings",
		/*09*/		"Audio settings",
		/*10*/		"Config files",
		/*11*/		"Microdrive",
		/*12*/		"Tools",
		/*13*/		"Reset",
		/*14*/		"Quit",
		NULL
};

static const char *emulation_messages[] = {
		/*00*/		"Emulated machine",
		/*01*/		"^|48k_2|48K_3|128k|+2|+2A/+3|128K_Sp|NTSC",
		/*02*/		"Frame rate",
		/*03*/		"^|100%|50%|33%|25%|20%",
		/*04*/		"Tape instant load",
		/*05*/		"^|on|off",
		/*06*/		"Instant load pause",
		/*07*/		"^|on|off",
		/*08*/		"Turbo mode",
		/*09*/		"^|off|auto|fast|ultrafast",
		/*10*/		"Rewind tape on reset",
		/*11*/		"^|on|off",	
		/*12*/		"Precision",
		/*13*/		"^|on|off",	
		NULL
};

static const char *audio_messages[] = {
		/*00*/		"Volume",
		/*01*/		"^|0|1|2|3|4|5|6|7|max",
		/*02*/		"  ",
		/*03*/		"AY-3-8912 Emulation",
		/*04*/		"^|on|off",	
		/*05*/		"  ",
		/*06*/		"Audio mode",
		/*07*/		"^|mono|ABC|ACB|BAC",
		/*08		"  ", */
		/*09		"Beeper low pass filter",*/
		/*10		"^|0|1|2|3|4|5|6|7|max",*/
		NULL
};

static const char *screen_messages[] = {
		/*00*/		"Double scan",
		/*01*/		"^|on|off",
		/*02*/		"  ",
		/*03*/		"TV mode",
		/*04*/		"^|Color|B&W",
		/*05*/		"  ",
		/*06*/		"Buffer resolution",
		/*07*/		"^|640X480|320X240",
		/*08*/		"  ",
		/*09		"576p video mode",*/
		/*10		"^|on|off",*/
		NULL
};

static const  char *input_messages[] = {
		/*00*/		"Joystick type",
		/*01*/		"^|Cursor|Kempston|Sinclair1|Sinclair2|QAOP",
		/*02*/		"Bind key to Wiimote",
		/*03*/		"^|A|B|1|2|-|+",
		/*04*/		"Bind key to Nunchuk",
		/*05*/		"^|Z|C",
		/*06*/		"Bind key to Classic",
		/*07*/		"^|a|b|x|y|L|R|Zl|Zr|-|+",
		/*08*/		"Bind key to Pad",
		/*09*/		"^|Up|Down|Left|Right",
		/*10*/		"Use Joypad as Joystick",
		/*11*/		"^|On|Off",
		/*12*/		"Rumble",
		/*13*/		"^|On|Off",
		NULL,
};

static const char *microdrive_messages[] = {
		/*00*/		"Microdrive",
		/*01*/		"^|Insert|Create|Delete",
		/*02*/		"  ",
		/*03*/		"Interface I",
		/*04*/		"^|on|off",
		/*05*/		"  ",
		/*06*/		"Write protection",
		/*07*/		"^|on|off",
		NULL
};

static const char *tools_messages[] = {
		/*00*/		"Screen shot",
		/*01*/		"^|Save1|Save2|Load|Delete",
		/*02*/		"Port",
		/*03*/		"^|default|sd|usb|smb|ftp",
		/*04*/		"Auto virtual keyboard",
		/*05*/		"^|on|off",
		/*06*/		"Keyboard rumble",
		/*07*/		"^|on|off",
		/*08*/		"  ",
		/*09*/		"Load poke file",
		/*10*/		"  ",
		/*11*/		"Insert poke",
		/*12*/		"  ",
		/*13*/		"Help",
		NULL
};

static const char *help_messages[] = {
		/*00*/		"#2HOME enters the menu system, where arrow keys",
		/*01*/		"#2and nunchuck are used to navigate up and down.",
		/*02*/		"#2You can bind keyboard keys to the wiimote",
		/*03*/		"#2buttons in the 'Wiimote' menu and",
		/*04*/		"#2change emulation options in the Settings menu.",
		/*05*/		"#2 ",
		/*06*/		"#2The easiest way to play a game is to load",
		/*07*/		"#2a snapshot (.z80 and .sna files).",
		/*08*/		"#2You can also insert a tape file (.tap and .tzx)",
		/*09*/		"#2and then load the file in the tape menu.",
		/*10*/		"#2 ",
		/*11*/		"#2More information is available on the wiki:",
		/*12*/		"#2   http://wiibrew.org/wiki/FBZX_Wii",
		/*13*/		"#2 ",
		/*14*/		"OK",
		NULL,
};

static const char *confs_messages[] = {
		/*00*/		"General configurations",
		/*01*/		"^|Load|Save|Delete",
		/*02*/		"  ",
		/*03*/		"Game configurations",
		/*04*/		"^|Load|Save|Delete",
		/*05*/		"  ",
		/*06*/		"Load confs automatically",
		/*07*/		"^|on|off",
		/*08*/		"  ",
		/*09*/		"Ignore .z80 joystick confs",
		/*10*/		"^|on|off",
		
		NULL
};

void maybe_load_conf(const char *filename)
{
	const char *dir = path_confs;
	char *ptr;
	char db[MAX_PATH_LENGTH];
	char fb[81];
	
	if (filename==NULL) return;	

	if (strrchr(filename, '/'))
		strncpy(fb, strrchr(filename, '/') + 1, 80);
	else
		strncpy(fb, filename, 80);
		
	//remove the extension
	ptr = strrchr (fb, '.');
		if (ptr) *ptr = 0;	

	snprintf(db, MAX_PATH_LENGTH-1, "%s/%s.conf", dir, fb);
	if (!load_config(&ordenador,db)) msgInfo("Configurations loaded",2000,NULL)	;
	
}

static void insert_tape()
{
	unsigned char char_id[11];
	int retorno, retval;
	
	const char *filename = menu_select_file(load_path_taps, ordenador.current_tap, 1);
	
	if (filename==NULL) // Aborted
		return; 
	
	if (strstr(filename, "None") != NULL)
	{
			ordenador.current_tap[0] = '\0';
			free((void *)filename);
			if(ordenador.tap_file!=NULL) {
			fclose(ordenador.tap_file);
			}
			ordenador.tap_file=NULL;
			ordenador.tape_file_type = TAP_TAP;
			return;
	}
	
	if (!(ext_matches(filename, ".tap")|ext_matches(filename, ".TAP")|ext_matches(filename, ".tzx")|
	ext_matches(filename, ".TZX"))) {free((void *)filename); return;}
	
	ordenador.tape_current_bit=0;
	ordenador.tape_current_mode=TAP_TRASH;
	ordenador.next_block= NOBLOCK;
	
	if(ordenador.tap_file!=NULL) {
		fclose(ordenador.tap_file);
	}

	if (!strncmp(filename,"smb:",4)) ordenador.tap_file=fopen(filename,"r"); //tinysmb does not work with r+
	else ordenador.tap_file=fopen(filename,"r+"); // read and write
	
	ordenador.tape_write = 1; // by default, can record
	
	if(ordenador.tap_file==NULL)
		retorno=-1;
	else
		retorno=0;
	
	switch(retorno) {
	case 0: // all right
	strcpy(ordenador.current_tap,filename);
	strcpy(ordenador.last_selected_file,filename);
	if (ordenador.autoconf) maybe_load_conf(filename);
	break;
	case -1:
		msgInfo("Error: Can't load that file",3000,NULL);
		ordenador.current_tap[0]=0;
		free((void *)filename);
		return;
	break;
	}
	
	free((void *)filename);
	
	retval=fread(char_id,10,1,ordenador.tap_file); // read the (maybe) TZX header
	if((!strncmp(char_id,"ZXTape!",7)) && (char_id[7]==0x1A)&&(char_id[8]==1)) {
		ordenador.tape_file_type = TAP_TZX;
		rewind_tape(ordenador.tap_file,1);
	} else {
		ordenador.tape_file_type = TAP_TAP;
		rewind_tape(ordenador.tap_file,1);
	}
}

static void delete_tape()
{
	const char *filename = menu_select_file(load_path_taps, NULL, 1);
	
	if (filename==NULL) // Aborted
		return; 
	
	if ((ext_matches(filename, ".tap")|ext_matches(filename, ".TAP")|ext_matches(filename, ".tzx")|
	ext_matches(filename, ".TZX"))
	&& (msgYesNo("Delete the file?", 0, FULL_DISPLAY_X /2-138/RATIO, FULL_DISPLAY_Y /2-48/RATIO))) unlink(filename);
	
	free((void *)filename);
}

void create_tapfile_sdl() {

	unsigned char *videomem;
	int ancho,retorno;
	unsigned char nombre2[MAX_PATH_LENGTH];

	videomem=screen->pixels;
	ancho=screen->w;

	clean_screen();
	
	print_string(videomem,"Choose a name for the TAP file",-1,32,14,0,ancho);
	print_string(videomem,"(up to 30 characters)",-1,52,14,0,ancho);

	print_string(videomem,"TAP file will be saved in:",-1,112,12,0,ancho);
	print_string(videomem,path_taps,0,132,12,0,ancho);


	retorno=ask_filename_sdl(nombre2,82,"tap",path_taps,NULL);
	

	if(retorno==2) // abort
		return;

	if(ordenador.tap_file!=NULL)
		fclose(ordenador.tap_file);
	
	ordenador.tap_file=fopen(nombre2,"r"); // test if it exists
	if(ordenador.tap_file==NULL)
		retorno=0;
	else
		retorno=-1;
	
	if(!retorno) {
		ordenador.tap_file=fopen(nombre2,"a+"); // create for read and write
		if(ordenador.tap_file==NULL)
			retorno=-2;
		else
			retorno=0;
	}
	ordenador.tape_write=1; // allow to write
	strcpy(ordenador.current_tap,nombre2);
	ordenador.tape_file_type = TAP_TAP;
	switch(retorno) {
	case 0:
	strcpy(ordenador.last_selected_file,nombre2);
	break;
	case -1:
		msgInfo("File already exists",3000,NULL);
		ordenador.current_tap[0]=0;
	break;
	case -2:
		msgInfo("Can't create file",3000,NULL);
		ordenador.current_tap[0]=0;
	break;
	}
	clean_screen();
}	

static int manage_tape(int which)
{
	int retorno=0; //Stay in menu as default
	switch (which)
	{
	case 0: //Insert
		insert_tape();
		break;
	case 1: //Emulate load ""
		countdown_buffer=8;
		switch (ordenador.mode128k)
		{
		case 4://Spanish 128k
			ordenador.keyboard_buffer[0][8]= SDLK_l;		
			ordenador.keyboard_buffer[1][8]= 0;
			ordenador.keyboard_buffer[0][7]= SDLK_o;		
			ordenador.keyboard_buffer[1][7]= 0;
			ordenador.keyboard_buffer[0][6]= SDLK_a;		
			ordenador.keyboard_buffer[1][6]= 0;
			ordenador.keyboard_buffer[0][5]= SDLK_d;		
			ordenador.keyboard_buffer[1][5]= 0;
			ordenador.keyboard_buffer[0][4]= SDLK_p;		//"	
			ordenador.keyboard_buffer[1][4]= SDLK_LCTRL;
			ordenador.keyboard_buffer[0][3]= SDLK_p;		//"	
			ordenador.keyboard_buffer[1][3]= SDLK_LCTRL;
			ordenador.keyboard_buffer[0][2]= SDLK_RETURN;	// Return
			ordenador.keyboard_buffer[1][2]= 0;
			ordenador.keyboard_buffer[0][1]= SDLK_F6;		//F6 - play
			ordenador.keyboard_buffer[1][1]= 0;
			ordenador.kbd_buffer_pointer=8;
			break;
		case 3: //+3
		case 2: //+2
		case 1: //128k
			ordenador.kbd_buffer_pointer=2;
			if (ordenador.mport1 & 0x10) //ROM 48k
			{
			ordenador.keyboard_buffer[0][5]= SDLK_j;		//Load
			ordenador.keyboard_buffer[1][5]= 0;
			ordenador.keyboard_buffer[0][4]= SDLK_p;		//"
			ordenador.keyboard_buffer[1][4]= SDLK_LCTRL;
			ordenador.keyboard_buffer[0][3]= SDLK_p;		//"
			ordenador.keyboard_buffer[1][3]= SDLK_LCTRL;
			ordenador.kbd_buffer_pointer=5;
			}
			ordenador.keyboard_buffer[0][2]= SDLK_RETURN;	// Return
			ordenador.keyboard_buffer[1][2]= 0;
			ordenador.keyboard_buffer[0][1]= SDLK_F6;		//F6 - play
			ordenador.keyboard_buffer[1][1]= 0;
			break;
		case 0: //48k
		default:
			ordenador.keyboard_buffer[0][5]= SDLK_j;		//Load
			ordenador.keyboard_buffer[1][5]= 0;
			ordenador.keyboard_buffer[0][4]= SDLK_p;		//"
			ordenador.keyboard_buffer[1][4]= SDLK_LCTRL;
			ordenador.keyboard_buffer[0][3]= SDLK_p;		//"
			ordenador.keyboard_buffer[1][3]= SDLK_LCTRL;
			ordenador.keyboard_buffer[0][2]= SDLK_RETURN;	// Return
			ordenador.keyboard_buffer[1][2]= 0;
			ordenador.keyboard_buffer[0][1]= SDLK_F6;		//F6
			ordenador.keyboard_buffer[1][1]= 0;
			ordenador.kbd_buffer_pointer=5;
			break;
		}
		retorno=-1;
		break;	
	case 2: //Play
		if (ordenador.tape_fast_load == 0)
				ordenador.tape_stop = 0;
				ordenador.tape_stop_fast = 0;
				ordenador.stop_tape_start_countdown = 0;
		retorno=-1;
		break;
	case 3: //Stop
		//if (ordenador.tape_fast_load == 0)
				ordenador.tape_stop = 1;
				ordenador.tape_stop_fast = 1;
				ordenador.stop_tape_start_countdown = 1;
		retorno=-1;		
		break;
	case 4: //Rewind
			ordenador.tape_stop=1;
			ordenador.tape_stop_fast = 1;
			ordenador.tape_start_countdwn=0;
			if(ordenador.tap_file!=NULL) {
				ordenador.tape_current_mode=TAP_TRASH;
				rewind_tape(ordenador.tap_file,1);		
			}
			//msgInfo("Tape rewound",3000,NULL);
			sprintf (ordenador.osd_text, "Rewind tape");			
			ordenador.osd_time = 100;
			retorno=-1;
		break;
	case 5: //Create
		// Create tape 
		create_tapfile_sdl();
		break;	
	case 6: //Delete
		delete_tape();
		break;	
	default:
		break;
	}
	return retorno;
}

static unsigned int get_machine_model(void)
{
	if (ordenador.videosystem == 0)
	return  (ordenador.mode128k + (ordenador.issue==3));
	else return (6); 
}

static void set_machine_model(int which)
{
	switch (which)
	{
	case 0: //48k issue2
			ordenador.issue=2;
			ordenador.mode128k=0;
			ordenador.ay_emul=0;
			ordenador.videosystem =0;
		break;
	case 1: //48k issue3
			ordenador.issue=3;
			ordenador.mode128k=0;
			ordenador.ay_emul=0;
			ordenador.videosystem =0;
		break;
	case 2: //128k
			ordenador.issue=3;
			ordenador.mode128k=1;
			ordenador.ay_emul=1;
			ordenador.videosystem =0;
		break;
	case 3: //Amstrad +2
			ordenador.issue=3;
			ordenador.mode128k=2;
			ordenador.ay_emul=1;
			ordenador.videosystem =0;
		break;
	case 4: //Amstrad +2A/+3
			ordenador.issue=3;
			ordenador.mode128k=3;
			ordenador.ay_emul=1;
			ordenador.mdr_active=0;
			ordenador.videosystem =0;
		break;
	case 5: //128K Spanish
			ordenador.issue=3;
			ordenador.mode128k=4;
			ordenador.ay_emul=1;
			ordenador.videosystem =0;
		break;
	case 6: //48k ntsc
			ordenador.issue=3;
			ordenador.mode128k=0;
			ordenador.ay_emul=0;
			ordenador.videosystem =1;
		break;
	}
}

static int emulation_settings(void)
{
	unsigned int submenus[7],submenus_old[7];
	int opt, i, retorno;
	unsigned char old_mode, old_videosystem;
	
	retorno=-1; //exit to the previous menue
	
	memset(submenus, 0, sizeof(submenus));
	
	submenus[0] = get_machine_model();
	submenus[1] = jump_frames;
	submenus[2] = !ordenador.tape_fast_load;
	submenus[3] = !ordenador.pause_instant_load;
	submenus[4] = ordenador.turbo;
	submenus[5] = !ordenador.rewind_on_reset;
	submenus[6] = !ordenador.precision;

	
	for (i=0; i<7; i++) submenus_old[i] = submenus[i];
	old_mode=ordenador.mode128k;
	old_videosystem = ordenador.videosystem;
	
	opt = menu_select_title("Emulation settings menu",
			emulation_messages, submenus);
	if (opt < 0)
		return retorno;
	
	if (submenus[0]!=submenus_old[0]) set_machine_model(submenus[0]);
	if ((old_mode!=ordenador.mode128k)||(old_videosystem!=ordenador.videosystem)) {ResetComputer(); retorno=-2;} 
	
	jump_frames = submenus[1];
	ordenador.tape_fast_load = !submenus[2];
	ordenador.pause_instant_load = !submenus[3];
	
	ordenador.turbo = submenus[4];
	ordenador.rewind_on_reset = !submenus[5];
	
	curr_frames=0;
	if (submenus[4] != submenus_old[4])
	{
	switch(ordenador.turbo)
	{
	case 1: //auto
		//ordenador.precision =0;
	case 0: //off
		update_frequency(0); //set machine frequency
		jump_frames=0;
		ordenador.turbo_state=0;
		ordenador.precision = ordenador.precision_old;
		break;
	case 2:	//fast	
		update_frequency(10000000);
		jump_frames=4;
		ordenador.precision =0;
		ordenador.precision_old =0;
		ordenador.turbo_state=2;
		break;
	case 3:	//ultra fast
		update_frequency(14000000);
		jump_frames=24;
		ordenador.precision =0;
		ordenador.precision_old =0;
		ordenador.turbo_state=3;
		break;
	default:
		break;	
	}
	}
	
	if (submenus[6] != submenus_old[6])
	{
	ordenador.precision = !submenus[6];
	ordenador.precision_old=ordenador.precision;
	if (ordenador.turbo_state!=1)  //Tape is not loading with turbo mode
	 if (ordenador.precision)
		{ 
		update_frequency(0);
		jump_frames=0;
			if (ordenador.turbo!=1)
			{
			ordenador.turbo =0;
			ordenador.turbo_state=0;
			}
			
		}
	}
	
	return retorno;
}

unsigned int get_value_filter (unsigned int value)
{
 switch (value)
 {
 case 480:
	return(0);
 case 240:
	return(1);
 case 160:
	return(2);
 case 120:
	return(3);
 case 80:
	return(4);
 case 60:
	return(5);
 case 40:
	return(6);
 case 24:
	return(7);
 case 10:
	return(8);
 default:
	return(0);
 }
}

unsigned int set_value_filter (unsigned int value)
{
	unsigned int set[9] = {480,240,160,120,80,60,40,24,10};
	beeper=0;
	return set[value];
}

static void audio_settings(void)
{
	unsigned int submenus[4];
	int opt;

	
	memset(submenus, 0, sizeof(submenus));
	
	
	submenus[0] = ordenador.volume/2;
	submenus[1] = !ordenador.ay_emul;
	submenus[2] = ordenador.audio_mode;
	submenus[3] = get_value_filter(ordenador.low_filter);
	
	
	opt = menu_select_title("Audio settings menu",
			audio_messages, submenus);
	if (opt < 0)
		return;

	
	ordenador.volume = submenus[0]*2; 
	ordenador.ay_emul = !submenus[1];
	ordenador.audio_mode = submenus[2];
	ordenador.low_filter= set_value_filter(submenus[3]);
	
	
}

static void save_load_general_configurations(int);

static void screen_settings(void)
{
	unsigned int submenus[4],submenus_old[4];
	int opt, i;
	
	memset(submenus, 0, sizeof(submenus));
	
	submenus[0] = !ordenador.dblscan;
	submenus[1] = ordenador.bw;
	submenus[2] = ordenador.zaurus_mini?1:0;
	submenus[3] = !ordenador.progressive;
	
	for (i=0; i<4; i++) submenus_old[i] = submenus[i];
	
	
	opt = menu_select_title("Screen settings menu",
			screen_messages, submenus);
	if (opt < 0)
		return;
	
	ordenador.dblscan = !submenus[0];
	ordenador.bw = submenus[1]; 
	ordenador.progressive = !submenus[3];
	
	if (submenus[0] != submenus_old[0]) update_npixels();
	
	if (submenus[1]!=submenus_old[1]) computer_set_palete();
	
	if (submenus[2] != submenus_old[2])
	{
		if (submenus[2]==0) {ordenador.zaurus_mini = 0; ordenador.text_mini=0;}
		else {ordenador.zaurus_mini = 3; ordenador.text_mini=1;}
		update_npixels();
	    restart_video();
	}
	if (submenus[3] != submenus_old[3])
	{
		switch (set_video_mode()) 
		{
		case 1:
		msgInfo("Necessary component cable",3000,NULL);
		ordenador.progressive = 0;
		break;
		case 2:
		msgInfo("Only avalaible from 576i PAL",3000,NULL);
		ordenador.progressive = 0;
		break;
		}
	}
}

static void setup_joystick(int joy, unsigned int sdl_key, int joy_key)
{
	int loop;
	
	//Cancel the previous assignement - it is not possible to assign a same sdl_key to 2 joybuttons
	for (loop=0; loop<23; loop++)
	 if (ordenador.joybuttonkey[joy][loop] == sdl_key) ordenador.joybuttonkey[joy][loop] =0;
	
	ordenador.joybuttonkey[joy][joy_key] = sdl_key;
	
}

static void input_options(int joy)
{
	const unsigned int wiimote_to_sdl[] = {0, 1, 2, 3, 4,5};
	const unsigned int nunchuk_to_sdl[] = {7, 8};
	const unsigned int classic_to_sdl[] = {9, 10, 11, 12, 13, 14, 15, 16, 17,18};
	const unsigned int pad_to_sdl[] = {19, 20, 21, 22};
	int joy_key = 1;
	unsigned int sdl_key;
	unsigned int submenus[7];
	int opt;
	
	struct virtkey *virtualkey;

	do {
	memset(submenus, 0, sizeof(submenus));
	
	submenus[0] = ordenador.joystick[joy];
	submenus[5] = !ordenador.joypad_as_joystick[joy];
	submenus[6] = !ordenador.rumble[joy];
	
	opt = menu_select_title("Input menu",
			input_messages, submenus);
	if (opt < 0)
		return;
	
	ordenador.joystick[joy] = submenus[0];
	ordenador.joypad_as_joystick[joy] = !submenus[5];
	ordenador.rumble[joy] = !submenus[6];
	
	if (opt == 0 || opt == 10|| opt == 12)
		return;
	
	VirtualKeyboard.sel_x = 64;
	VirtualKeyboard.sel_y = 90;
	
	virtualkey = get_key();
	if (virtualkey == NULL)
		return;
	sdl_key = virtualkey->sdl_code;
	
	if (virtualkey->sdl_code==1) //"Done" selected
		{if (virtualkey->caps_on)  sdl_key = 304; //Caps Shit
			else if (virtualkey->sym_on)  sdl_key = 306; //Sym Shit
			else return; } 
	
	switch(opt)
		{
		case 2: // wiimote 
			joy_key = wiimote_to_sdl[submenus[1]]; break;
		case 4: // nunchuk
			joy_key = nunchuk_to_sdl[submenus[2]]; break;
		case 6: // classic
			joy_key = classic_to_sdl[submenus[3]]; break;
		case 8: // pad
			joy_key = pad_to_sdl[submenus[4]]; break;
		default:
			break;
		}
		
	setup_joystick(joy, sdl_key, joy_key);
	} while (opt == 2 || opt == 4 || opt == 6 || opt == 8);
	
	
}

static int select_mdr()
{
	int retorno, retval, retorno2;
	
	retorno2=0; //stay in menu as default

	const char *filename = menu_select_file(path_mdrs, ordenador.mdr_current_mdr, 0);
	
	if (filename==NULL) // Aborted
		return 0; 
		
	if (strstr(filename, "None") != NULL)
	{
			ordenador.mdr_current_mdr[0] = '\0';
			free((void *)filename);
			return 0;
	}	
	
	if (!(ext_matches(filename, ".mdr")|ext_matches(filename, ".MDR"))) {free((void *)filename); return 0;}
	
	ordenador.mdr_file=fopen(filename,"rb"); // read
	if(ordenador.mdr_file==NULL)
		retorno=-1;
	else {
		retorno=0;
		retval=fread(ordenador.mdr_cartridge,137923,1,ordenador.mdr_file); // read the cartridge in memory
		ordenador.mdr_modified=0; // not modified
		fclose(ordenador.mdr_file);
		ordenador.mdr_tapehead=0;
	}

	strcpy(ordenador.mdr_current_mdr,filename);

	free((void *)filename);

	switch(retorno) {
	case 0: // all right
	retorno2=-1;
		break;
	default:
		ordenador.mdr_current_mdr[0]=0;
		msgInfo("Error: Can't load that file",3000,NULL);
	retorno2=0;	
	break;
	}
	return retorno2;
}

static void delete_mdr()
{
	const char *filename = menu_select_file(path_mdrs, NULL, 0);
	
	if (filename==NULL) // Aborted
		return; 
	
	if ((ext_matches(filename, ".mdr")|ext_matches(filename, ".MDR"))
	&& (msgYesNo("Delete the file?", 0, FULL_DISPLAY_X /2-138/RATIO, FULL_DISPLAY_Y /2-48/RATIO))) unlink(filename);
	
	free((void *)filename);
}

void create_mdrfile_sdl() {

	unsigned char *videomem;
	int ancho,retorno,bucle,retval;
	unsigned char nombre2[MAX_PATH_LENGTH];

	videomem=screen->pixels;
	ancho=screen->w;

	clean_screen();

	print_string(videomem,"Choose a name for the MDR file",-1,32,14,0,ancho);
	print_string(videomem,"(up to 30 characters)",-1,52,14,0,ancho);

	print_string(videomem,"MDR file will be saved in:",-1,112,12,0,ancho);
	print_string(videomem,path_mdrs,0,132,12,0,ancho);

	retorno=ask_filename_sdl(nombre2,82,"mdr",path_mdrs, NULL);

	if(retorno==2) // abort
		return;

	ordenador.mdr_file=fopen(nombre2,"r"); // test if it exists
	if(ordenador.mdr_file==NULL)
		retorno=0;
	else
		retorno=-1;
	
	if(!retorno) {
		ordenador.mdr_file=fopen(nombre2,"wb"); // create for write
		if(ordenador.mdr_file==NULL)
			retorno=-2;
		else {
			for(bucle=0;bucle<137921;bucle++)
				ordenador.mdr_cartridge[bucle]=0xFF; // erase cartridge
			ordenador.mdr_cartridge[137922]=0;
			retval=fwrite(ordenador.mdr_cartridge,137923,1,ordenador.mdr_file); // save cartridge
			fclose(ordenador.mdr_file);
			ordenador.mdr_file=NULL;
			ordenador.mdr_modified=0;
			retorno=0;
		}
	}	
	strcpy(ordenador.mdr_current_mdr,nombre2);	
	switch(retorno) {
	case 0:
	break;
	case -1:
		msgInfo("File already exists",3000,NULL);
		ordenador.mdr_current_mdr[0]=0;
	break;
	case -2:
		msgInfo("Can't create file",3000,NULL);
		ordenador.mdr_current_mdr[0]=0;
	break;
	}
	clean_screen();
}

static void microdrive()
{
	
	unsigned int submenus[3], submenus_old[3];
	int opt,retval, retorno ;

	memset(submenus, 0, sizeof(submenus));
	
	do {
	retorno=-1; //Exit from menu as default
	
	submenus[1] = !ordenador.mdr_active;
	submenus[2] = !ordenador.mdr_cartridge[137922];
	
	submenus_old[1] = submenus[1];
	submenus_old[2] = submenus[2];
	
	opt = menu_select_title("Microdrive menu",
			microdrive_messages, submenus);
	if (opt < 0)
		return;
	
	ordenador.mdr_active = !submenus[1];
	
	
	if (submenus[1]!=submenus_old[1]) ResetComputer();
	if (submenus[2]!=submenus_old[2]) 
		{if(ordenador.mdr_cartridge[137922])
				ordenador.mdr_cartridge[137922]=0;
			else
				ordenador.mdr_cartridge[137922]=1;
			ordenador.mdr_file=fopen(ordenador.mdr_current_mdr,"wb"); // create for write
			if(ordenador.mdr_file!=NULL) {				
				retval=fwrite(ordenador.mdr_cartridge,137923,1,ordenador.mdr_file); // save cartridge
				fclose(ordenador.mdr_file);
				ordenador.mdr_file=NULL;
				ordenador.mdr_modified=0;
			}			
		}
	
	if (opt==0)
		switch (submenus[0]) 
		{
		case 0: // Insert microdrive 
			retorno = select_mdr();
			break;
		case 1: // Create microdrive file
			create_mdrfile_sdl();
			retorno=0;
			break;
		case 2: // Delete microdrive file
			delete_mdr();
			retorno=0;
			break;
		default:
			break;
		}
	} while (!retorno); //Stay in menu if create or delete are selected	
}
	
static void delete_scr()
{
	const char *filename = menu_select_file(getenv("HOME"), NULL, 0); //Start from Home
	
	if (filename==NULL) // Aborted
		return; 
	
	if ((ext_matches(filename, ".scr")|ext_matches(filename, ".SCR"))
	&& (msgYesNo("Delete the file?", 0, FULL_DISPLAY_X /2-138/RATIO, FULL_DISPLAY_Y /2-48/RATIO))) unlink(filename);
	
	free((void *)filename);
}

static int load_scr()
{
	int retorno,loop, retorno2;;
	unsigned char value;
	FILE *fichero;
	unsigned char paleta_tmp[64];
	retorno2=0; //stay in the menu as default


	const char *filename = menu_select_file(load_path_scr1, NULL, 0); // Load from SCR1
	
	if (filename==NULL) // Aborted
		return 0; 
	
	if (!(ext_matches(filename, ".scr")|ext_matches(filename, ".SCR"))) {free((void *)filename); return 0;}
	
	ordenador.osd_text[0]=0;
	fichero=fopen(filename,"rb");
	retorno=0;
	if (!fichero) {
		retorno=-1;
	} else {
		for(loop=0;loop<6912;loop++) {
			if (1==fread(&value,1,1,fichero)) {
				*(ordenador.block1 + 0x04000 + loop) = value;
			} else {
				retorno=-1;
				break;
			}
		}
		if (1==fread(paleta_tmp,64,1,fichero)) {
			memcpy(ordenador.ulaplus_palete,paleta_tmp,64);
			ordenador.ulaplus=1;
		} else {
			ordenador.ulaplus=0;
		}
		fclose(fichero);
	}

	switch(retorno) {
	case 0: // all right
		strcpy(ordenador.last_selected_file,filename);
		retorno2=-2; //come back to emulator
		break;
	case -1:
		msgInfo("Error: Can't load that file",3000,NULL);
		retorno2=0;
		break;
	default:
		break;	
	}
	
	free((void *)filename);
	
	return retorno2;
	
}

static int save_scr(int i)
{
	const char *dir; 
	const char *tape = ordenador.last_selected_file;
	char *ptr;
	FILE *fichero;
	char db[MAX_PATH_LENGTH];
	char fb[81];
	int retorno,retval, retorno2;
	retorno2=0; //Stay in menu as default
	
	if (i==1) dir=path_scr1; else
	 if (i==2) dir=path_scr2; else return 0;
	
	// Name (for saves) - TO CHECK
	if (tape && strrchr(tape, '/'))
		strncpy(fb, strrchr(tape, '/') + 1, 80);
	else
		strcpy(fb, "unknown");
		
	//remove the extension
	ptr = strrchr (fb, '.');
		if (ptr) *ptr = 0;
					
	
	// Save SCR file		
	snprintf(db, MAX_PATH_LENGTH-1, "%s/%s.scr", dir, fb);
	
		
	fichero=fopen(db,"r");
	
	if(fichero!=NULL)
	{	
		fclose(fichero);
		if (!msgYesNo("Overwrite the exiting file?", 0, FULL_DISPLAY_X /2-160/RATIO, FULL_DISPLAY_Y /2-48/RATIO))
			return 0; // file already exists
	}
	
	fichero=fopen(db,"wb"); // create for write
		
	if(fichero==NULL)
		retorno=-1;
	else {
		retval=fwrite(ordenador.block1+0x04000+ordenador.video_offset,6912,1,fichero); // save screen
		if (ordenador.ulaplus!=0) {
			retval=fwrite(ordenador.ulaplus_palete,64,1,fichero); // save ULAPlus palete
			}
		fclose(fichero);
		retorno=0;
		}

	switch(retorno) {
	case 0:
		if (i==1) msgInfo("Screen 1 saved",3000,NULL); else msgInfo("Screen 2 saved",3000,NULL);
		retorno2=-2; //come back to emulator
		break;
	case -1:
		msgInfo("Can't create file",3000,NULL);
		retorno2=0;
	break;
	default:
	break;
	}
	return retorno2;
}

static void set_port(int which)
{
	int length;
	
	switch (which)
	{
	case 0: //PORT_DEFAULT
		strcpy(load_path_snaps,getenv("HOME"));
		length=strlen(load_path_snaps);
		if ((length>0)&&(load_path_snaps[length-1]!='/')) strcat(load_path_snaps,"/");
		strcpy(load_path_taps,load_path_snaps);
		strcpy(load_path_scr1,load_path_snaps);
		strcpy(load_path_poke,load_path_snaps);
		strcat(load_path_snaps,"snapshots");
		strcat(load_path_taps,"tapes");
		strcat(load_path_scr1,"scr");
		strcat(load_path_poke,"poke");
		ordenador.port = which;
		break;
	case 1: //PORT_SD
		if (sdismount) {
			strcpy(load_path_snaps,"sd:/");
			strcpy(load_path_taps,"sd:/");
			strcpy(load_path_scr1,"sd:/");
			strcpy(load_path_poke,"sd:/");
			ordenador.port = which;}
		else
			msgInfo("SD is not mounted",3000,NULL);
		break;
	case 2: //PORT_USB
		if (usbismount) {
			strcpy(load_path_snaps,"usb:/");
			strcpy(load_path_taps,"usb:/");
			strcpy(load_path_scr1,"usb:/");
			strcpy(load_path_poke,"usb:/");
			ordenador.port = which;}
		else
			msgInfo("USB is not mounted",3000,NULL);
		break;
	case 3: //PORT_SMB
		if (!smbismount)
		{
			msgInfo("Try to mount SMB",0,NULL);
			if (!networkisinit) networkisinit = InitNetwork();
			if (networkisinit) ConnectShare();
			if (smbismount) msgInfo("SMB is now mounted",3000,NULL);
		}
		if (smbismount) {
			strcpy(load_path_snaps,"smb:/");
			strcpy(load_path_taps,"smb:/");
			strcpy(load_path_scr1,"smb:/");
			strcpy(load_path_poke,"smb:/");
			ordenador.port = which;}
		else
			msgInfo("SMB is not mounted",3000,NULL);
		break;
	case 4: //PORT_FTP
		if (!ftpismount)
		{
			msgInfo("Try to mount FTP",0,NULL);
			if (!networkisinit) networkisinit = InitNetwork();
			if (networkisinit) ConnectFTP();
			if (ftpismount) msgInfo("FTP is now mounted",3000,NULL);
		}
		
		if (ftpismount) {
			strcpy(load_path_snaps,"ftp:/");
			strcpy(load_path_taps,"ftp:/");
			strcpy(load_path_scr1,"ftp:/");
			strcpy(load_path_poke,"ftp:/");
			ordenador.port = which;}
		else
			msgInfo("FTP is not mounted",3000,NULL);
		break;	
	default:
		break;		
	}	
}

// shows the POKE menu

static int do_poke_sdl() {

	unsigned char *videomem,string[80];
	int ancho,retorno,address,old_value,new_value;

	videomem=screen->pixels;
	ancho=screen->w;

	clean_screen();

	while(1) {
		print_string(videomem,"Type address to POKE",-1,32,15,0,ancho);

		retorno=ask_value_sdl(&address,84,65535);

		clean_screen();

		if (retorno==0) { //Escape
			return (0);
		}
		
		if (retorno==-1) { //Done
			return (-2); //come back yo emulator
		}

		if ((address<16384) && ((ordenador.mode128k != 3) || (1 != (ordenador.mport2 & 0x01)))) {
			print_string(videomem,"That address is ROM memory.",-1,13,15,0,ancho);
			continue;
		}

		switch (address & 0x0C000) {
		case 0x0000:
			old_value= (*(ordenador.block0 + address));
		break;

		case 0x4000:
			old_value= (*(ordenador.block1 + address));
		break;

		case 0x8000:
			old_value= (*(ordenador.block2 + address));
		break;

		case 0xC000:
			old_value= (*(ordenador.block3 + address));
		break;
		default:
			old_value=0;
		break;
		}

		print_string(videomem,"Type new value to POKE",-1,32,15,0,ancho);
		sprintf(string,"Address: %d; old value: %d\n",address,old_value);
		print_string(videomem,string,-1,130,14,0,ancho);

		retorno=ask_value_sdl(&new_value,84,255);

		clean_screen();

		if (retorno==0) { //Escape
			return (0);
		}

		switch (address & 0x0C000) {
		case 0x0000:
			(*(ordenador.block0 + address))=new_value;
		break;

		case 0x4000:
			(*(ordenador.block1 + address))=new_value;
		break;

		case 0x8000:
			(*(ordenador.block2 + address))=new_value;
		break;

		case 0xC000:
			(*(ordenador.block3 + address))=new_value;
		break;
		default:
		break;
		}

		sprintf(string,"Set address %d from %d to %d\n",address,old_value,new_value);
		print_string(videomem,string,-1,130,14,0,ancho);

	}
}


int parse_poke (const char *filename)
{
	static unsigned char old_poke[MAX_TRAINER][MAX_POKE]; //Max 19 Pokes per trainer and max 50 trainer
	FILE* fpoke;
	unsigned char title[128], flag, newfile, restore, old_mport1;
	int bank, address, value, original_value, ritorno,y,k, trainer, poke;
	SDL_Rect src, banner;

	src.x=0;
	src.y=30/RATIO;
	src.w=FULL_DISPLAY_X;
	src.h=FULL_DISPLAY_Y-60/RATIO;

	banner.x=0;
	banner.y=30/RATIO;
	banner.w=FULL_DISPLAY_X;
	banner.h=20/RATIO;

	y=60/RATIO;

	if (strcmp(ordenador.last_selected_poke_file,filename)) newfile=1; else newfile=0;

	trainer=0;

	fpoke = fopen(filename,"r");

	if (fpoke==NULL) 
	{
		msgInfo("Can not access the file",3000,NULL);	
		return (-1);
	}

	clean_screen();

	SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));

	print_font(screen, 0x0, 0x0, 0x0,0, 30/RATIO, "Press 1 to deselect, 2 to select", 16);

	ritorno=0;
	do
	{
		if (trainer==MAX_TRAINER) {ritorno=2;break;}
	
		poke=1;
		restore=0;
		if (!fgets(title,128,fpoke)) {ritorno=1;break;}
		if (title[0]=='Y') break;
		if (title[0]!='N') {ritorno=1;break;}

		if (strlen(title)>1) title[strlen(title)-2]='\0'; //cancel new line and line feed

		if (y>420/RATIO) {SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));y=40/RATIO;}
	
		if (newfile) print_font(screen, 0x80, 0x80, 0x80,0, y, title+1, 16);
		else {if (old_poke[trainer][0]==0) print_font(screen, 0xd0, 0, 0,0, y, title+1, 16); //In row 0 information on trainer selection 
				else print_font(screen, 0, 0xd0, 0,0, y, title+1, 16);}

		SDL_Flip(screen);
		k=0;

		while (!((k & KEY_ESCAPE)||(k & KEY_SELECT)))
		{k = menu_wait_key_press();}
	
		banner.y=y;
	
		SDL_FillRect(screen, &banner, SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));
	
		if (k & KEY_SELECT) 
		{
			print_font(screen, 0, 0x40, 0,0, y, title+1, 16);
			old_poke[trainer][0]=1;
		}
		else 
		{
			if ((!newfile)&&(old_poke[trainer][0]==1)) restore=1;
			print_font(screen, 0x80, 0, 0,0, y, title+1, 16);
			old_poke[trainer][0]=0;
		}	
	
		SDL_Flip(screen);

		y+=20/RATIO;
					
		do 
		{
			if (poke==MAX_POKE) old_poke[trainer][0]=0; //in order not to restore the old_value
		
			fscanf(fpoke, "%1s %d %d %d %d", &flag, &bank, &address, &value, &original_value);
			if (((flag!='M')&&(flag!='Z'))||(bank>8)||(bank<0)||(address>0xFFFF)||(address<0x4000)||(value>256)||(value<0)||(original_value>255)||(original_value<0)) {ritorno=1;break;}
			if (feof(fpoke)) {ritorno=1;break;}
			if ((!(bank&0x8))&&(ordenador.mode128k)) //128k,+2,+3,SP
			{
				old_mport1 = ordenador.mport1;
				ordenador.mport1 = (unsigned char) (bank&0x7);
				set_memory_pointers ();	// set the pointers
			
				if (poke<MAX_POKE)
				{
					if(newfile)
						{if (original_value) old_poke[trainer][poke]=(unsigned char) original_value; else old_poke[trainer][poke]= Z80free_Rd_fake ((word) address);}
					if (restore) value = (int) old_poke[trainer][poke]; 
				}
				//if ((value == 256) && (k & KEY_SELECT)) {value = choice_value(); Z80free_Wr_fake ((word)address, (unsigned char) value);} TODO
				if (((value < 256) && (k & KEY_SELECT))||(restore)) Z80free_Wr_fake ((word)address, (unsigned char) value);
				ordenador.mport1 = old_mport1;
				set_memory_pointers ();	// set the pointers
			}
			else 
			{
				if (poke<MAX_POKE)
				{
					if(newfile)
						{if (original_value) old_poke[trainer][poke]=(unsigned char) original_value; else old_poke[trainer][poke]= Z80free_Rd_fake ((word) address);}
					if (restore) value = (int) old_poke[trainer][poke];
				}
				//if ((value == 256) && (k & KEY_SELECT)) {value = choice_value(); Z80free_Wr_fake ((word)address, (unsigned char) value);} TODO
				if (((value < 256) && (k & KEY_SELECT))||(restore)) Z80free_Wr_fake ((word)address, (unsigned char) value);
			}
			poke++;
		}
		while (flag!='Z');
	
		trainer++;
	
		if (!fgets(title,128,fpoke)) {ritorno=1;break;} //line feed reading
	
	} 
	while (ritorno==0);

	k=0;

	while (!(k & KEY_ESCAPE)&&(ritorno==0))
	{k = menu_wait_key_press();}

	fclose(fpoke);
	if (ritorno==0) strcpy(ordenador.last_selected_poke_file,filename);		
	return (ritorno);
}

static int load_poke_file()
{
	char *dir = load_path_poke;
	int ritorno, retorno2;
	ritorno=0;
	retorno2=0; //Stay in menu as default
	
	const char *filename = menu_select_file(dir, NULL,0);
		
	if (!filename) return 0;

	if (ext_matches(filename, ".pok")|ext_matches(filename, ".POK"))
	ritorno = parse_poke(filename); else return 0;
	
	switch(ritorno)
	{
	case -1: //can not access the file
	retorno2=0;
	break;
	case 0: //OK
	retorno2=-2; //come back to emulator
	break;
	case 1:
	msgInfo("Not compatible file",3000,NULL);
	retorno2=0;
	break;
	case 2:
	msgInfo("Too many trainers",3000,NULL);
	retorno2=0;
	break;
	}
				
	free((void*)filename);
	
	return retorno2;
	
}

static void help(void)
{
	menu_select_title("FBZX-WII help",
			help_messages, NULL);
}

static int manage_scr(int which)
{
int retorno = 0; //Stay in menu as default
switch (which) 
		{
		case 0: // Save SCR 1
			retorno=save_scr(1);
			break;
		case 1: // Save SCR 2
			retorno=save_scr(2);
			break;	
		case 2: // Load SCR 
			retorno=load_scr();
			break;
		case 3: // Delete scr
			delete_scr();
			retorno=0;
			break;
		default:
			break;
		}		
 return retorno;
}

static int tools()
{
	int opt , retorno;
	int submenus[4], old_port;

	memset(submenus, 0, sizeof(submenus));

	do {
	retorno=-1; //Exit from menu as default
 
	submenus[1] = ordenador.port;
	submenus[2] = !ordenador.vk_auto;
	submenus[3] = !ordenador.vk_rumble;
	
	old_port=ordenador.port;
	
	opt = menu_select_title("Tools menu",
			tools_messages, submenus);
	if (opt < 0)
		return 0;
		
	if (old_port!= submenus[1]) set_port(submenus[1]);
	ordenador.vk_auto = !submenus[2];
	ordenador.vk_rumble = !submenus[3];
	
	switch(opt)
		{
		case 0: 
			retorno = manage_scr(submenus[0]);
			break;
		case 9: // Load poke file
			retorno = load_poke_file();
			break;
		case 11: // Insert poke
			retorno = do_poke_sdl();
			break;	
		case 13:
			help();
			retorno = -1;
			break;
		default:
			break;
		}
	} while (!retorno);
	return retorno;
}


void virtual_keyboard(void)
{
	int key_code;
	
	VirtualKeyboard.sel_x = 64;
	VirtualKeyboard.sel_y = 90;
	
	virtkey_t *key =get_key();  
	if (key) {key_code = key->sdl_code;} else return;
	
	ordenador.kbd_buffer_pointer=1;
	countdown_buffer=8;
	ordenador.keyboard_buffer[0][1]= key_code;
	if 	(key->caps_on) ordenador.keyboard_buffer[1][1]= SDLK_LSHIFT; 
	else if (key->sym_on) ordenador.keyboard_buffer[1][1]= SDLK_LCTRL; 
	else ordenador.keyboard_buffer[1][1]= 0;
	
	printf ("Push Event: keycode %d\n", key_code);

}	

static int save_load_snapshot(int which)
{
	char *dir = path_snaps;
	char *dir_load = load_path_snaps;
	const char *tape = ordenador.last_selected_file;
	char *ptr;
	char db[MAX_PATH_LENGTH];
	char fb[81];
	int retorno, retorno2;
	
	retorno2 = 0; //Stay in menu as default

	// Name (for saves) - TO CHECK
	if (tape && strrchr(tape, '/'))
		strncpy(fb, strrchr(tape, '/') + 1, 80);
	else
		strcpy(fb, "unknown");
		
	//remove the extension
	ptr = strrchr (fb, '.');
		if (ptr) *ptr = 0;	

	switch(which)
	{
	case 2:
	case 0: // Load or delete file
	{
		const char *filename = menu_select_file(dir_load, NULL,1);

		if (!filename)
			return 0;

		if (ext_matches(filename, ".z80")|ext_matches(filename, ".Z80")|
		ext_matches(filename, ".sna")|ext_matches(filename, ".SNA"))
		{
			if (which == 0) // Load snapshot file
			{
				retorno=load_z80((char *)filename);

				switch(retorno) {
				case 0: // all right
				strcpy(ordenador.last_selected_file,filename);
				if (ordenador.autoconf) maybe_load_conf(filename);
				retorno2=-1;
				break;
				case -1:
				msgInfo("Error: Can't load that file",3000,NULL);
				break;
				case -2:
				case -3:
				msgInfo("Error: unsupported snap file",3000,NULL);
				break;
				}
			}
			else // Delete snashot file
				if (msgYesNo("Delete the file?", 0, FULL_DISPLAY_X /2-138/RATIO, FULL_DISPLAY_Y /2-48/RATIO)) unlink(filename);
		}	
		free((void*)filename);
	} break;
	case 1: // Save snapshot file
		snprintf(db, MAX_PATH_LENGTH-1, "%s/%s.z80", dir, fb);
		
		retorno=save_z80(db,0);
		switch(retorno) 
			{
			case 0: //OK
				msgInfo("Snapshot saved",3000,NULL);
				retorno2=-1;
				break;
			case -1:
				if (msgYesNo("Overwrite the exiting file?", 0, FULL_DISPLAY_X /2-160/RATIO, FULL_DISPLAY_Y /2-48/RATIO))
				{
					save_z80(db,1); //force overwrite
					msgInfo("Snapshot saved",3000,NULL);
					retorno2=-1;
				}
				break;
			case -2:
				msgInfo("Can't create file",3000,NULL);
				break;
			}
		break;
	default:
		break;
	}
	
	return retorno2;
}

static int save_load_game_configurations(int which)
{
	char *dir = path_confs;
	const char *tape = ordenador.last_selected_file;
	char *ptr;
	char db[MAX_PATH_LENGTH];
	char fb[81];
	int retorno, retorno2;
	
	retorno2 = 0; //stay in menu as default
	
	switch(which)
	{
	case 2:
	case 0: // Load or delete file
	{
		const char *filename = menu_select_file(dir, NULL,0);
		
		if (!filename)
			return 0;

		if (ext_matches(filename, ".conf")|ext_matches(filename, ".CONF"))
		{
			if (which == 0) // Load config file
			{
				if (!load_config(&ordenador,(char *)filename)) {msgInfo("Game confs loaded",3000,NULL);retorno2=-1;}
				break;
			}
			else // Delete config file
				if (msgYesNo("Delete the file?", 0, FULL_DISPLAY_X /2-138/RATIO, FULL_DISPLAY_Y /2-48/RATIO)) unlink(filename);
		}	
		free((void*)filename);
	} break;
	case 1: // Save configuration file
		
		// Name (for game config saves) - TO CHECK
		if (tape && strrchr(tape, '/'))
			strncpy(fb, strrchr(tape, '/') + 1, 80);
		else
			{
			msgInfo("No file selected",3000,NULL);
			return 0;
			}
		
		//remove the extension
		ptr = strrchr (fb, '.');
		
		if (ptr) *ptr = 0;	
	
		snprintf(db, MAX_PATH_LENGTH-1, "%s/%s.conf", dir, fb);
	
		retorno=save_config_game(&ordenador,db,0);
		
		switch(retorno) 
			{
			case 0: //OK
				msgInfo("Game confs saved",3000,NULL);
				retorno2=-1;
				break;
			case -1:
				if (msgYesNo("Overwrite the exiting file?", 0, FULL_DISPLAY_X /2-160/RATIO, FULL_DISPLAY_Y /2-48/RATIO))
				{
					save_config_game(&ordenador,db,1); //force overwrite
					msgInfo("Game confs saved",3000,NULL);
					retorno2=-1;
				}
				break;
			case -2:
				msgInfo("Can't create file",3000,NULL);
				break;
			}
		break;
	default:
		break;
	}
	return retorno2;
}
static void save_load_general_configurations(int which)
{

	int retorno;
	unsigned char old_bw,old_mode;
	char config_path[MAX_PATH_LENGTH];
	int length;
	FILE *fconfig; 
	
	strcpy(config_path,getenv("HOME"));
	length=strlen(config_path);
	if ((length>0)&&(config_path[length-1]!='/'))
		strcat(config_path,"/");
	strcat(config_path,"fbzx.conf");
	
	switch(which)
	{
	case 2:
	case 0: // Load or delete file
	{
		fconfig = fopen(config_path,"r");
		if (fconfig==NULL) 
			{
			msgInfo("Can not access the file",3000,NULL);
			return;
			}
		else fclose(fconfig);
		
			if (which == 0) // Load config file
			{
				old_bw = ordenador.bw;
				old_mode= ordenador.mode128k;
				if (!load_config(&ordenador,config_path)) msgInfo("General confs loaded",3000,NULL);
				if (old_bw!=ordenador.bw) computer_set_palete();
				if (old_mode != ordenador.mode128k) ResetComputer();
				break;
			}
			else // Delete config file
				if (msgYesNo("Delete the file?", 0, FULL_DISPLAY_X /2-138/RATIO, FULL_DISPLAY_Y /2-48/RATIO)) unlink(config_path);
		
	} break;
	case 1: // Save configuration file
		retorno=save_config(&ordenador,config_path);
		
		switch(retorno) 
			{
			case 0: //OK
				msgInfo("General confs saved",3000,NULL);
				break;
			case -2:
				msgInfo("Can't create file",3000,NULL);
				break;
			}
		break;
	default:
		break;
	}
}
static void manage_configurations()
{
	int opt , retorno;
	int submenus[4];
	

	memset(submenus, 0, sizeof(submenus));
	do {
	retorno = -1; //Exit from menu as default
	
	submenus[2]=!ordenador.autoconf;
	submenus[3]=!ordenador.ignore_z80_joy_conf;

	opt = menu_select_title("Configurations file menu",
			confs_messages, submenus);
	if (opt < 0)
		return;
		
	ordenador.autoconf=!submenus[2];
	ordenador.ignore_z80_joy_conf=!submenus[3];
	
	switch(opt)
		{
		case 0: // Save, load and delete general configurations 
			save_load_general_configurations(submenus[0]);
			retorno=-1;
			break;
		case 3: // Save, load and delete game configurations
			retorno = save_load_game_configurations(submenus[1]);
			break;
		default:
			break;
		}
	} while (!retorno);		
}


void main_menu()
{
	int submenus[3];
	int opt;
	int retorno;
	
	memset(submenus, 0, sizeof(submenus));

	do
	{
	retorno=0; //stay in menu as default
		opt = menu_select_title("Main menu", main_menu_messages, submenus);
		if (opt < 0)
			break;

		switch(opt)
		{
		case 0:
			retorno = manage_tape(submenus[0]);
			break;
		case 2:
			retorno = save_load_snapshot(submenus[1]);
			break;
		case 5:
			input_options(submenus[2]);
			break;
		case 7:
			if (emulation_settings()==-2) retorno=-1;
			break;
		case 8:
			screen_settings();
			break;
		case 9:
			audio_settings();
			break;	
		case 10:
			manage_configurations();
			break;
		case 11:
			microdrive();
			break;	
		case 12:
			if (tools()==-2) retorno=-1;
			break;
		case 13:
			ResetComputer ();
			retorno=-1;
			break;	
		case 14:
			if (msgYesNo("Are you sure to quit?", 0, FULL_DISPLAY_X /2-138/RATIO, FULL_DISPLAY_Y /2-48/RATIO)) 
				{salir = 0;retorno=-1;}	
			break;
		default:
			break;
		}
	} while (!retorno);
	
	clean_screen();
	
}
