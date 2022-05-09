/***************************************************************************

    file                 : grboard.cpp
    created              : Thu Aug 17 23:52:20 CEST 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: grboard.cpp 7166 2020-08-09 23:56:00Z kakukri $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// SIMULATED DRIVING ASSISTANCE: removed functions of unused hud elements 
// All changes are documented in src\simulatedDrivingAssistance\CHANGES.txt

#include "grboard.h"

#include <plib/ssg.h>
#include <portability.h>    // snprintf
#include <robottools.h>     // RELAXATION

#include <algorithm>        // remove
#include <sstream>

#include "grmain.h"       // grWinX, grHandle, grMaxDamage
#include "grtrackmap.h"   // cGrTrackMap
#include "grcar.h"        // grCarInfo
#include "grutil.h"       // grWriteTime
#include "grloadac.h"     // grssgSetCurrentOptions
#include "grscreen.h"

#include "IndicatorConfig.h"
#include "Mediator.h"

// SIMULATED DRIVING ASSITANCE: array to store the loaded (ssg) textures
ssgSimpleState** m_textures;

#define ALIGN_CENTER 0
#define ALIGN_LEFT   1
#define ALIGN_RIGHT  2

using std::string;

static const string rgba[4] =
    { GFSCR_ATTR_RED, GFSCR_ATTR_GREEN, GFSCR_ATTR_BLUE, GFSCR_ATTR_ALPHA };

static const int NB_DEBUG = 4;

// Boards work on a OrthoCam with fixed height of 600, width flows
// with split screen(s) and can be limited to 'board width' % of screen
static const int TOP_ANCHOR = 600;
static const int BOTTOM_ANCHOR = 0;
static const int DEFAULT_WIDTH = 800;

static const int BUFSIZE = 256;


cGrBoard::cGrBoard(int myid) : 
    normal_color_(NULL), danger_color_(NULL), emphasized_color_(NULL), background_color_(NULL)
{
  id = myid;
}


cGrBoard::~cGrBoard()
{
  delete [] normal_color_;
  delete [] danger_color_;
  delete [] emphasized_color_;
  delete [] background_color_;
}


void cGrBoard::loadDefaults(const tCarElt *curCar)
{
  //Load dash colours from graph.xml
  char path[1024];
  snprintf(path, sizeof(path), "%s%s", GfLocalDir(), GR_PARAM_FILE);
  void *hdle = GfParmReadFile(path, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);
  const char *pszSpanSplit;

  ReadDashColor(hdle, GFSCR_ELT_NORMALCLR,      &normal_color_);
  ReadDashColor(hdle, GFSCR_ELT_DANGERCLR,      &danger_color_);
  ReadDashColor(hdle, GFSCR_ELT_EMPHASIZEDCLR,  &emphasized_color_);
  ReadDashColor(hdle, GFSCR_ELT_BACKGROUNDCLR,  &background_color_);

  GfParmReleaseHandle(hdle);

  //Load other dash properties
  snprintf(path, sizeof(path), "%s/%d", GR_SCT_DISPMODE, id);

  debugFlag = (int)GfParmGetNum(grHandle, path, GR_ATT_DEBUG, NULL, 1);
  boardWidth  = (int)GfParmGetNum(grHandle, path, GR_ATT_BOARDWIDTH, NULL, 100);
  speedoRise  = (int)GfParmGetNum(grHandle, path, GR_ATT_SPEEDORISE, NULL, 0);

  // Only apply driver preferences when not spanning split screens
  pszSpanSplit = GfParmGetStr(grHandle, GR_SCT_GRAPHIC, GR_ATT_SPANSPLIT, GR_VAL_NO);
  if (strcmp(pszSpanSplit, GR_VAL_YES) && curCar->_driverType == RM_DRV_HUMAN) {
    snprintf(path, sizeof(path), "%s/%s", GR_SCT_DISPMODE, curCar->_name);
    debugFlag = (int)GfParmGetNum(grHandle, path, GR_ATT_DEBUG, NULL, debugFlag);
    boardWidth  = (int)GfParmGetNum(grHandle, path, GR_ATT_BOARDWIDTH, NULL, boardWidth);
    speedoRise  = (int)GfParmGetNum(grHandle, path, GR_ATT_SPEEDORISE, NULL, speedoRise);
  }

  if (boardWidth < 0 || boardWidth > 100)
    boardWidth = 100;
  this->setWidth(DEFAULT_WIDTH);

  if (speedoRise < 0 || speedoRise > 100)
    speedoRise = 0;
}


void cGrBoard::setWidth(int val)
{
  // Setup the margins according to percentage of screen width
  centerAnchor = (val / 2);
  leftAnchor = (val / 2) - val * boardWidth / 200;
  rightAnchor = (val / 2) + val * boardWidth / 200;
}


/// @brief      Updates one of the board flags, called when one of the defined keybinds is pressed.
/// @param val  Value corresponding to a specific keybind.
void cGrBoard::selectBoard(int val)
{
  char path[1024];
  snprintf(path, sizeof(path), "%s/%d", GR_SCT_DISPMODE, id);

  switch (val) {
    case 3:
      debugFlag = (debugFlag + 1) % NB_DEBUG;
      GfParmSetNum(grHandle, path, GR_ATT_DEBUG, (char*)NULL, (tdble)debugFlag);
      break;
  }
  GfParmWriteFile(NULL, grHandle, "graph");
}


/**
 * grDispDebug
 *
 * Displays debug information in the top right corner.
 * It is a 3-state display, states as follows:
 * 0 - Don't display any info
 * 1 - Display the mean and instant FPS
 * 2 - Like 2 + the absolute frame counter
 * 3 - Like 2 + the segment the car is on, car's distance from startline, current camera
 *
 * @param s[in]     The current situation
 * @param frame[in] Frame info to display
 */
