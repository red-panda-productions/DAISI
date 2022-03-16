//#include "Recorder.h"
//
//Recorder::Recorder()
//{
//	time_t now = time(0);
//	char* nowString = ctime(&now);
//	m_FileName = "Data/Recordings/" + nowString + ".txt";
//	m_RecordingFile = new std::ofstream(m_FileName, std::ios::app);
//	m_RecordingFile.close()
//
//}
//
//
//void Recorder::WriteRecording(float* p_Input)
//{
//	m_RecordingFile.open(m_FileName);
//
//	m_RecordingFile.close(m_FileName);
//
//}