#include "Recorder.h"

Recorder::Recorder()
{
	time_t now = time(0);
	std::string nowString = ctime(&now);
	m_FileName = "Data/Recordings/Record" + nowString + ".txt";
	m_RecordingFile.close();

}


void Recorder::WriteRecording(float* p_Input)
{
	m_RecordingFile.open(m_FileName);

	m_RecordingFile.close();

}