/***************************************************************************

    file                 : joystickconfig.cpp
    created              : Wed Mar 21 21:46:11 CET 2001
    copyright            : (C) 2001 by Eric Espie
    email                : eric.espie@torcs.org
    version              : $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file   
    		Human player joystick configuration menu
    @author	<a href=mailto:eric.espie@torcs.org>Eric Espie</a>
    @version	$Id$
*/


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <tgf.hpp>
#include <tgfclient.h>

#include <forcefeedbackconfig.h>
#include <forcefeedback.h>


// Menu screen handle.
static void *ScrHandle = NULL;
static void* PrevScrHandle = NULL;

//force feedback manager
extern TGFCLIENT_API ForceFeedbackManager forceFeedback;
 


//editBoxStruct
struct EditBox
{
    int id;
    std::string effectTypeName;
    std::string effectParameterName;
};

//editBoxVector
std::vector<EditBox> EditBoxes; 



// Called on menu (re)activation 
static void
onActivate(void * /* dummy */)
{
	for(std::vector<EditBox>::iterator editbox = EditBoxes.begin(); editbox != EditBoxes.end(); ++editbox) {
		
		//convert the integer to a string
		std::ostringstream editBoxValue;
		editBoxValue << forceFeedback.effectsConfig[editbox->effectTypeName.c_str()][editbox->effectParameterName.c_str()];
		
		//reset the on screen value
		GfuiEditboxSetString(ScrHandle, editbox->id, editBoxValue.str().c_str());
		
	}	

}

// Called on menu deactivation
static void
onQuitForceFeedbackConfig(void * /* dummy */)
{
    // Activate caller screen/menu
    GfuiScreenActivate(PrevScrHandle);
}

static void
onSaveForceFeedbackConfig(void * /* dummy */)
{
	//save the changes
	
	//read all the values from the editBox into the forceFeedback config map
	for(std::vector<EditBox>::iterator editbox = EditBoxes.begin(); editbox != EditBoxes.end(); ++editbox) {

		GfLogInfo("%s%s.\n", editbox->effectTypeName.c_str(), editbox->effectParameterName.c_str());

		if (
			editbox->effectParameterName.compare("enabled") == 0
			|| editbox->effectParameterName.compare("reverse") == 0
		){
			
			forceFeedback.effectsConfig[editbox->effectTypeName.c_str()][editbox->effectParameterName.c_str()] = 
				(int)GfuiCheckboxIsChecked(ScrHandle, (int)editbox->id);

	
		}else{
			
			forceFeedback.effectsConfig[editbox->effectTypeName.c_str()][editbox->effectParameterName.c_str()] = 
				atoi(GfuiEditboxGetString(ScrHandle, editbox->id));

		}
		
	}	
	
	
	//call the save function of the force feedback manager
	forceFeedback.saveConfiguration();

	//then quit
	onQuitForceFeedbackConfig(0 /* dummy */);

}

static void
onValueChange(void * /* dummy */)
{
	GfLogInfo("*****************Some value has changed.\n");

}



