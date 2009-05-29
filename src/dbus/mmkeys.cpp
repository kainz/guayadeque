// -------------------------------------------------------------------------------- //
//	Copyright (C) 2008-2009 J.Rios
//	anonbeat@gmail.com
//
//    This Program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2, or (at your option)
//    any later version.
//
//    This Program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; see the file LICENSE.  If not, write to
//    the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//    http://www.gnu.org/copyleft/gpl.html
//
// -------------------------------------------------------------------------------- //
#include "mmkeys.h"

#include "Commands.h"
#include "Utils.h"

// -------------------------------------------------------------------------------- //
guMMKeys::guMMKeys( guPlayerPanel * playerpanel ) : guDBus( NULL, false )
{
    m_PlayerPanel = playerpanel;

    //RegisterObjectPath( "org/gnome/SettingsDaemon" );
    AddMatch( "type='signal',interface='org.gnome.SettingsDaemon'" );
    //AddMatch( "type='signal',interface='org.gnome.SettingsDaemon.MediaKeys'" );
}

// -------------------------------------------------------------------------------- //
guMMKeys::~guMMKeys()
{
    //UnRegisterObjectPath( "org/gnome/SettingsDaemon" );
}

// -------------------------------------------------------------------------------- //
DBusHandlerResult guMMKeys::HandleMessages( guDBusMessage * msg, guDBusMessage * reply )
{
    wxASSERT( msg );

    if( !msg )
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    // Show the details of the msg
    printf( "Type   : %i\n", msg->GetType() );
    printf( "Iface  : %s\n", msg->GetInterface() );
    printf( "Path   : %s\n", msg->GetPath() );
    printf( "Member : %s\n", msg->GetMember() );
    printf( "Sender : %s\n", msg->GetSender() );
    printf( "Reply  : %i\n", msg->NeedReply() );
    printf( "Serial : %i\n", msg->GetSerial() );
    if( reply )
        printf( "RSerial: %i\n", reply->GetReplySerial() );
    printf( "==MMKEYS=========================\n" );

    DBusHandlerResult RetVal = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    const char *    Interface = msg->GetInterface();
    const char *    Member = msg->GetMember();
    int             Type = msg->GetType();
    const char *    Path = msg->GetPath();

    // If its a Signal message
    if( Type == DBUS_MESSAGE_TYPE_SIGNAL )
    {
        // INTROSPECT
        if( !strcmp( Path, "/org/gnome/SettingsDaemon" ) ||
            !strcmp( Path, "/org/gnome/SettingsDaemon/MediaKeys" ) )
        {
            if( !strcmp( Member, "MediaPlayerKeyPressed" ) )
            {
                DBusError error;
                dbus_error_init( &error );

                const char * s = NULL;
                const char * KeyName = NULL;

                dbus_message_get_args( msg->GetMessage(), &error,
                      DBUS_TYPE_STRING, &s,
                      DBUS_TYPE_STRING, &KeyName,
                      DBUS_TYPE_INVALID );

                if( dbus_error_is_set( &error ) )
                {
                    printf( "Could not read the MediaPlayerKeyPressed parameters : %s\n", error.message );
                    dbus_error_free( &error );
                    //RetVal =  DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
                }
                else
                {
                    if( !strcmp( KeyName, "Play" ) )
                    {
                        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PLAYERPANEL_PLAY );
                        wxPostEvent( m_PlayerPanel, event );
                        RetVal = DBUS_HANDLER_RESULT_HANDLED;
                    }
                    else if( !strcmp( KeyName, "Stop" ) )
                    {
                        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PLAYERPANEL_STOP );
                        wxPostEvent( m_PlayerPanel, event );
                        RetVal = DBUS_HANDLER_RESULT_HANDLED;
                    }
                    else if( !strcmp( KeyName, "Previous" ) )
                    {
                        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PLAYERPANEL_PREVTRACK );
                        wxPostEvent( m_PlayerPanel, event );
                        RetVal = DBUS_HANDLER_RESULT_HANDLED;
                    }
                    else if( !strcmp( KeyName, "Next" ) )
                    {
                        wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PLAYERPANEL_NEXTTRACK );
                        wxPostEvent( m_PlayerPanel, event );
                        RetVal = DBUS_HANDLER_RESULT_HANDLED;
                    }
                    else if( !strcmp( KeyName, "Pause" ) )
                    {
                        if( m_PlayerPanel->GetState() == wxMEDIASTATE_PLAYING )
                        {
                            wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_PLAYERPANEL_PLAY );
                            wxPostEvent( m_PlayerPanel, event );
                        }
                        RetVal = DBUS_HANDLER_RESULT_HANDLED;
                    }
                }
            }
        }
    }

    // Call the inherited default processing which destroys the msgs
    guDBus::HandleMessages( msg, reply );

    return RetVal;
}

// -------------------------------------------------------------------------------- //
