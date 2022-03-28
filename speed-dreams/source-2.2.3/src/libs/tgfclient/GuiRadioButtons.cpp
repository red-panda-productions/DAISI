#include <cstdlib>
#include <cstring>

#include "tgfclient.h"

#include "gui.h"
#include "guifont.h"


/// @brief Initializes the radio buttons
void GfuiRadioButtonsInit(void) { }


/// @brief       Function to call when the first radio button is clicked
/// @param p_idv The id-value of the radio button
static void OnPushFirst(void* p_idv)
{
    GfuiRadioButtonsSetSelected(GfuiScreen, (long)p_idv, 0);
    GfuiUnSelectCurrent();

    tGfuiObject* object = gfuiGetObject(GfuiScreen, (long)p_idv);
    if (!object)
        return;

    tGfuiRadioButtons* radioButtons = &(object->u.radiobuttons);

    if (radioButtons->onChange)
        radioButtons->onChange(radioButtons->pInfo);
}
/// @brief       Function to call when the second radio button is clicked
/// @param p_idv The id-value of the radio button
static void OnPushSecond(void* p_idv)
{
    GfuiRadioButtonsSetSelected(GfuiScreen, (long)p_idv, 1);
    GfuiUnSelectCurrent();

    tGfuiObject* object = gfuiGetObject(GfuiScreen, (long)p_idv);
    if (!object)
        return;

    tGfuiRadioButtons* radioButtons = &(object->u.radiobuttons);

    if (radioButtons->onChange)
        radioButtons->onChange(radioButtons->pInfo);
}


/// @brief                   Creates a RadioButtons object
/// @param p_scr             The screen information
/// @param p_font            The font
/// @param p_x               The button x-coordinate
/// @param p_y               The button y-coordinate
/// @param p_imageWidth      The image width
/// @param p_imageHeight     The image height
/// @param p_pszText         The text
/// @param p_selected        The id of the radioButton selected
/// @param p_userData        The userData
/// @param p_onChange        The function to call when a button is clicked
/// @param p_userDataOnFocus The userData when the mouse is hovering above the object
/// @param p_onFocus         The function to call when the mouse is hovering above the object
/// @param p_onFocusLost     The function to call when the mouse stops hovering above the object
/// @return                  The radioButtons object id
int GfuiRadioButtonsCreate(void* p_scr, int p_font, int p_x, int p_y, int p_imageWidth, int p_imageHeight,
                           const char* p_pszText, int p_selected,
                           void* p_userData, tfuiRadioButtonsCallback p_onChange,
                           void* p_userDataOnFocus, tfuiCallback p_onFocus, tfuiCallback p_onFocusLost)
{
    tGfuiRadioButtons* radioButtons;
    tGfuiObject* object;
    tGfuiScreen* screen = (tGfuiScreen*)p_scr;

    object = (tGfuiObject*)calloc(1, sizeof(tGfuiObject));
    object->widget = GFUI_RADIOBUTTONS;
    object->focusMode = GFUI_FOCUS_NONE;
    object->id = screen->curId++;
    object->visible = 1;

    radioButtons = &(object->u.radiobuttons);
    radioButtons->onChange = p_onChange;
    radioButtons->pInfo = new tRadioButtonsInfo;
    radioButtons->pInfo->userData = p_userData;
    radioButtons->pInfo->selected = p_selected;
    radioButtons->pInfo->amount = 4;
    radioButtons->pInfo->dist = 10;
    radioButtons->scr = p_scr;

    // Initialize the checked and unchecked button children.
    // Warning: All the images are supposed to be the same size.
    // TODO: Make graphic properties XML-customizable (images, ...)
    // Note: We avoid sharing the same p_userDataOnFocus among multiple controls
    //       (otherwise multiple frees at release time ...).
    for (int i = 0; i < radioButtons->pInfo->amount; i++)
    {
        int yPos = p_y - (p_imageHeight + radioButtons->pInfo->dist) * i;

        radioButtons->buttonId[i] =
            GfuiGrButtonCreate(p_scr, "data/img/checked.png", "data/img/checked.png",
                               "data/img/checked.png", "data/img/checked.png",
                               p_x, yPos, p_imageWidth, p_imageHeight, GFUI_MIRROR_NONE, false, GFUI_MOUSE_UP,
                               (void*)(long)(object->id), OnPushFirst,
                               p_userDataOnFocus, p_onFocus, p_onFocusLost);

        radioButtons->buttonId[i + radioButtons->pInfo->amount] =
            GfuiGrButtonCreate(p_scr, "data/img/unchecked.png", "data/img/unchecked.png",
                               "data/img/unchecked.png", "data/img/unchecked.png",
                               p_x, yPos, p_imageWidth, p_imageHeight, GFUI_MIRROR_NONE, false, GFUI_MOUSE_UP,
                               (void*)(long)(object->id), OnPushSecond, 0, 0, 0);
    }

    // Compute total height (text or buttons)
    tGfuiGrButton* pRadioButton = &(gfuiGetObject(p_scr, radioButtons->buttonId[0])->u.grbutton);
    int height = gfuiFont[p_font]->getHeight();
    if (height < pRadioButton->height)
        height = pRadioButton->height;

        // Fix button y coordinate if text is higher than the buttons
    else
    {
        tGfuiGrButton* pUncheckedBut = &(gfuiGetObject(p_scr, radioButtons->buttonId[1])->u.grbutton);
        pRadioButton->y = pUncheckedBut->y =
            p_y + (gfuiFont[p_font]->getHeight() - pRadioButton->height) / 2;
    }

    int width = p_imageWidth + 5 + gfuiFont[p_font]->getWidth(p_pszText);

    // Bounding box
    object->xmin = p_x;
    object->xmax = p_x + width;
    object->ymin = p_y;
    object->ymax = p_y + height;

    // Initialize the label child (beware of p_y if the buttons are higher than the text).
    static const int hPadding = 5;
    const int xl = p_x + p_imageWidth + hPadding;
    int yl = p_y;
    if (height > gfuiFont[p_font]->getHeight())
        yl += (height -  gfuiFont[p_font]->getHeight()) / 2;

    radioButtons->labelId =
        GfuiLabelCreate(p_scr, p_pszText, p_font, xl, yl, 0, GFUI_ALIGN_HL, strlen(p_pszText));

    gfuiAddObject(screen, object);

    GfuiRadioButtonsSetSelected(p_scr, object->id, p_selected);

    return object->id;
}