void *
ForceFeedbackMenuInit(void *prevMenu, void *nextMenu, int curPlayerIdx, const std::string &carName)
{
	//make the previous screen/menu value available outside of this function
    PrevScrHandle = prevMenu;

	// Is the Screen has beean already created  we nothing more to do 
    if (ScrHandle) {
		return ScrHandle;
    }

    // Create screen
    ScrHandle = GfuiScreenCreate(NULL, NULL, onActivate, NULL, NULL, 1);
	
	// load menu XML descriptor 
    void *menuXMLDescHdle = GfuiMenuLoad("forcefeedbackconfigmenu.xml");

	// Create static controls.
    GfuiMenuCreateStaticControls(ScrHandle, menuXMLDescHdle);

	/*******************************************************************
	**	NOW WE ARE GONNA CREATE THE DYNAMIC CONTROLS...
	**
	********************************************************************
	*/
	//read the current force feedback configuration
	forceFeedback.readConfiguration(carName);


	// create and set the values on the input fields
	std::string editBoxName;
	std::string sectionName;
	int editBoxId = 0;
	

	// iterate on the first map: the various effect config sections
	typedef std::map<std::string, std::map<std::string, int> >::iterator it_type;
	for(it_type iterator = forceFeedback.effectsConfig.begin(); iterator != forceFeedback.effectsConfig.end(); ++iterator) {
		// iterator->first = key (effect type name)
		// iterator->second = value (second map)

		// now iterate on the second map: the actuals config params of each section
		typedef std::map<std::string, int>::iterator it_type2;
		for(it_type2 iterator2 = iterator->second.begin(); iterator2 != iterator->second.end(); ++iterator2) {
			// iterator2->first = key (effect parameter name)
			// iterator2->second = value (effect value)
	
			editBoxName.clear();
			editBoxName.append(iterator->first.c_str());
			editBoxName.append(iterator2->first.c_str());

			sectionName = "dynamic controls/";
			sectionName.append(editBoxName);

			//GfLogInfo("%s%s\n", iterator->first.c_str(), iterator2->first.c_str());
			
			if(GfParmExistsSection(menuXMLDescHdle, sectionName.c_str())){
				GfLogInfo("Exist: %s\n", editBoxName.c_str());
		
				//if (iterator2->first.c_str() == "enabled"){
				if (
					iterator2->first.compare("enabled") == 0
					|| iterator2->first.compare("reverse") == 0
				){
					
					int checkboxId =
						GfuiMenuCreateCheckboxControl(ScrHandle, menuXMLDescHdle, editBoxName.c_str(),
							  NULL, NULL/*onClientPlayerReady*/);
							  
					//
					GfuiCheckboxSetChecked(ScrHandle, checkboxId, (bool)iterator2->second);
					//
					//GfuiCheckboxSetText(ScrHandle, checkboxId, iterator->first.c_str());
					
					//save this data for later use
					EditBox editbox;
					editbox.id = checkboxId;
					editbox.effectTypeName = iterator->first.c_str();
					editbox.effectParameterName = iterator2->first.c_str();

					//add it to our list
					EditBoxes.push_back(editbox);	
												
					GfLogInfo("Generated checkbox for (%s)\n", editBoxName.c_str());
					
				}else{
					//crete the editBox GUI
					editBoxId = GfuiMenuCreateEditControl(ScrHandle, menuXMLDescHdle, editBoxName.c_str(),
					NULL, NULL, onValueChange);
					
					//convert the integer to a string
					std::ostringstream editBoxValue;
					editBoxValue << iterator2->second;

					//set the value of the editBox
					GfuiEditboxSetString(ScrHandle, editBoxId, editBoxValue.str().c_str());
					
					//save this data for later use
					EditBox editbox;
					editbox.id = editBoxId;
					editbox.effectTypeName = iterator->first.c_str();
					editbox.effectParameterName = iterator2->first.c_str();
					
					//add it to our list
					EditBoxes.push_back(editbox);					
				}
				
			}else{
				//GfLogInfo("NON Exist: %s\n", editBoxName.c_str());
				
			}
		}
	}
		   
		   
    // create Accept and Cancel buttons. (dynamic controls)
    GfuiMenuCreateButtonControl(ScrHandle, menuXMLDescHdle, "ApplyButton", NULL, onSaveForceFeedbackConfig);
    GfuiMenuCreateButtonControl(ScrHandle, menuXMLDescHdle, "CancelButton", NULL, onQuitForceFeedbackConfig);

    // Close menu XML descriptor.
    GfParmReleaseHandle(menuXMLDescHdle);
    
    // Register keyboard shortcuts.
    GfuiMenuDefaultKeysAdd(ScrHandle);
    GfuiAddKey(ScrHandle, GFUIK_ESCAPE, "Quit", NULL, onQuitForceFeedbackConfig , NULL);
    GfuiAddKey(ScrHandle, GFUIK_RETURN, "Save", NULL, onSaveForceFeedbackConfig, NULL);

    return ScrHandle;
}





