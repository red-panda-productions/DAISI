#pragma once

#include <car.h>
#include <raceman.h>

class cGrFrameInfo;

class SDAHud
{
protected:
    int Id; // identifier of the hud
    const tCarElt* Car;

    int debugFlag;
    int counterFlag;
    int boardWidth;
    int leftAnchor;
    int centerAnchor;
    int rightAnchor;
    int speedoRise;

private:
    void grDispDebug(const tSituation* s, const cGrFrameInfo* frame);
    void grDispSplitScreenIndicator();
    void grDrawGauge(tdble X1, tdble Y1, tdble H, float* clr1,
        float* clr2, tdble val, const char* title);

    void grDispCounterBoard2();

    void grDispIntervention();

private:
    //Dash colour handling
    float* normal_color_;
    float* danger_color_;
    float* emphasized_color_;
    float* background_color_;

    void ReadDashColor(void* hdle, const std::string& color_name, float** color);

public:
    explicit SDAHud(int myid);
    ~SDAHud();

    void initBoard(void);
    void shutdown(void);
    void selectBoard(int brd);
    void setWidth(int width);

    void refreshBoard(tSituation* s, const cGrFrameInfo* frameInfo,
        const tCarElt* currCar, bool isCurrScreen);
    void loadDefaults(const tCarElt* curCar);
};

extern void grInitBoardCar(tCarElt* car);
extern void grShutdownBoardCar(void);