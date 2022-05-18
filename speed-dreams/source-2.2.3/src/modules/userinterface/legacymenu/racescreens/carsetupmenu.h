/***************************************************************************

    file                 : carsettupmenu.h
    created              : April 2020
    copyright            : (C) 2020 Robert Reif
    web                  : speed-dreams.sourceforge.net

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CARSETUPMENU_H_
#define _CARSETUPMENU_H_

#include <array>

#include <tgfclient.h>

#include "confscreens.h"

class GfRace;
class GfDriver;
class GfCar;
class GfTrack;

#define ITEMS_PER_PAGE  12

class CarSetupMenu : public GfuiMenuScreen
{
public:

    CarSetupMenu();
    bool initialize(void *pPrevMenu,
                    const GfRace *pRace,
                    const GfDriver *pDriver);

    struct attribute
    {
        int         labelId;
        int         editId;
        int         comboId;
        int         defaultLabelId;
        bool        exists;
        tdble       value;
        tdble       minValue;
        tdble       defaultValue;
        tdble       maxValue;
        std::string type;
        std::string section;
        std::string param;
        std::string units;
        std::string label;
        std::string strValue;
        std::string defaultStrValue;
        std::vector<std::string> in;
        int         precision;

        attribute() :
            labelId(0), editId(0), comboId(0), defaultLabelId(0), exists(false),
            value(0), minValue(0), defaultValue(0), maxValue(0), precision(0)
        {
        }
    };

    struct ComboCallbackData
    {
        CarSetupMenu *menu;
        size_t index;
    };

    // callback functions must be static
    static void onActivateCallback(void *pMenu);
    static void onAcceptCallback(void *pMenu);
    static void onCancelCallback(void *pMenu);
    static void onResetCallback(void *pMenu);
    static void onPreviousCallback(void *pMenu);
    static void onNextCallback(void *pMenu);
    static void onComboCallback(tComboBoxInfo *pInfo);

private:

    void storeSettings();
    void loadSettings();
    void updateControls();
    void readCurrentPage();

    const GfCar *getCar() const { return _pDriver->getCar(); }
    const GfTrack *getTrack() const { return _pRace->getTrack(); }

    void onActivate();
    void onAccept();
    void onCancel() const;
    void onReset();
    void onPrevious();
    void onNext();
    void onCombo(tComboBoxInfo *pInfo);

    // The target race.
    const GfRace *_pRace;

    // The target driver.
    const GfDriver *_pDriver;

    std::vector<std::array<attribute, ITEMS_PER_PAGE> > items;
    size_t  currentPage;
    ComboCallbackData comboCallbackData[ITEMS_PER_PAGE];
};

#endif /* _CARSETUPMENU_H_ */
