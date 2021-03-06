
/*******************************************************************************/
/* Copyright (C) 2008 Jonathan Moore Liles                                     */
/* Copyright (C) 2012 Aurélien Roux (OSC part)                                 */
/*                                                                             */
/* This program is free software; you can redistribute it and/or modify it     */
/* under the terms of the GNU General Public License as published by the       */
/* Free Software Foundation; either version 2 of the License, or (at your      */
/* option) any later version.                                                  */
/*                                                                             */
/* This program is distributed in the hope that it will be useful, but WITHOUT */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       */
/* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for   */
/* more details.                                                               */
/*                                                                             */
/* You should have received a copy of the GNU General Public License along     */
/* with This program; see the file COPYING.  If not,write to the Free Software */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */
/*******************************************************************************/

#include "const.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_Pack.H>
#include <FL/Boxtypes.H>
#include "Thread.H"
#include "debug.h"
#include <getopt.h>

#include "Mixer.H"
#include "Project.H"

#include "Loggable.H"

#include "OSC_Server.H"

/* for registration */
#include "Module.H"
#include "Gain_Module.H"
#include "Plugin_Module.H"
#include "JACK_Module.H"
#include "Meter_Module.H"
#include "Meter_Indicator_Module.H"
#include "Controller_Module.H"
#include "Mono_Pan_Module.H"
#include "Chain.H"
#include "Mixer_Strip.H"


/* TODO: put these in a header */
#define USER_CONFIG_DIR ".non-mixer/"

char *user_config_dir;
Mixer *mixer;

const char *instance_name;
int listen_port;

#include <errno.h>

static int
ensure_dirs ( void )
{
    asprintf( &user_config_dir, "%s/%s", getenv( "HOME" ), USER_CONFIG_DIR );

    int r = mkdir( user_config_dir, 0777 );

    return r == 0 || errno == EEXIST;
}

#include <signal.h>

static void cb_main ( Fl_Double_Window *o, void *)
{
    if ( Fl::event_key() != FL_Escape )
        o->hide();
}


int
main ( int argc, char **argv )
{
    Thread::init();

    Thread thread( "UI" );
    thread.set();

    ensure_dirs();

    Fl_Tooltip::color( FL_BLACK );
    Fl_Tooltip::textcolor( FL_YELLOW );
    Fl_Tooltip::size( 14 );
    Fl_Tooltip::hoverdelay( 0.1f );

    Fl::visible_focus( 0 );

    fl_register_images();

    LOG_REGISTER_CREATE( Mixer_Strip );
    LOG_REGISTER_CREATE( Chain );
    LOG_REGISTER_CREATE( Plugin_Module );
    LOG_REGISTER_CREATE( Gain_Module );
    LOG_REGISTER_CREATE( Meter_Module );
    LOG_REGISTER_CREATE( JACK_Module );
    LOG_REGISTER_CREATE( Mono_Pan_Module );
    LOG_REGISTER_CREATE( Meter_Indicator_Module );
    LOG_REGISTER_CREATE( Controller_Module );

    init_boxtypes();

    signal( SIGPIPE, SIG_IGN );

    Fl::get_system_colors();
    Fl::scheme( "plastic" );

    Plugin_Module::spawn_discover_thread();

    Fl_Double_Window *main_window;

    {
        Fl_Double_Window *o = main_window = new Fl_Double_Window( 800, 600, "Non-DAW : Mixer" );
        {
            main_window->xclass( APP_NAME );

            Fl_Widget *o = mixer = new Mixer( 0, 0, main_window->w(), main_window->h(), NULL );
            Fl_Group::current()->resizable(o);
        }
        o->end();

        o->size_range( main_window->w(), mixer->min_h(), 0, 0 );

        o->callback( (Fl_Callback*)cb_main, main_window );
        o->show( argc, argv );
    }

    {
      int c;

      while(1)
      {
	static struct option long_options[] = 
	{
	  {"instance", required_argument, 0, 'i'},
	  {"listen_port", required_argument, 0, 'l'},
	  //	  {"control_port", required_argument, 0, 'c'},
	  {0, 0, 0, 0}
	};
	
	int option_index;

	c = getopt_long (argc, argv, "i:l:c:", long_options, &option_index);

	if ( c == -1)
	  break;

	switch( c )
	{
	case 'i':
	  MESSAGE( "Using instance name \"%s\"", optarg );
	  instance_name = optarg;
	  break;
	case 'l':
	  MESSAGE("Using OSC listen port %s", optarg);
	  mixer->set_listen_port( optarg );
	  mixer->osc_server_start();
	  
	  //TODO : utiliser la classe OSC_Server	  OSC_Server *s = oserver = new OSC_Server( optarg );
	  break;
	  //	case 'c':
	  //MESSAGE("Using OSC contol port %s", optarg);
	  //break;
	case '?':
	  printf("erreur dans les options\n");
	}

      }

      if ( optind < argc )
      {
	if( argc - optind < 2 )
	{
	  MESSAGE( "Loading \"%s\"", argv[optind] );
	  
	  if ( ! mixer->command_load( argv[optind] ) )
	  {
	    fl_alert( "Error opening project specified on commandline" );
	  }
	}
	else
	{
	  fl_alert( "Invalid project name on commandline" );
	}
      }
    }

    Fl::run();

    delete main_window;
    main_window = NULL;

    MESSAGE( "Your fun is over" );
}
