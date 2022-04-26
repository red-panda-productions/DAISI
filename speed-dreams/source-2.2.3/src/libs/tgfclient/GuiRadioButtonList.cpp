#include <cstdlib>

#include "tgfclient.h"
#include "gui.h"

/// @brief Initializes the radio button list
void GfuiRadioButtonListInit() {}

/// @brief                   Creates a RadioButtonList object
/// @param p_scr             The current screen
/// @param p_font            The text font
/// @param p_x               The radio-button-list x-coordinate
/// @param p_y               The radio-button-list y-coordinate
/// @param p_imageWidth      The image width
/// @param p_imageHeight     The image height
/// @param p_pszText         The text
/// @param p_selected        The id of the radio button selected (-1 for none)
/// @param p_amount          The amount of radio buttons to create
/// @param p_distance        The distance between radio buttons
/// @param p_minimumOfOne    If at least one radiobutton has to be selected at all times
/// @param p_userData        The userData
/// @param p_onChange        The function to call when a radiobutton is clicked
/// @param p_userDataOnFocus The userData when the mouse is hovering above the object
/// @param p_onFocus         The function to call when the mouse is hovering above the object
/// @param p_onFocusLost     The function to call when the mouse stops hovering above the object
/// @return                  The radioButtonList object id
int GfuiRadioButtonListCreate(void* p_scr, int p_font, int p_x, int p_y, int p_imageWidth, int p_imageHeight,
                              const char** p_pszText, int p_selected, int p_amount, int p_distance, bool p_minimumOfOne,
                              void* p_userData, tfuiRadioButtonCallback p_onChange,
                              void** p_userDataOnFocus, tfuiCallback* p_onFocus, tfuiCallback* p_onFocusLost)
{
    tGfuiRadioButtonList* radioButtonList;
    tGfuiObject* object;
    tGfuiScreen* screen = (tGfuiScreen*)p_scr;

    object = (tGfuiObject*)calloc(1, sizeof(tGfuiObject));
    object->widget = GFUI_RADIOBUTTONLIST;
    object->focusMode = GFUI_FOCUS_NONE;
    object->id = screen->curId++;
    object->visible = 1;

    radioButtonList = &(object->u.radiobuttonlist);
    radioButtonList->Scr = p_scr;
    radioButtonList->ButtonControls = new int[p_amount];

    radioButtonList->Info = new tRadioButtonListInfo;
    radioButtonList->Info->UserData = p_userData;
    radioButtonList->Info->Selected = p_selected;
    radioButtonList->Info->Amount = p_amount;
    radioButtonList->Info->Dist = p_distance;
    radioButtonList->Info->MinimumOfOne = p_minimumOfOne;

    // Initialize the radiobutton children
    for (int i = 0; i < radioButtonList->Info->Amount; i++)
    {
        int yPos = p_y - (p_imageHeight + radioButtonList->Info->Dist) * i;
        radioButtonList->ButtonControls[i] = GfuiRadioButtonCreate(p_scr, p_font, p_x, yPos, p_imageWidth, p_imageHeight,
                                                                   p_pszText[i], p_selected, i, object->id, p_userData, p_onChange,
                                                                   p_userDataOnFocus[i], p_onFocus[i], p_onFocusLost[i]);
    }

    gfuiAddObject(screen, object);

    GfuiRadioButtonListSetSelected(p_scr, object->id, p_selected);

    return object->id;
}

/// @brief       Draws the radio buttons on screen
/// @param p_obj The RadioButtonList object
void GfuiDrawRadioButtonList(tGfuiObject* p_obj)
{
    // Do nothing because children already draw themselves
}

/// @brief         Sets the label texts of the radiobutton children to p_texts
/// @param p_scr   The current screen
/// @param p_id    The radiobutton-list object id
/// @param p_texts A p_text for every radio button
void GfuiRadioButtonListSetText(void* p_scr, int p_id, const char** p_texts)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTONLIST)
        return;

    tGfuiRadioButtonList* radioButtonList = &(object->u.radiobuttonlist);

    for (int i = 0; i < radioButtonList->Info->Amount; i++)
    {
        GfuiRadioButtonSetText(p_scr, radioButtonList->ButtonControls[i], p_texts[i]);
    }
}

/// @brief            Selects the radio button
/// @param p_scr      The current screen
/// @param p_id       The RadioButtonList object id
/// @param p_selected The radiobutton that is being selected (-1 for unselecting all)
void GfuiRadioButtonListSetSelected(void* p_scr, int p_id, int p_selected)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTONLIST)
        return;

    tGfuiRadioButtonList* radioButtonList = &(object->u.radiobuttonlist);

    // if at least one radiobutton has to be selected and p_selected is -1, return
    if (radioButtonList->Info->MinimumOfOne && (p_selected < 0))
        return;

    radioButtonList->Info->Selected = p_selected;

    for (int i = 0; i < radioButtonList->Info->Amount; i++)
    {
        tGfuiObject* obj = gfuiGetObject(p_scr, radioButtonList->ButtonControls[i]);
        tGfuiRadioButton* radioButton = &(obj->u.radiobutton);

        GfuiRadioButtonSelect(p_scr, radioButtonList->ButtonControls[i], radioButton->NrInList == p_selected);
    }
}

/// @brief       Return the radiobutton selected value
/// @param p_scr The current screen
/// @param p_id  The RadioButton object id
/// @return      The radiobutton selected value
int GfuiRadioButtonListGetSelected(void* p_scr, int p_id)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTONLIST)
        return -1;

    tGfuiRadioButtonList* radioButtonList = &(object->u.radiobuttonlist);
    return radioButtonList->Info->Selected;
}

/// @brief         Sets the text color of all radiobutton children to p_color
/// @param p_scr   The current screen
/// @param p_id    The RadioButtonList object id
/// @param p_color The color
void GfuiRadioButtonListSetTextColor(void* p_scr, int p_id, const GfuiColor& p_color)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTONLIST)
        return;

    tGfuiRadioButtonList* radioButtonList = &(object->u.radiobuttonlist);

    for (int i = 0; i < radioButtonList->Info->Amount; i++)
    {
        GfuiRadioButtonSetTextColor(p_scr, radioButtonList->ButtonControls[i], p_color);
    }
}

/// @brief       Frees the pointer to the RadioButtonList object
/// @param p_obj The RadioButtonList object
void GfuiReleaseRadioButtonList(tGfuiObject* p_obj)
{
    tGfuiRadioButtonList* radioButtonList = &(p_obj->u.radiobuttonlist);

    for (int i = 0; i < radioButtonList->Info->Amount; i++)
    {
        tGfuiObject* radioButton = gfuiGetObject(GfuiScreen, radioButtonList->ButtonControls[i]);
        GfuiReleaseRadioButton(radioButton);
    }
    delete[] radioButtonList->ButtonControls;

    free(p_obj);
}
