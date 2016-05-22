// -------------------------------------------------------------------------------- //
//	Copyright (C) 2008-2016 J.Rios anonbeat@gmail.com
//
//    This Program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 3, or (at your option)
//    any later version.
//
//    This Program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; see the file LICENSE.  If not, write to
//    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//    http://www.gnu.org/copyleft/gpl.html
//
// -------------------------------------------------------------------------------- //
#include "AutoScrollText.h"

#include "Utils.h"

#include <wx/dcclient.h>
#include <wx/region.h>

#define guSCROLL_START_TIMEOUT  500
#define guSCROLL_TIMEOUT        500
#define guSCROLL_STEP_WIDTH     16

#define guSCROLL_TIMER_START    11
#define guSCROLL_TIMER_SCROLL   12

BEGIN_EVENT_TABLE( guAutoScrollText, wxControl )
    EVT_MOUSE_EVENTS( guAutoScrollText::OnMouseEvents )
END_EVENT_TABLE()

// -------------------------------------------------------------------------------- //
guAutoScrollText::guAutoScrollText( wxWindow * parent, const wxString &label, const wxSize &size ) :
    wxControl( parent, wxID_ANY, wxDefaultPosition, size, wxBORDER_NONE ),
    m_StartTimer( this, guSCROLL_TIMER_START ),
    m_ScrollTimer( this, guSCROLL_TIMER_SCROLL )
{
    m_ScrollPos = 0;
    m_ScrollQuantum = 1;
    m_VisWidth = 0;
    m_AllowScroll = false;
    m_DefaultSize = size;

    SetLabel( label );

    Connect( wxEVT_PAINT, wxPaintEventHandler( guAutoScrollText::OnPaint ), NULL, this );
    Connect( guSCROLL_TIMER_START, wxEVT_TIMER, wxTimerEventHandler( guAutoScrollText::OnStartTimer ), NULL, this );
    Connect( guSCROLL_TIMER_SCROLL, wxEVT_TIMER, wxTimerEventHandler( guAutoScrollText::OnScrollTimer ), NULL, this );
    Connect( wxEVT_SIZE, wxSizeEventHandler( guAutoScrollText::OnSize ), NULL, this );
}

// -------------------------------------------------------------------------------- //
guAutoScrollText::~guAutoScrollText()
{
    Disconnect( wxEVT_PAINT, wxPaintEventHandler( guAutoScrollText::OnPaint ), NULL, this );
    Disconnect( guSCROLL_TIMER_START, wxEVT_TIMER, wxTimerEventHandler( guAutoScrollText::OnStartTimer ), NULL, this );
    Disconnect( guSCROLL_TIMER_SCROLL, wxEVT_TIMER, wxTimerEventHandler( guAutoScrollText::OnScrollTimer ), NULL, this );
    Disconnect( wxEVT_SIZE, wxSizeEventHandler( guAutoScrollText::OnSize ), NULL, this );
}

// -------------------------------------------------------------------------------- //
wxSize guAutoScrollText::DoGetBestSize() const
{
    wxSize RetVal;
    RetVal.x = ( m_DefaultSize.x != wxNOT_FOUND ) ? wxMin( m_DefaultSize.x, m_LabelExtent.x ) : m_LabelExtent.x;
    RetVal.y = ( m_DefaultSize.y != wxNOT_FOUND ) ? wxMin( m_DefaultSize.y, m_LabelExtent.y ) : m_LabelExtent.y;
    return RetVal;
}

// -------------------------------------------------------------------------------- //
void guAutoScrollText::SetLabel( const wxString &label )
{
    m_Label = label;
    GetTextExtent( label, &m_LabelExtent.x, &m_LabelExtent.y );
    m_ScrollPos = 0;
    m_ScrollQuantum = 1;
    //guLogMessage( wxT( "LabelExtent: %u Width: %u  %i" ), m_LabelExtent.x, m_VisWidth, m_ScrollTimer.IsRunning() );
    if( m_ScrollTimer.IsRunning() && !( m_LabelExtent.x > m_VisWidth ) )
    {
        m_ScrollTimer.Stop();
    }
    Refresh();
    Update();
}

// -------------------------------------------------------------------------------- //
void guAutoScrollText::OnPaint( wxPaintEvent &event )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextForeground( GetForegroundColour() );
    dc.SetFont( GetFont() );
    dc.DrawText( m_Label.Mid( m_ScrollPos ), 0, 0 );
}

// -------------------------------------------------------------------------------- //
void guAutoScrollText::OnSize( wxSizeEvent &event )
{
    wxRect Size = event.GetSize();
    //guLogMessage( wxT( "Size Event: %i, %i" ), Size.GetWidth(), Size.GetHeight() );
    m_VisWidth = Size.GetWidth();
    event.Skip();
}
// -------------------------------------------------------------------------------- //
void guAutoScrollText::OnMouseEvents( wxMouseEvent &event )
{
    if( event.Moving() && ( m_LabelExtent.x > m_VisWidth ) )
    {
        if( !m_ScrollTimer.IsRunning() )
        {
            // No need to stop the timer first anymore
            //if( m_StartTimer.IsRunning() )
            //    m_StartTimer.Stop();
            m_StartTimer.Start( guSCROLL_START_TIMEOUT, wxTIMER_ONE_SHOT );
        }
    }
    event.Skip();
}

// -------------------------------------------------------------------------------- //
bool inline MouseIsOver( const wxRect &rect )
{
    int MouseX, MouseY;
    wxGetMousePosition( &MouseX, &MouseY );
    return rect.Contains( MouseX, MouseY );
}

// -------------------------------------------------------------------------------- //
void guAutoScrollText::OnStartTimer( wxTimerEvent &event )
{
    if( MouseIsOver( GetScreenRect() ) )
    {
        m_ScrollTimer.Start( guSCROLL_TIMEOUT, wxTIMER_CONTINUOUS );
    }
}

// -------------------------------------------------------------------------------- //
void guAutoScrollText::OnScrollTimer( wxTimerEvent &event )
{
    if( !MouseIsOver( GetScreenRect() ) )
    {
        if( m_ScrollQuantum > 0 )
            m_ScrollQuantum = -1;
        if( !m_ScrollPos )
        {
            m_ScrollTimer.Stop();
            return;
        }

    }
    else if( m_ScrollQuantum > 0 )
    {
        int ActualWidth;
        int ActualHeight;
        GetTextExtent( m_Label.Mid( m_ScrollPos ), &ActualWidth, &ActualHeight );
        if( ( ActualWidth + guSCROLL_STEP_WIDTH ) < m_VisWidth )
        {
            m_ScrollQuantum = -1;
        }
    }
    else
    {
        if( !m_ScrollPos )
        {
            m_ScrollQuantum = 1;
        }
    }
    m_ScrollPos += m_ScrollQuantum;
    //guLogMessage( wxT( "Actual ScrollPos: %i  ScrollQ: %i" ), m_ScrollPos, m_ScrollQuantum );
    Refresh();
}

// -------------------------------------------------------------------------------- //
