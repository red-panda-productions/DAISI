#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "DriveSituation.h"

class SocketBlackBox
{
public:
    /**
     * Initialize a socket black box:
     * connect to the black box
     * register what variables should be shared
     * etc...
     */
    void Initialize();
    /**
     * Shut down a socket black box:
     * disconnect the black box
     */
    void Shutdown();
    /**
     * Based on the current driving situation, return the decisions of the black box:
     * get the variables the black box needs
     * serialize and send over the variables the black box needs to
     * wait for the decisions of the black box (in abstract format)
     * deserialize the decisions
     * return the decisions
     */

    void SerializeDriveSituation(std::stringstream& stringstream, DriveSituation& driveSituation);
    void GetDecisions(DriveSituation& driveSituation); //has to be decision array

    std::vector<std::string> m_variablesToSend;
    std::vector<std::string> m_variablesToReceive;

    using inserterFunction = void (*) (std::vector<std::string>&, DriveSituation&);
    std::unordered_map<std::string, inserterFunction> m_variableConvertAndInsertMap;

    using decisionConvertFunction = void (*) ();
    std::unordered_map<std::string, decisionConvertFunction> m_variableDecisionMap;
};