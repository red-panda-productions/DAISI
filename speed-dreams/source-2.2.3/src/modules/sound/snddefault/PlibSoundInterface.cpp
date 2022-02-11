/***************************************************************************

    file                 : PlibSoundInterface.cpp
    created              : Thu Apr 7 04:21 CEST 2005
    copyright            : (C) 2005 Christos Dimitrakakis
    email                : dimitrak@idiap.ch
    version              : $Id: PlibSoundInterface.cpp 6998 2020-04-29 15:19:02Z iobyte $

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "CarSoundData.h"

#include "PlibSound.h"
#include "PlibSoundInterface.h"


PlibSoundInterface::PlibSoundInterface(float sampling_rate, int n_channels) : SoundInterface (sampling_rate, n_channels)
{
	sched = new slScheduler ((int) sampling_rate);
	sched->setSafetyMargin (0.128f);
	sched->setMaxConcurrent (n_channels);
	engpri = NULL;
	car_src = NULL;

	// initialise mappings
	grass.schar = &CarSoundData::grass;
	grass_skid.schar = &CarSoundData::grass_skid;
	road.schar = &CarSoundData::road;
	metal_skid.schar = &CarSoundData::drag_collision;
	backfire_loop.schar = &CarSoundData::engine_backfire;
	turbo.schar = &CarSoundData::turbo;
	axle.schar = &CarSoundData::axle;

}

PlibSoundInterface::~PlibSoundInterface()
{
	for (unsigned int i=0; i<sound_list.size(); i++) {
		delete sound_list[i];
	}
	delete [] engpri;
	delete sched;
	
	if (car_src) {
		delete [] car_src;
	}
	
}

void PlibSoundInterface::setNCars(int n_cars)
{
	engpri = new SoundPri[n_cars];
	car_src = new SoundSource[n_cars];
}

slScheduler* PlibSoundInterface::getScheduler()
{
	return sched;
}

Sound* PlibSoundInterface::addSample(const char* filename, int flags, bool loop, bool static_pool)
{
	PlibSound* sound = new PlibSound(sched, filename, flags, loop);
    sound->setVolume(getGlobalGain());
	sound_list.push_back (sound);
	return sound;
}
	
void PlibSoundInterface::update(CarSoundData** car_sound_data, int n_cars, sgVec3 p_obs, sgVec3 u_obs, sgVec3 c_obs, sgVec3 a_obs)
{
	// Copy car ID basically.
	for (int i = 0; i<n_cars; i++) {
		car_sound_data[i]->copyEngPri(engpri[i]);
	}

	for (int i = 0; i<n_cars; i++) {
		int id = engpri[i].id;
		sgVec3 p;
		sgVec3 u;
		car_sound_data[id]->getCarPosition(p);
		car_sound_data[id]->getCarSpeed(u);
		car_src[id].setSource (p, u);
		car_src[id].setListener (p_obs, u_obs);
		car_src[id].update();
		engpri[id].a = car_src[id].a;
	}

	qsort((void*) engpri, n_cars, sizeof(SoundPri), &sortSndPriority);

	for (int i = 0; i<n_cars; i++) {
		int id = engpri[i].id;
		Sound* engine = car_sound_data[id]->getEngineSound();
		if (i>=NB_ENGINE_SOUND) {
			engine->setVolume (0.0f);
			engine->pause();
		} else {
			engine->resume();
			engine->setLPFilter(car_src[id].lp*car_sound_data[id]->engine.lp);
			engine->setPitch(car_src[id].f*car_sound_data[id]->engine.f);
			engine->setVolume(0.5f*getGlobalGain()*car_src[id].a*car_sound_data[id]->engine.a);
			engine->update();
		}
	}

	float max_skid_vol[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	int max_skid_id[4] = {0,0,0,0};
	for (int id = 0; id<n_cars; id++) {
		CarSoundData* sound_data = car_sound_data[id];
		for (int j=0; j<4; j++) {
			float skvol=sound_data->attenuation*sound_data->wheel[j].skid.a;
			if (skvol > max_skid_vol[j]) {
				max_skid_vol[j] = skvol;
				max_skid_id[j] = id;
			}
		}
	}

	for (int i = 0; i<4; i++) {
		int id = max_skid_id[i];
		WheelSoundData* sound_data = car_sound_data[id]->wheel;
		float mod_a = car_src[id].a;
		float mod_f = car_src[id].f;
		skid_sound[i]->setVolume(0.5f*getGlobalGain()*sound_data[i].skid.a * mod_a);
		skid_sound[i]->setPitch(sound_data[i].skid.f * mod_f);
		skid_sound[i]->update();
#if 0
		if (sound_data[i].skid.a > VOLUME_CUTOFF) {
			skid_sound[i]->start();
		} else {
			skid_sound[i]->stop();
		}
#endif
	}

	
	// other looping sounds
	road.snd = road_ride_sound;
	sortSingleQueue (car_sound_data, &road, n_cars);
	setMaxSoundCar (car_sound_data, &road);

	grass.snd = grass_ride_sound;
	sortSingleQueue (car_sound_data, &grass, n_cars);
	setMaxSoundCar (car_sound_data, &grass);

	grass_skid.snd = grass_skid_sound;
	sortSingleQueue (car_sound_data, &grass_skid, n_cars);
	setMaxSoundCar (car_sound_data, &grass_skid);

	metal_skid.snd = metal_skid_sound;
	sortSingleQueue (car_sound_data, &metal_skid, n_cars);
	setMaxSoundCar (car_sound_data, &metal_skid);

	backfire_loop.snd = backfire_loop_sound;
	sortSingleQueue (car_sound_data, &backfire_loop, n_cars);
	setMaxSoundCar (car_sound_data, &backfire_loop);

	turbo.snd = turbo_sound;
	sortSingleQueue (car_sound_data, &turbo, n_cars);
	setMaxSoundCar (car_sound_data, &turbo);

	axle.snd = axle_sound;
	sortSingleQueue (car_sound_data, &axle, n_cars);
	setMaxSoundCar (car_sound_data, &axle);


	// One-off sounds
	for (int id = 0; id<n_cars; id++) {
		float crash_threshold = 0.5f;
		float gear_threshold = 0.75;

		CarSoundData* sound_data = car_sound_data[id];

		if (sound_data->crash) {
			if (++curCrashSnd>=NB_CRASH_SOUND) {
				
				curCrashSnd = 0;
			}
			if (car_src[id].a > crash_threshold) {
				crash_sound[curCrashSnd]->start();
			}
		}

		if (sound_data->bang) {
			if (car_src[id].a > crash_threshold) {
				bang_sound->start();
			}
		}

		if (sound_data->bottom_crash) {
			if (car_src[id].a > crash_threshold) {
				bottom_crash_sound->start();
			}
		}

		if (sound_data->gear_changing) {
			if (car_src[id].a > gear_threshold) {
				gear_change_sound->start();
			}
		}
	}


	sched->update();
}

void PlibSoundInterface::setMaxSoundCar(CarSoundData** car_sound_data, QueueSoundMap* smap)
{
	int id = smap->id;
	//float max_vol = smap->max_vol;
	QSoundChar CarSoundData::*p2schar = smap->schar;
	QSoundChar* schar = &(car_sound_data[id]->*p2schar);
	Sound* snd = smap->snd;

	snd->setVolume(0.5f*getGlobalGain() * schar->a * car_src[id].a);
	snd->setPitch(schar->f * car_src[id].f);
	snd->update();
#if 0
	if (max_vol > VOLUME_CUTOFF) {
		snd->start();
	} else {
		snd->stop();
	}
#endif
}