/// @brief       Draws the radio buttons on screen
/// @param p_obj The RadioButtons object
void GfuiDrawRadioButtons(tGfuiObject* p_obj)
{
    //Do nothing because children already draw themselves
}

/// @brief        Sets the p_text for the radio buttons
/// @param p_scr  The screen information
/// @param p_id   The object p_id
/// @param p_text The p_text
void GfuiRadioButtonsSetText(void* p_scr, int p_id, const char* p_text)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTONS)
        return;

    tGfuiRadioButtons* radioButtons = &(object->u.radiobuttons);

    GfuiLabelSetText(p_scr, radioButtons->labelId, p_text);
}

/// @brief            Selects the radio button
/// @param p_scr      The screen information
/// @param p_id       The object p_id
/// @param p_selected The radiobutton that is selected
void GfuiRadioButtonsSetSelected(void* p_scr, int p_id, int p_selected)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTONS)
        return;

    tGfuiRadioButtons* radioButtons = &(object->u.radiobuttons);

    radioButtons->pInfo->selected = p_selected;

    for (int i = 0; i < radioButtons->pInfo->amount; i++)
    {
        GfuiVisibilitySet(p_scr, radioButtons->buttonId[i], i == p_selected);
        GfuiVisibilitySet(p_scr, radioButtons->buttonId[i + radioButtons->pInfo->amount], i != p_selected);
    }
}

/// @brief         Sets the text p_color to p_color
/// @param p_scr   The screen information
/// @param p_id    The object p_id
/// @param p_color The p_color
void GfuiRadioButtonsSetTextColor(void* p_scr, int p_id, const GfuiColor& p_color)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTONS)
        return;

    tGfuiRadioButtons* radioButtons = &(object->u.radiobuttons);

    GfuiLabelSetColor(p_scr, radioButtons->labelId, p_color.toFloatRGBA());
}

/// @brief       Frees the pointer to the RadioButtons object
/// @param p_obj The RadioButtons object
void GfuiReleaseRadioButtons(tGfuiObject* p_obj)
{
    free(p_obj);
}