void cGrBoard::grDispDebug(const tSituation *s, const cGrFrameInfo* frame)
{
  char buf[BUFSIZE];
  snprintf(buf, sizeof(buf), "FPS: %.1f(%.1f)  ",
            frame->fInstFps, frame->fAvgFps);
  int dx = GfuiFontWidth(GFUI_FONT_SMALL_C, buf);

  int x2 = rightAnchor - dx;    // 2nd column
  int y = TOP_ANCHOR - 15;
  int dy = GfuiFontHeight(GFUI_FONT_SMALL_C);
  int x = (debugFlag > 1) ? x2 - dx : x2;   // 1st column

  // Display frame rates (instant and average)
  snprintf(buf, sizeof(buf), "FPS: %.1f(%.1f)",
            frame->fInstFps, frame->fAvgFps);
  GfuiDrawString(buf, normal_color_, GFUI_FONT_SMALL_C, x, y);

  if (debugFlag == 2) {
    //Only display detailed information in Debug Mode > 1
    // Display frame counter in 2nd column
    snprintf(buf, sizeof(buf),  "Frm: %u", frame->nTotalFrames);
    GfuiDrawString(buf, normal_color_, GFUI_FONT_SMALL_C, x2, y);

    // Display current fovy in 2nd row, 1st column
    y -= dy;
    cGrScreen *curScreen = grGetCurrentScreen();
    if (curScreen) {
      cGrCamera *curCam = curScreen->getCurCamera();
      if (curCam) {
        snprintf(buf, sizeof(buf), "FovY: %2.1f", curCam->getFovY());
        GfuiDrawString(buf, normal_color_, GFUI_FONT_SMALL_C, x, y);
      }
    }

    // Display simulation time in 2nd row, 2nd column
    snprintf(buf, sizeof(buf),  "Time: %.f", s->currentTime);
    GfuiDrawString(buf, normal_color_, GFUI_FONT_SMALL_C, x2, y);
  } else if (debugFlag == 3) {
    // Only display detailed information in Debug Mode > 1
    // Display segment name in 2nd column
    snprintf(buf, sizeof(buf),  "Seg: %s", car_->_trkPos.seg->name);
    GfuiDrawString(buf, normal_color_, GFUI_FONT_SMALL_C, x2, y);

    // Display distance from start in 2nd row, 1st column
    y -= dy;
    snprintf(buf, sizeof(buf), "DfS: %5.0f", car_->_distFromStartLine);
    GfuiDrawString(buf, normal_color_, GFUI_FONT_SMALL_C, x, y);

    // Display current camera in 2nd row, 2nd column
    tRoadCam *curCam = car_->_trkPos.seg->cam;
    if (curCam) {
      snprintf(buf, sizeof(buf), "Cam: %s", curCam->name);
      GfuiDrawString(buf, normal_color_, GFUI_FONT_SMALL_C, x2, y);
    }
  }
}  // grDispDebug


