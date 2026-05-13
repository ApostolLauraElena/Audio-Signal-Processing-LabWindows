//==============================================================================
//
// Title:		SwitchPanels
// Purpose:		A short description of the application.
//
// Created on:	11/5/2020 at 4:51:51 PM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "SwitchPanels.h"
#include "toolbox.h"

#include <formatio.h>
#include <utility.h>
#include <analysis.h>

//==============================================================================
// Constants
#define SAMPLE_RATE		0
#define NPOINTS			1
//==============================================================================
// Types

//==============================================================================
// Static global variables

int wavePanel = 0;
int acqPanel = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables
int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
int sampleRate = 0;
int npoints = 0;
double *waveData = 0;
double *waveDataCopy = 0;

int filtruAplicat = 0; // 0 = fara filtru, 1 = filtrul este activ

int selectedFilter; // 0-mediere, 1-alpha
int dimensiune; // dimensiune mediere 16 sau 32
double alpha; // alpha intre 0 si 1
double skewness, kurtosis;

int currentSecond = 0;
int totalSeconds = 0;
int numChannels = 2;    
int nframes = 0;       
//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (wavePanel = LoadPanel (0, "SwitchPanels.uir", WAVE_PANEL));
	errChk (acqPanel = LoadPanel (0, "SwitchPanels.uir", ACQ_PANEL));
	
	/* display the panel and run the user interface */
	errChk (DisplayPanel (wavePanel));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (wavePanel > 0)
		DiscardPanel (wavePanel);
	if (acqPanel > 0)
		DiscardPanel (acqPanel);
	if (waveData) free(waveData);
	if (waveDataCopy) free(waveDataCopy);
	return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK OnWavePanelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
		QuitUserInterface (0);
	return 0;
}

