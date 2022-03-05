#include "Driver.h"

Driver::Driver(int p_index, const char* p_name) : m_index(p_index), m_humanDriver(p_name) {
    m_humanDriver.init_context(p_index);
}

void Driver::InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {
    m_humanDriver.init_track(m_index, p_track, p_carHandle, p_carParmHandle, p_situation);
}

void Driver::NewRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.new_race(m_index, p_car, p_situation);
}

void Driver::Drive(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.drive_at(m_index, p_car, p_situation);
}

void Driver::PauseRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.pause_race(m_index, p_car, p_situation);
}

void Driver::ResumeRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.resume_race(m_index, p_car, p_situation);
}

int Driver::PitCmd(tCarElt* p_car, tSituation* p_situation) {
    return m_humanDriver.pit_cmd(m_index, p_car, p_situation);
}

void Driver::EndRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.end_race(m_index, p_car, p_situation);
}

void Driver::Shutdown() {
    m_humanDriver.shutdown(m_index);
}

void Driver::Terminate() {
    m_humanDriver.terminate();
}