/// @brief          Draws a vertical gauge displaying some information
/// @param X1       Reference X-coordinate to start drawining from
/// @param Y1       Reference Y-coordinate to start drawining from
/// @param H        Height of the gauge
/// @param color1   Color of the information to be drawn 
/// @param color2   Color of the gauge background
/// @param val      Value of the information to display
/// @param title    Name of the information to display
void cGrBoard::grDrawGauge(tdble X1, tdble Y1, tdble H,
                        float *color1, float *color2,
                        tdble val, const char *title)
{
  tdble curH = MAX(MIN(val, 1.0), 0.0);
  curH *= H;

  static const tdble THNSSBG = 2.0;
  static const tdble THNSSFG = 2.0;

  glBegin(GL_QUADS);
  // set F and D column to: 404040
  glColor4f(0.25, 0.25, 0.25, 0.8);
  glVertex2f(X1 - (THNSSBG + THNSSFG), Y1 - THNSSBG);
  glVertex2f(X1 + (THNSSBG + THNSSFG), Y1 - THNSSBG);
  glVertex2f(X1 + (THNSSBG + THNSSFG), Y1 + H + THNSSBG);
  glVertex2f(X1 - (THNSSBG + THNSSFG), Y1 + H + THNSSBG);

  glColor4fv(color2);
  glVertex2f(X1 - THNSSFG, Y1 + curH);
  glVertex2f(X1 + THNSSFG, Y1 + curH);
  glVertex2f(X1 + THNSSFG, Y1 + H);
  glVertex2f(X1 - THNSSFG, Y1 + H);

  glColor4fv(color1);
  glVertex2f(X1 - THNSSFG, Y1);
  glVertex2f(X1 + THNSSFG, Y1);
  glVertex2f(X1 + THNSSFG, Y1 + curH);
  glVertex2f(X1 - THNSSFG, Y1 + curH);
  glEnd();
  GfuiDrawString((char *)title, emphasized_color_, GFUI_FONT_MEDIUM, //yellow
                    (int)(X1 - (THNSSBG + THNSSFG)),
                    (int)(Y1 - THNSSBG - GfuiFontHeight(GFUI_FONT_MEDIUM)),
                    2*(THNSSBG + THNSSFG), GFUI_ALIGN_HC);
}