int CVICALLBACK OnSwitchPanelCB (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == wavePanel)
			{
				SetCtrlVal(acqPanel, ACQ_PANEL_IDC_SWITCH_PANEL, 1);
				DisplayPanel(acqPanel);
				HidePanel(panel);
			}
			else
			{
				SetCtrlVal(wavePanel, WAVE_PANEL_IDC_SWITCH_PANEL, 0);
				DisplayPanel(wavePanel);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnLoadButtonCB (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	double mean = 0.0;
	double maxVal = 0.0;
	double minVal = 0.0;
	int maxIndex = 0;
	int minIndex = 0;
	double median = 0.0;
	double dispersie = 0.0;
	double axisArray[100];
	
	switch (event)
	{
		case EVENT_COMMIT:
			//LaunchExecutable("python main.py");
			//Delay(4);
			
			FileToArray("waveInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			double *rawData = (double*)calloc(npoints, sizeof(double));
			FileToArray("waveData.txt", rawData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			numChannels = 2; 
			nframes = npoints / numChannels;

			waveData = (double*)calloc(nframes, sizeof(double));
				
			for (int i = 0; i < nframes; i++)
    			waveData[i] = rawData[i * numChannels + 0];  // folosesc si afisez doar canalul stang
			
			free(rawData);

			PlotY (panel, WAVE_PANEL_GRAPH, waveData, nframes, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
			filtruAplicat = 0;
			
			MaxMin1D (waveData, nframes, &maxVal, &maxIndex, &minVal, &minIndex);
			Median(waveData,nframes,&median);
			Variance(waveData,nframes,&mean, &dispersie);
			int count = 0;
		    int i;

		    for (i = 1; i < nframes; i++) {
		        if (waveData[i - 1] * waveData[i] < 0 || waveData[i]==0) {
		            count++;
		        }
		    }	
			int interv = 100;
			int *histograma = (int *) calloc (interv, sizeof(int));
			Histogram (waveData, nframes, minVal, maxVal, histograma, axisArray, interv);
			PlotXY (panel, WAVE_PANEL_GRAPH_3, axisArray, histograma, interv, VAL_DOUBLE, VAL_INTEGER, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_DK_CYAN);
			
			SetCtrlVal (panel, WAVE_PANEL_MEAN, mean);
			SetCtrlVal (panel, WAVE_PANEL_MEDIAN, median);
			SetCtrlVal (panel, WAVE_PANEL_VARIANCE,dispersie);
			SetCtrlVal (panel, WAVE_PANEL_MIN , minVal);
			SetCtrlVal (panel, WAVE_PANEL_MAX , maxVal);
			SetCtrlVal (panel, WAVE_PANEL_INDEX_MIN, minIndex);
			SetCtrlVal (panel, WAVE_PANEL_INDEX_MAX , maxIndex);
			SetCtrlVal (panel, WAVE_PANEL_ZEROS, count);
			
			totalSeconds = nframes / sampleRate;
			currentSecond = 0;
	
			SetCtrlVal(panel, WAVE_PANEL_STARTTEXTBOX, 0);
			SetCtrlVal(panel, WAVE_PANEL_STOPTEXTBOX, 1);

			double m2 = 0.0, m3 = 0.0, m4 = 0.0;
			Moment(waveData, 256, 2, &m2);
			Moment(waveData, 256, 3, &m3);
			Moment(waveData, 256, 4, &m4);

			if (m2 > 0.0) {
			    skewness = m3 / pow(m2, 1.5);   
			    kurtosis = m4 / pow(m2, 2.0);
			} else {
			    skewness = 0.0;
			    kurtosis = 0.0;
			}
			SetCtrlVal(panel, WAVE_PANEL_SKEWNESSTEXTBOX, skewness);
			SetCtrlVal(panel, WAVE_PANEL_KURTOSISTEXTBOX, kurtosis);

			SetCtrlAttribute (panel, WAVE_PANEL_COMMANDBUTTON_3, ATTR_DIMMED, 0);
			SetCtrlAttribute (panel, WAVE_PANEL_COMMANDBUTTON, ATTR_DIMMED, 0);
			SetCtrlAttribute (panel, WAVE_PANEL_COMMANDBUTTON_4, ATTR_DIMMED, 1);
			SetCtrlAttribute (panel, WAVE_PANEL_COMMANDBUTTON_2, ATTR_DIMMED, 0);
			SetCtrlAttribute (panel, WAVE_PANEL_BINARYSWITCH, ATTR_DIMMED, 0);
			free(histograma);
			break;
	}
	return 0;
}
void mediere(double *signal, double *filtered, int length, int dimensiune)
{
    double sum = 0;
    
    for(int i = 0; i < length; i++)
    {
        if(i < dimensiune) 
        {
            sum += signal[i];
            filtered[i] = sum / (i + 1); 
        }
        else
        {
            sum = sum - signal[i - dimensiune] + signal[i];
            filtered[i] = sum / dimensiune;
        }
    }
}


void ordin1(double *signal, double *filtered, int length, double alpha) 
{
	filtered[0] = signal[0]; 

    for (int i = 1; i < length; i++) 
	{
        filtered[i] = (1 - alpha) * filtered[i - 1] + alpha * signal[i];
    }
}

int CVICALLBACK OnApplyButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
		{
			filtruAplicat = 1;
            if (!waveDataCopy)
                waveDataCopy = (double*) calloc(nframes, sizeof(double));
            GetCtrlVal(panel, WAVE_PANEL_FILTER, &selectedFilter);

            if (selectedFilter == 1) 
			{
                GetCtrlVal(panel, WAVE_PANEL_DIMENSIUNE, &dimensiune);
            } else 
			{
                GetCtrlVal(panel, WAVE_PANEL_ALPHANUMERIC, &alpha);
            }

            if (selectedFilter == 1) 
			{
                mediere(waveData, waveDataCopy, nframes, dimensiune);
            } else 
			{
                ordin1(waveData, waveDataCopy, nframes, alpha);
            }

            int oneSecMode = 0;
            GetCtrlVal(panel, WAVE_PANEL_BINARYSWITCH, &oneSecMode);

            int startFrame = 0;
            int length = nframes;
            if (oneSecMode == 1) {
                startFrame = currentSecond * sampleRate;
                length = sampleRate;
                if (startFrame + length > nframes) length = nframes - startFrame;
            }

            DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
            PlotY(panel, WAVE_PANEL_GRAPH_2, &waveDataCopy[startFrame], length, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
			break;

        }
    }
    return 0;
}

int CVICALLBACK OnNextButton(int panel, int control, int event,
                             void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
		{
			if (currentSecond < totalSeconds - 1)
    			currentSecond++;

		    int startIndex = currentSecond * sampleRate;
		    int stopIndex = startIndex + sampleRate;
		    if (stopIndex > nframes)
		        stopIndex = nframes;

		    SetCtrlVal(panel, WAVE_PANEL_STARTTEXTBOX, currentSecond);
		    SetCtrlVal(panel, WAVE_PANEL_STOPTEXTBOX, currentSecond + 1);

		    
		    int currentLength = stopIndex - startIndex;
			
		    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
		    PlotY(panel, WAVE_PANEL_GRAPH, &waveData[startIndex], currentLength, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);

			char fileName[256];
			int bitmapID;

			sprintf(fileName, "ProiectAPDOutput\\original_secunda_%d.jpg", currentSecond);
			GetCtrlDisplayBitmap(panel, WAVE_PANEL_GRAPH, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
			
			if (filtruAplicat && waveDataCopy)
				{
				    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				    PlotY(panel, WAVE_PANEL_GRAPH_2, &waveDataCopy[startIndex], currentLength, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
					
					sprintf(fileName, "ProiectAPDOutput\\filtrat_secunda_%d.jpg", currentSecond);
					GetCtrlDisplayBitmap(panel, WAVE_PANEL_GRAPH_2, 1, &bitmapID);
					SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
				}
			else
				{
				    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				}
			
		    break;
		}
    }
    return 0;
}

int CVICALLBACK OnPrevButton(int panel, int control, int event,
                             void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
		{
		    currentSecond--;
		    if (currentSecond < 0)
		        currentSecond = 0;

		    int startIndex = currentSecond * sampleRate;
		    int stopIndex = startIndex + sampleRate;
		    if (stopIndex > nframes)
		        stopIndex = nframes;

		    SetCtrlVal(panel, WAVE_PANEL_STARTTEXTBOX, currentSecond);
		    SetCtrlVal(panel, WAVE_PANEL_STOPTEXTBOX, currentSecond + 1);

		    int currentLength = stopIndex - startIndex;

		    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
		    PlotY(panel, WAVE_PANEL_GRAPH, &waveData[startIndex], currentLength, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			char fileName[256];
			int bitmapID;

			sprintf(fileName, "ProiectAPDOutput\\original_secunda_%d.jpg", currentSecond);
			GetCtrlDisplayBitmap(panel, WAVE_PANEL_GRAPH, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);

			if (filtruAplicat && waveDataCopy)
			{
			    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
			    PlotY(panel, WAVE_PANEL_GRAPH_2, &waveDataCopy[startIndex], currentLength, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
				sprintf(fileName, "ProiectAPDOutput\\filtrat_secunda_%d.jpg", currentSecond);
				GetCtrlDisplayBitmap(panel, WAVE_PANEL_GRAPH_2, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
			} else
			{
				DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
			}
		    break;
		}
    }
    return 0;
}
int CVICALLBACK OnBinarySwitch (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int val;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, WAVE_PANEL_BINARYSWITCH, &val);

			if(val == 1)
			{
				SetCtrlAttribute(panel, WAVE_PANEL_NEXTBUTTON, ATTR_DIMMED, 0);
				SetCtrlAttribute(panel, WAVE_PANEL_PREVBUTTON, ATTR_DIMMED, 0);
				SetCtrlAttribute (panel, WAVE_PANEL_COMMANDBUTTON, ATTR_DIMMED, 1);
				SetCtrlAttribute (panel, WAVE_PANEL_COMMANDBUTTON_2, ATTR_DIMMED, 1);
				
				currentSecond = 0;

				int startIndex = 0;
				int stopIndex = sampleRate;
				if(stopIndex > nframes) stopIndex = nframes;
				int length = stopIndex - startIndex;

				SetCtrlVal(panel, WAVE_PANEL_STARTTEXTBOX, 0);
				SetCtrlVal(panel, WAVE_PANEL_STOPTEXTBOX, 1);

				DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, WAVE_PANEL_GRAPH, &waveData[startIndex], length, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				if (!waveDataCopy){
			        waveDataCopy = (double*) calloc(nframes, sizeof(double));
					for (int i = 0; i < nframes; i++)
			        	waveDataCopy[i] = waveData[i];					
				}
			    
				if (filtruAplicat && waveDataCopy)
				{
				    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				    PlotY(panel, WAVE_PANEL_GRAPH_2, &waveDataCopy[startIndex], length, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
				}
				else
				{
				    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				}
			}
			else
			{
				
				SetCtrlAttribute(panel, WAVE_PANEL_NEXTBUTTON, ATTR_DIMMED, 1);
				SetCtrlAttribute(panel, WAVE_PANEL_PREVBUTTON, ATTR_DIMMED, 1);
				SetCtrlAttribute(panel, WAVE_PANEL_COMMANDBUTTON, ATTR_DIMMED, 0);
				SetCtrlAttribute(panel, WAVE_PANEL_COMMANDBUTTON_2, ATTR_DIMMED, 0);
				
				SetCtrlVal(panel, WAVE_PANEL_STARTTEXTBOX, 0);
				SetCtrlVal(panel, WAVE_PANEL_STOPTEXTBOX, totalSeconds);

				DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, WAVE_PANEL_GRAPH, waveData, nframes, VAL_DOUBLE,
					  VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);

				if(waveDataCopy && filtruAplicat)
				{
					DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, WAVE_PANEL_GRAPH_2, waveDataCopy, nframes, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
				}
				else
				{
				    DeleteGraphPlot(panel, WAVE_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				}
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnAnvelopaButton (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
				ssize_t count1 = 0, count2 = 0;
	            double *peakLocations = NULL, *peakAmplitudes = NULL, *peakDerivatives = NULL;
	            double *valleyLocations = NULL, *valleyAmplitudes = NULL, *valleyDerivatives = NULL;
				PeakDetector(waveData, nframes, 0.0, 3, 0, 1, 1, &count1, &peakLocations, &peakAmplitudes, &peakDerivatives);
				PeakDetector(waveData, nframes, 0.0, 3, 1, 1, 1, &count2, &valleyLocations, &valleyAmplitudes, &valleyDerivatives);		
				
				DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_NO_DRAW);
				
				PlotY (panel, WAVE_PANEL_GRAPH, waveData, nframes, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				PlotXY (panel, WAVE_PANEL_GRAPH, peakLocations, peakAmplitudes, count1, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
				PlotXY (panel, WAVE_PANEL_GRAPH, valleyLocations, valleyAmplitudes, count2, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_ASTERISK, VAL_SOLID, VAL_CONNECTED_POINTS,VAL_GREEN);
			break;
	}
	return 0;
}
int CVICALLBACK OnDerivataButton (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			double *output = 0;
			if (!output)
                output = (double*) calloc(nframes, sizeof(double));

			for (int i = 0; i < nframes - 1; i++) {
			    output[i] = waveData[i + 1] - waveData[i];
			}
			output[nframes - 1] = output[nframes - 2];

			DeleteGraphPlot(panel, WAVE_PANEL_GRAPH, -1, VAL_NO_DRAW);
			PlotY (panel, WAVE_PANEL_GRAPH, output, nframes, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_MAGENTA);
			if (output) free(output);
			break;
	}
	return 0;
}