#pragma once
#include <wx/window.h>
#include <wx/timer.h>
#include "uirect.h"

class Progress : public wxWindow
{
    DECLARE_EVENT_TABLE()
    wxDECLARE_DYNAMIC_CLASS(Progress);
    public:

        /** @brief default constructor
        **/
        Progress();

        /** @brief Constructor - made to be the same as a wxButton
        *   @param parent pointer to the parent window
        *   @param id the wxWindowId of the button
        *   @param pos the top-left point of the button
        *   @param size the width and height of the button
        *   @param style the button style: wmButton::STYLE_NORMAL, wmButton::STYLE_SELECT, wmButton::STYLE_REPEAT, wmButton::STYLE_HOLD
        *   @param validator not used - just here to have same structure as wxButton
        *   @param name not used - just here to have same structure as wxButton
        **/
        Progress(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

        bool Create(wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

        virtual ~Progress();

        /** @brief Sets the background colour of the button for all button states
        *   @param colour
        *   @return <i>bool</i> true
        **/
        bool SetBackgroundColour(const wxColour &colour) override;

        /** @brief Sets the text colour of the button for all button states
        *   @param colour
        *   @return <i>bool</i> true
        **/
        bool SetForegroundColour(const wxColour &colour) override;

        /** @brief Set the border state of the button
        *   @param
        **/
        void SetBorderState(unsigned short nState);

        /** @brief returns the default size of the button for sizers
        **/
        wxSize DoGetBestSize() const
        {
            return wxSize(200,40);
        }

        void SetRange(unsigned int nMax);

        void SetValue(unsigned int nValue);

  protected:

        /** Called to draw the console
        *   @param event
        **/
        void OnPaint(wxPaintEvent& event);

        /** Called when the console resizes
        *   @param event
        **/
        virtual void OnSize(wxSizeEvent& event);

        ///< @brief Creates the rectangles for the button
        void CreateRect();

        void CalculateResolution();

        unsigned int m_nMax;
        unsigned int m_nValue;
        double m_dResolution;
        double m_dValue;
        uiRect m_uiRect;            ///< @brief the internal uiRect
        unsigned int m_nBorder;

};