/// @brief For split-screen gameplay: draws a green square in the bottom right of the focussed screen.
void cGrBoard::grDispSplitScreenIndicator()
{
    static const float h = 10.0f;
    const float w = h;

    const float x = rightAnchor - w - 5;
    const float y = BOTTOM_ANCHOR + 5;

    glBegin(GL_QUADS);
    glColor4f(0.0, 1.0, 0.0, 1.0);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

/// @brief Middle 'Driver Counters' display with speed/gear meters and Fuel/Damage gauges.
void cGrBoard::grDispCounterBoard2()
{
  // RPM
  tgrCarInstrument *curInst = &(grCarInfo[car_->index].instrument[0]);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTranslatef(centerAnchor,
                BOTTOM_ANCHOR + (speedoRise * TOP_ANCHOR / 100), 0);
  if (curInst->texture) {
    glBindTexture(GL_TEXTURE_2D, curInst->texture->getTextureHandle());
  }
  glCallList(curInst->CounterList);
  glBindTexture(GL_TEXTURE_2D, 0);

  tdble val = (*(curInst->monitored) - curInst->minValue) / curInst->maxValue;
  val = MIN(1.0, MAX(0.0, val));    // val between 0.0 and 1.0
  val = curInst->minAngle + val * curInst->maxAngle;

  RELAXATION(val, curInst->prevVal, 30);

  glPushMatrix();
  glTranslatef(curInst->needleXCenter, curInst->needleYCenter, 0);
  glRotatef(val, 0, 0, 1);
  glCallList(curInst->needleList);
  glPopMatrix();

  // Show gear
  char buf[32];
  if (car_->_gear <= 0)
    snprintf(buf, sizeof(buf), "%s", car_->_gear == 0 ? "N" : "R");
  else
    snprintf(buf, sizeof(buf), "%d", car_->_gear);
  GfuiDrawString(buf, curInst->needleColor, GFUI_FONT_LARGE_C,
                    (int)curInst->digitXCenter - 30,
                    (int)curInst->digitYCenter,
                    60, GFUI_ALIGN_HC);

  glTranslatef(-centerAnchor, -BOTTOM_ANCHOR, 0);

  // Speedo
  curInst = &(grCarInfo[car_->index].instrument[1]);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTranslatef(centerAnchor, BOTTOM_ANCHOR, 0);
  if (curInst->texture) {
    glBindTexture(GL_TEXTURE_2D, curInst->texture->getTextureHandle());
  }
  glCallList(curInst->CounterList);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Reverse speed should show on needle, too
  val = (*(curInst->monitored) - curInst->minValue) / curInst->maxValue;
  if (val < 0.0)
    val *= -1.0;
  val = MIN(1.0, val);
  val = curInst->minAngle + val * curInst->maxAngle;

  RELAXATION(val, curInst->prevVal, 30);

  glPushMatrix();
  glTranslatef(curInst->needleXCenter, curInst->needleYCenter, 0);
  glRotatef(val, 0, 0, 1);
  glCallList(curInst->needleList);
  glPopMatrix();

  // Digital speedo
  if (curInst->digital) {
    // Do not add "%3d" or something, because the digital font
    // DOES NOT SUPPORT BLANKS!!!!
    snprintf(buf, sizeof(buf), "%d", abs((int)(car_->_speed_x * 3.6)));
    GfuiDrawString(buf, curInst->needleColor, GFUI_FONT_LARGE_C,
                    (int)curInst->digitXCenter - 30,
                    (int)curInst->digitYCenter,
                    60, GFUI_ALIGN_HC);
  }

  glTranslatef(-centerAnchor, -BOTTOM_ANCHOR, 0);
  glTranslatef(0, -(speedoRise * TOP_ANCHOR / 100), 0);
}  // grDispCounterBoard2
 

/// @brief Initializes the dashboard by creating a trackmap object.
void cGrBoard::initBoard(void)
{

}


/// @brief Shuts down the dashboard by releasing the trackmap object memory.
void cGrBoard::shutdown(void)
{

}

/// @brief              Refreshes the HUD, called every frame.
/// @param s            The current situation
/// @param frameInfo    Information about the number of passed frames, average fps, etc
/// @param currCar      The current car that is being driven
/// @param isCurrScreen Boolean flag whether the displayed screen is the focussed screen.
void cGrBoard::refreshBoard(tSituation *s, const cGrFrameInfo* frameInfo,
                            const tCarElt *currCar, bool isCurrScreen)
{
    car_ = currCar;

    // SIMULATED DRIVING ASSISTANCE: displays the current active and neutral intervention indicators
    tIndicator settings = SMediator::GetInstance()->GetIndicatorSettings();
    InterventionType interventionType = SMediator::GetInstance()->GetInterventionType();
    DispActiveIndicators(settings, interventionType);
    DispNeutralIndicators(settings, interventionType);

    grDispCounterBoard2();

    if (debugFlag) 
        grDispDebug(s, frameInfo);
    if (isCurrScreen) 
        grDispSplitScreenIndicator();
}

// SIMULATED DRIVING ASSISTANCE
/// @brief Displays the currently active indicators from IndicatorConfig
///        Depending on the indicator settings that are currently active.
void cGrBoard::DispActiveIndicators(tIndicator settings, InterventionType interventionType) 
{
    for (const tIndicatorData &indicator : IndicatorConfig::GetInstance()->GetActiveIndicators(interventionType))
    {
        DispIndicatorsHelper(indicator, settings);
    }
}

// SIMULATED DRIVING ASSISTANCE
/// @brief Displays the currently neutral indicators from IndicatorConfig
///        Depending on the indicator settings that are currently active.
void cGrBoard::DispNeutralIndicators(tIndicator settings, InterventionType interventionType)
{
    for (const tIndicatorData &indicator : IndicatorConfig::GetInstance()->GetNeutralIndicators(interventionType))
    {
        DispIndicatorsHelper(indicator, settings);
    }
}

// SIMULATED DRIVING ASSISTANCE
/// @brief Helper function for displaying active and neutral indicators from IndicatorConfig
///        Depending on the indicator settings that are currently active.
void cGrBoard::DispIndicatorsHelper(tIndicatorData m_indicator, tIndicator settings) 
{
    if (settings.Icon)
        DispIndicatorIcon(m_indicator.Texture, m_textures[m_indicator.Action]);

    if (settings.Text)
        DispIndicatorText(m_indicator.Text);
}

// SIMULATED DRIVING ASSISTANCE
/// @brief           Displays the icon indicator (if the texture was loaded correctly) 
/// @param p_data    Pointer to struct containing data about the texture, like its position   
/// @param p_texture Pointer to the object containing the actual loaded OpenGL texture
void cGrBoard::DispIndicatorIcon(tTextureData *p_data, ssgSimpleState *p_texture)
{
    // Guard if texture data is null or the texture itself is null
    if (!p_data || !p_texture) return;

    // Dimensions of the icon on the screen (will be put in XML settings file later)
    float iconWidth = 100;
    float iconHeight = 100;

    // Duplicate the current matrix and enable opengl settings.
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Translate the opengl matrix to the position on the screen where we want to display the texture, and load the texture.
    glTranslatef(
        rightAnchor * p_data->ScrPos.X,
        TOP_ANCHOR  * p_data->ScrPos.Y,
        0);
    glBindTexture(GL_TEXTURE_2D, p_texture->getTextureHandle());

    // Draw the texture as a Triangle Strip. 
    // glTexCoord2f defines point of the texture that you take (0-1).
    // glVertex2f then defines where to place this on the screen (relative to the current matrix)
    glBegin(GL_TRIANGLE_STRIP);
    glColor4f(1.0, 1.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);
    glTexCoord2f(0.0, 1.0); glVertex2f(0, iconHeight);
    glTexCoord2f(1.0, 0.0); glVertex2f(iconWidth, 0);
    glTexCoord2f(1.0, 1.0); glVertex2f(iconWidth, iconHeight);
    glEnd();

    // Unbind the texture and pop the translated matrix of the stack.
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

// SIMULATED DRIVING ASSISTANCE
/// @brief        Displays the intervention indicator text
/// @param p_data Pointer to struct containing data about the text, like its position  
void cGrBoard::DispIndicatorText(tTextData* p_data)
{
    // Guard if no text data is defined for this indicator
    if (!p_data) return;

    GfuiDrawString(
        rightAnchor * p_data->ScrPos.X, 
        TOP_ANCHOR * p_data->ScrPos.Y, 
        p_data->Text, p_data->Font, normal_color_);
}

// SIMULATED DRIVING ASSISTANCE
/// @brief Loads the textures for the indicators from their filepaths.
///        MUST BE CALLED AFTER THE TEXTURE DIRECTORY HAS BEEN ADDED TO grFilePath
///        which happens in grInitBoardCar.
void LoadIndicatorTextures()
{
    std::vector<tIndicatorData> indicators = IndicatorConfig::GetInstance()->GetIndicatorData();

    m_textures = new ssgSimpleState*[indicators.size()];
    for (const tIndicatorData& indicator : indicators)
    {
        ssgSimpleState* texture = nullptr;
        if (indicator.Texture)
        {
            // Guard if there is no texture data for this action.
            texture = (ssgSimpleState*)grSsgLoadTexState(indicator.Texture->Path);
        }
        m_textures[indicator.Action] = texture;
    }
}


void grInitBoardCar(tCarElt *car)
{
  static const int nMaxTexPathSize = 4096;
  const bool bMasterModel = strlen(car->_masterModel) != 0;

  grssgLoaderOptions options;
  grssgSetCurrentOptions(&options);

  int index = car->index; /* current car's index */
  tgrCarInfo *carInfo = &grCarInfo[index];
  void *handle = car->_carHandle;

  /* Set tachometer/speedometer textures search path :
   1) driver level specified, in the user settings,
   2) driver level specified,
   3) car level specified,
   4) common textures */
  grFilePath = (char*)malloc(nMaxTexPathSize);
  int lg = 0;
  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "%sdrivers/%s/%d/%s;",
                    GfLocalDir(), car->_modName, car->_driverIndex, car->_carName);
  if (bMasterModel)
    lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "%sdrivers/%s/%d/%s;",
                    GfLocalDir(), car->_modName, car->_driverIndex, car->_masterModel);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "%sdrivers/%s/%d;",
                    GfLocalDir(), car->_modName, car->_driverIndex);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "%sdrivers/%s/%s;",
                    GfLocalDir(), car->_modName, car->_carName);
  if (bMasterModel)
    lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "%sdrivers/%s/%s;",
                    GfLocalDir(), car->_modName, car->_masterModel);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "%sdrivers/%s;",
                    GfLocalDir(), car->_modName);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "drivers/%s/%d/%s;",
                    car->_modName, car->_driverIndex, car->_carName);
  if (bMasterModel)
    lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "drivers/%s/%d/%s;",
                    car->_modName, car->_driverIndex, car->_masterModel);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "drivers/%s/%d;",
                    car->_modName, car->_driverIndex);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "drivers/%s/%s;",
                    car->_modName, car->_carName);
  if (bMasterModel)
    lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "drivers/%s/%s;",
                    car->_modName, car->_masterModel);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "drivers/%s;", car->_modName);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "cars/models/%s;", car->_carName);
  if (bMasterModel)
    lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "cars/models/%s;", car->_masterModel);

  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "data/textures;");

  // SIMULATED DRIVING ASSISTANCE
  // Add the folder containing indicator textures to the searchable filepaths for filenames.
  lg += snprintf(grFilePath + lg, nMaxTexPathSize - lg, "data/indicators/texture");

  LoadIndicatorTextures();


  /* Tachometer --------------------------------------------------------- */
  tgrCarInstrument *curInst = &(carInfo->instrument[0]);

  /* Load the Tachometer texture */
  const char* param = GfParmGetStr(handle, SECT_GROBJECTS,
                                    PRM_TACHO_TEX, "rpm8000.png");

  curInst->texture = (ssgSimpleState*)grSsgLoadTexState(param);
  if (curInst->texture == 0)
    curInst->texture = (ssgSimpleState*)grSsgLoadTexState("rpm8000.rgb");

  /* Load the instrument placement */
  tdble xSz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_XSZ, (char*)NULL, 128);
  tdble ySz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_YSZ, (char*)NULL, 128);

  // position are delta from center of screen
  tdble xpos = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_XPOS, (char*)NULL, 0 - xSz);
  tdble ypos = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_YPOS, (char*)NULL, 0);
  tdble needlexSz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_NDLXSZ, (char*)NULL, 50);
  tdble needleySz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_NDLYSZ, (char*)NULL, 2);
  curInst->needleXCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_XCENTER, (char*)NULL, xSz / 2.0) + xpos;
  curInst->needleYCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_YCENTER, (char*)NULL, ySz / 2.0) + ypos;
  curInst->digitXCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_XDIGITCENTER, (char*)NULL, xSz / 2.0) + xpos;
  curInst->digitYCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_YDIGITCENTER, (char*)NULL, 10) + ypos;
  curInst->minValue = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_MINVAL, (char*)NULL, 0);
  curInst->maxValue = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_MAXVAL, (char*)NULL, RPM2RADS(10000)) - curInst->minValue;
  curInst->minAngle = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_MINANG, "deg", 225);
  curInst->maxAngle = GfParmGetNum(handle, SECT_GROBJECTS, PRM_TACHO_MAXANG, "deg", -45) - curInst->minAngle;
  curInst->monitored = &(car->_enginerpm);
  curInst->prevVal = curInst->minAngle;

  /* Get colour to use for needle, default is Red */
  curInst->needleColor[0] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_RED, (char*)NULL, 1.0);
  curInst->needleColor[1] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_GREEN, (char*)NULL, 0.0);
  curInst->needleColor[2] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_BLUE, (char*)NULL, 0.0);
  curInst->needleColor[3] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_ALPHA, (char*)NULL, 1.0);

  curInst->CounterList = glGenLists(1);
  glNewList(curInst->CounterList, GL_COMPILE);
  glBegin(GL_TRIANGLE_STRIP);
  {
    glColor4f(1.0, 1.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(xpos, ypos);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(xpos, ypos + ySz);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(xpos + xSz, ypos);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(xpos + xSz, ypos + ySz);
  }
  glEnd();
  glEndList();

  curInst->needleList = glGenLists(1);
  glNewList(curInst->needleList, GL_COMPILE);
  glBegin(GL_TRIANGLE_STRIP);
  {
    glColor4f(curInst->needleColor[0], curInst->needleColor[1],
                curInst->needleColor[2], curInst->needleColor[3]);
    glVertex2f(0, -needleySz);
    glVertex2f(0, needleySz);
    glVertex2f(needlexSz, -needleySz / 2.0);
    glVertex2f(needlexSz, needleySz / 2.0);
  }
  glEnd();
  glEndList();


  /* Speedometer ----------------------------------------------------------- */
  curInst = &(carInfo->instrument[1]);

  /* Load the Speedometer texture */
  param = GfParmGetStr(handle, SECT_GROBJECTS, PRM_SPEEDO_TEX, "speed360.png");

  curInst->texture = (ssgSimpleState*)grSsgLoadTexState(param);
  if (curInst->texture == 0)
    curInst->texture = (ssgSimpleState*)grSsgLoadTexState("speed360.rgb");

  free(grFilePath);

  /* Load the instrument placement */
  xSz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_XSZ, (char*)NULL, 128);
  ySz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_YSZ, (char*)NULL, 128);

  // position are delta from center of screen
  xpos = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_XPOS, (char*)NULL, 0);
  ypos = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_YPOS, (char*)NULL, 0);
  needlexSz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_NDLXSZ, (char*)NULL, 50);
  needleySz = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_NDLYSZ, (char*)NULL, 2);
  curInst->needleXCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_XCENTER, (char*)NULL, xSz / 2.0) + xpos;
  curInst->needleYCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_YCENTER, (char*)NULL, ySz / 2.0) + ypos;
  curInst->digitXCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_XDIGITCENTER, (char*)NULL, xSz / 2.0) + xpos;
  curInst->digitYCenter = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_YDIGITCENTER, (char*)NULL, 10) + ypos;
  curInst->minValue = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_MINVAL, (char*)NULL, 0);
  curInst->maxValue = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_MAXVAL, (char*)NULL, 100) - curInst->minValue;
  curInst->minAngle = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_MINANG, "deg", 225);
  curInst->maxAngle = GfParmGetNum(handle, SECT_GROBJECTS, PRM_SPEEDO_MAXANG, "deg", -45) - curInst->minAngle;
  curInst->monitored = &(car->_speed_x);
  curInst->prevVal = curInst->minAngle;
  if (strcmp(GfParmGetStr(handle, SECT_GROBJECTS, PRM_SPEEDO_DIGITAL, "yes"), "yes") == 0) {
    curInst->digital = 1;
  }

  /* Get colour to use for needle, default is Red */
  curInst->needleColor[0] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_RED, (char*)NULL, 1.0);
  curInst->needleColor[1] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_GREEN, (char*)NULL, 0.0);
  curInst->needleColor[2] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_BLUE, (char*)NULL, 0.0);
  curInst->needleColor[3] = GfParmGetNum(handle, SECT_GROBJECTS,
                                PRM_NEEDLE_ALPHA, (char*)NULL, 1.0);

  curInst->CounterList = glGenLists(1);
  glNewList(curInst->CounterList, GL_COMPILE);
  glBegin(GL_TRIANGLE_STRIP);
  {
    glColor4f(1.0, 1.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);
    glVertex2f(xpos, ypos);
    glTexCoord2f(0.0, 1.0);
    glVertex2f(xpos, ypos + ySz);
    glTexCoord2f(1.0, 0.0);
    glVertex2f(xpos + xSz, ypos);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(xpos + xSz, ypos + ySz);
  }
  glEnd();
  glEndList();

  curInst->needleList = glGenLists(1);
  glNewList(curInst->needleList, GL_COMPILE);
  glBegin(GL_TRIANGLE_STRIP);
  {
    glColor4f(curInst->needleColor[0], curInst->needleColor[1],
                curInst->needleColor[2], curInst->needleColor[3]);
    glVertex2f(0, -needleySz);
    glVertex2f(0, needleySz);
    glVertex2f(needlexSz, -needleySz / 2.0);
    glVertex2f(needlexSz, needleySz / 2.0);
  }
  glEnd();
  glEndList();
}   // grInitBoardCar

void cGrBoard::ReadDashColor(void *hdle, const string &color_name, float **color) {
  char buf[1024];
  snprintf(buf, sizeof(buf), "%s/%s/%s",
        GFSCR_SECT_DASHSETTINGS, GFSCR_DASH_COLORS, color_name.c_str());
  delete [] *color;
  *color = new float[4];
  for (int i = 0; i < 4; ++i) {
    (*color)[i] = GfParmGetNum(hdle, buf, rgba[i].c_str(), NULL, 1.0);
  }
}