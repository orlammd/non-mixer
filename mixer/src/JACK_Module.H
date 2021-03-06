
/*******************************************************************************/
/* Copyright (C) 2009 Jonathan Moore Liles                                     */
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

#pragma once


#include "Module.H"
#include "JACK/Port.H"
#include <vector>

class JACK_Module : public Module
{
    std::vector<JACK::Port> jack_input;
    std::vector<JACK::Port> jack_output;

    static void jack_port_activation_error ( JACK::Port *p );

public:

    JACK_Module ( );
    virtual ~JACK_Module ( );

    const char *name ( void ) const { return "JACK"; }

    bool initialize ( void );

    int can_support_inputs ( int );
    bool configure_inputs ( int n );
    bool configure_outputs ( int n );

    void add_output ( void );

    void handle_control_changed ( Port *p );
    void handle_chain_name_changed ();

    LOG_CREATE_FUNC( JACK_Module );

protected:

    virtual void process ( nframes_t nframes );

};
