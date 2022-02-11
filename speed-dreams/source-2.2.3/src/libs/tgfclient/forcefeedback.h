#ifndef __forcefeedback_h__
#define __forcefeedback_h__
/***************************************************************************
                    forcefeedback.h -- Interface file for The Gaming Framework
                             -------------------
    created              : 06/03/2015
    copyright            : (C) 2015 by MadBad
    email                : madbad82@gmail.com
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
        The Gaming Framework API (client part).
    @author     <a href=mailto:madbad82@gmail.com>MadBad</a>
    @version    $Id$
*/
/*to be checked*/
#include <map>
#include <vector>
#include <string>
#include <ctime>
#include <car.h> //tCarElt
#include <raceman.h> //tSituation

// DLL exported symbols declarator for Windows.
#ifdef WIN32
# ifdef TGFCLIENT_DLL
#  define TGFCLIENT_API __declspec(dllexport)
# else
#  define TGFCLIENT_API __declspec(dllimport)
# endif
#else
# define TGFCLIENT_API
#endif


struct forceFeedBackEffect_t {
	std::string name; //a name for the effect
	std::clock_t startTime; //when we have started this effect
	std::clock_t lastExecTime; //the last time the effect was updated/run
	std::clock_t duration; //how mich time the effect should last
};


class TGFCLIENT_API ForceFeedbackManager {

	public:
		void readConfiguration(const std::string &carName);
		void saveConfiguration();
		int updateForce(tCarElt* car, tSituation *s);
		bool initialized;
		int force;
		int reversed;
		std::vector<std::string> effects;
		std::map< std::string, std::map<std::string, int> > effectsConfig;
		std::map< std::string, std::map<std::string, int> > effectsConfigDefault;
		std::string carName;

		//constructor
		ForceFeedbackManager();
	
		//destructor
		~ForceFeedbackManager();


	private:
		//std::vector<std::string> msglist;
		void readConfigurationFromFileSection(const std::string &configFileUrl, const std::string &effectsSectionPath);

		std::clock_t lastExecTime; //the current time
		
		void* menuXMLDescHdle;

		std::clock_t animationStartTime; //when the animation started
		std::clock_t animationRestStartTime; //when the animation started

		int autocenterEffect(tCarElt* car, tSituation *s);
		int engineRevvingEffect(tCarElt* car, tSituation *s);
		int bumpsEffect(tCarElt* car, tSituation *s);
		int lowSpeedCostantForceEffect(tCarElt* car, tSituation *s);
		int globalMultiplier;

};
#endif // __forcefeedback_h__
