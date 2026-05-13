/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  ACQ_PANEL                        1       /* callback function: OnAcquisitionPanelCB */
#define  ACQ_PANEL_IDC_SWITCH_PANEL       2       /* control type: binary, callback function: OnSwitchPanelCB */
#define  ACQ_PANEL_GRAPH_3                3       /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_GRAPH_2                4       /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_GRAPH                  5       /* control type: graph, callback function: (none) */
#define  ACQ_PANEL_WINDOW_TYPE            6       /* control type: ring, callback function: (none) */
#define  ACQ_PANEL_FILTER_TYPE            7       /* control type: ring, callback function: (none) */
#define  ACQ_PANEL_PEAK_POWER             8       /* control type: numeric, callback function: (none) */
#define  ACQ_PANEL_PEAK_FREQ              9       /* control type: numeric, callback function: (none) */
#define  ACQ_PANEL_TIMER                  10      /* control type: timer, callback function: OnTimer */
#define  ACQ_PANEL_TEXTMSG                11      /* control type: textMsg, callback function: (none) */
#define  ACQ_PANEL_NPOINTS                12      /* control type: ring, callback function: (none) */
#define  ACQ_PANEL_START_BUTTO            13      /* control type: textButton, callback function: OnStart */

#define  WAVE_PANEL                       2       /* callback function: OnWavePanelCB */
#define  WAVE_PANEL_GRAPH                 2       /* control type: graph, callback function: (none) */
#define  WAVE_PANEL_GRAPH_2               3       /* control type: graph, callback function: (none) */
#define  WAVE_PANEL_PREVBUTTON            4       /* control type: command, callback function: OnPrevButton */
#define  WAVE_PANEL_NEXTBUTTON            5       /* control type: command, callback function: OnNextButton */
#define  WAVE_PANEL_FILTER                6       /* control type: ring, callback function: (none) */
#define  WAVE_PANEL_COMMANDBUTTON_3       7       /* control type: command, callback function: OnApplyButtonCB */
#define  WAVE_PANEL_COMMANDBUTTON_4       8       /* control type: command, callback function: OnLoadButtonCB */
#define  WAVE_PANEL_TEXTMSG_2             9       /* control type: textMsg, callback function: (none) */
#define  WAVE_PANEL_IDC_SWITCH_PANEL      10      /* control type: binary, callback function: OnSwitchPanelCB */
#define  WAVE_PANEL_TEXTMSG               11      /* control type: textMsg, callback function: (none) */
#define  WAVE_PANEL_INDEX_MIN             12      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_ZEROS                 13      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_INDEX_MAX             14      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_VARIANCE              15      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MEDIAN                16      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MIN                   17      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MAX                   18      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_MEAN                  19      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_GRAPH_3               20      /* control type: graph, callback function: (none) */
#define  WAVE_PANEL_ALPHANUMERIC          21      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_SKEWNESSTEXTBOX       22      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_KURTOSISTEXTBOX       23      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_STOPTEXTBOX           24      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_STARTTEXTBOX          25      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_DIMENSIUNE            26      /* control type: numeric, callback function: (none) */
#define  WAVE_PANEL_BINARYSWITCH          27      /* control type: binary, callback function: OnBinarySwitch */
#define  WAVE_PANEL_COMMANDBUTTON         28      /* control type: command, callback function: OnAnvelopaButton */
#define  WAVE_PANEL_COMMANDBUTTON_2       29      /* control type: command, callback function: OnDerivataButton */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK OnAcquisitionPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAnvelopaButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnApplyButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnBinarySwitch(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDerivataButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoadButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNextButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrevButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStart(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitchPanelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnWavePanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif