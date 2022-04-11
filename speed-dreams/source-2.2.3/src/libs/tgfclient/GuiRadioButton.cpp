#include <cstdlib>
#include <cstring>

#include "tgfclient.h"
#include "gui.h"
#include "guifont.h"


/// @brief Initializes the radio button
void GfuiRadioButtonInit(void) { }

/// @brief       Function to call when a selected radiobutton is clicked
/// @param p_idv The RadioButton object id
static void GfuiSelected(void* p_idv)
{
    tGfuiObject* object = gfuiGetObject(GfuiScreen, (long)p_idv);
    if (!object)
        return;

    tGfuiRadioButton* radioButton = &(object->u.radiobutton);

    GfuiRadioButtonListSetSelected(GfuiScreen, radioButton->ParentControl, -1);

    if (radioButton->OnChange)
        radioButton->OnChange(radioButton->Info);
}

/// @brief       Function to call when a non-selected radiobutton is clicked
/// @param p_idv The RadioButton object id
static void GfuiNotSelected(void* p_idv)
{
    tGfuiObject* object = gfuiGetObject(GfuiScreen, (long)p_idv);
    if (!object)
        return;

    tGfuiRadioButton* radioButton = &(object->u.radiobutton);

    GfuiRadioButtonListSetSelected(GfuiScreen, radioButton->ParentControl, radioButton->NrInList);

    if (radioButton->OnChange)
        radioButton->OnChange(radioButton->Info);
}

/// @brief            Selects or deselects the radio button, if the button was deselected it sets the selected to -1.
/// @param p_scr      The current screen
/// @param p_id       The RadioButton object id
/// @param p_selected If the radiobutton is selected
void GfuiRadioButtonSelect(void* p_scr, int p_id, bool p_selected)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTON)
        return;

    tGfuiRadioButton* radioButton = &(object->u.radiobutton);

    radioButton->Info->Checked  = p_selected;
    radioButton->Info->Selected = p_selected ? radioButton->NrInList : -1;
    GfuiVisibilitySet(p_scr, radioButton->SelectedControl, p_selected);
    GfuiVisibilitySet(p_scr, radioButton->NotSelectedControl, !p_selected);
}

/// @brief                   Creates a RadioButton object
/// @param p_scr             The current screen
/// @param p_font            The text font
/// @param p_x               The radio-button-list x-coordinate
/// @param p_y               The radio-button-list y-coordinate
/// @param p_imageWidth      The image width
/// @param p_imageHeight     The image height
/// @param p_pszText         The text
/// @param p_selected        The id of the radio button selected (-1 for none)
/// @param p_userData        The userData
/// @param p_onChange        The function to call when the button is clicked
/// @param p_userDataOnFocus The userData when the mouse is hovering above the object
/// @param p_onFocus         The function to call when the mouse is hovering above the object
/// @param p_onFocusLost     The function to call when the mouse stops hovering above the object
/// @return                  The RadioButton object id
int GfuiRadioButtonCreate(void* p_scr, int p_font, int p_x, int p_y, int p_imageWidth, int p_imageHeight,
                          const char* p_pszText, int p_selected, int p_listId, int p_parentId,
                          void* p_userData, tfuiRadioButtonCallback p_onChange,
                          void* p_userDataOnFocus, tfuiCallback p_onFocus, tfuiCallback p_onFocusLost)
{
    tGfuiRadioButton* radioButton;
    tGfuiObject* object;
    tGfuiScreen* screen = (tGfuiScreen*)p_scr;

    object = (tGfuiObject*)calloc(1, sizeof(tGfuiObject));
    object->widget = GFUI_RADIOBUTTON;
    object->focusMode = GFUI_FOCUS_NONE;
    object->id = screen->curId++;
    object->visible = 1;

    radioButton = &(object->u.radiobutton);
    radioButton->OnChange = p_onChange;
    radioButton->Info     = new tRadioButtonInfo;
    radioButton->Info->Checked  = p_selected == p_listId;
    radioButton->Info->Selected = p_selected;
    radioButton->Info->UserData = p_userData;
    radioButton->NrInList       = p_listId;
    radioButton->ParentControl  = p_parentId;
    radioButton->Scr            = p_scr;

    // Initialize the checked and unchecked button children.
    // Warning: All the images are supposed to be the same size.
    // Note: We avoid sharing the same p_userDataOnFocus among multiple controls
    //       (otherwise multiple frees at release time ...).
    radioButton->SelectedControl =
        GfuiGrButtonCreate(p_scr, "data/img/radio-checked.png", "data/img/radio-checked.png",
                           "data/img/radio-checked.png", "data/img/radio-checked.png",
                           p_x, p_y, p_imageWidth, p_imageHeight, GFUI_MIRROR_NONE, false, GFUI_MOUSE_UP,
                           (void*)(long)(object->id), GfuiSelected,
                           p_userDataOnFocus, p_onFocus, p_onFocusLost);

    radioButton->NotSelectedControl =
        GfuiGrButtonCreate(p_scr, "data/img/radio-unchecked.png", "data/img/radio-unchecked.png",
                           "data/img/radio-unchecked.png", "data/img/radio-unchecked.png",
                           p_x, p_y, p_imageWidth, p_imageHeight, GFUI_MIRROR_NONE, false, GFUI_MOUSE_UP,
                           (void*)(long)(object->id), GfuiNotSelected, 0, 0, 0);

    // Compute total height (text or buttons)
    tGfuiGrButton* selectedButton = &(gfuiGetObject(p_scr, radioButton->SelectedControl)->u.grbutton);
    int height = gfuiFont[p_font]->getHeight();
    if (height < selectedButton->height)
        height = selectedButton->height;

        // Fix button p_y coordinate if text is higher than the buttons
    else
    {
        tGfuiGrButton* notSelectedButton = &(gfuiGetObject(p_scr, radioButton->NotSelectedControl)->u.grbutton);
        selectedButton->y = notSelectedButton->y =
            p_y + (gfuiFont[p_font]->getHeight() - selectedButton->height) / 2;
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

    radioButton->LabelControl =
        GfuiLabelCreate(p_scr, p_pszText, p_font, xl, yl, 0, GFUI_ALIGN_HL, strlen(p_pszText));

    gfuiAddObject(screen, object);

    GfuiRadioButtonSelect(p_scr, object->id, p_selected);

    return object->id;
}

/// @brief       Draws the radio button on screen
/// @param p_obj The RadioButton object
void GfuiDrawRadioButton(tGfuiObject* p_obj)
{
    // Do nothing because children already draw themselves
}

/// @brief         Sets the label text to p_text
/// @param p_scr   The current screen
/// @param p_id    The RadioButton object id
/// @param p_texts The text
void GfuiRadioButtonSetText(void* p_scr, int p_id, const char* p_text)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTON)
        return;

    tGfuiRadioButton* radioButton = &(object->u.radiobutton);

    GfuiLabelSetText(p_scr, radioButton->LabelControl, p_text);
}

/// @brief         Sets the text color to p_color
/// @param p_scr   The current screen
/// @param p_id    The RadioButton object id
/// @param p_color The color
void GfuiRadioButtonSetTextColor(void* p_scr, int p_id, const GfuiColor& p_color)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTON)
        return;

    tGfuiRadioButton* radioButton = &(object->u.radiobutton);

    GfuiLabelSetColor(p_scr, radioButton->LabelControl, p_color.toFloatRGBA());
}

/// @brief       Return whether the radiobutton is selected or not
/// @param p_scr The current screen
/// @param p_id  The RadioButton object id
/// @return      Whether the radiobutton is selected or not
bool GfuiRadioButtonIsSelected(void* p_scr, int p_id)
{
    tGfuiObject* object = gfuiGetObject(p_scr, p_id);
    if (!object || object->widget != GFUI_RADIOBUTTON)
        return false;

    tGfuiRadioButton* radioButton = &(object->u.radiobutton);

    return radioButton->Info->Checked;
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

/// @brief       Frees the pointer to the RadioButton object
/// @param p_obj The RadioButton object
void GfuiReleaseRadioButton(tGfuiObject* obj)
{
    free(obj);
}

