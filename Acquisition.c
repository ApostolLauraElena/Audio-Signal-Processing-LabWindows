//==============================================================================
//
// Title:		Acquisition.c
// Purpose:		A short description of the implementation.
//
// Created on:	11/5/2020 at 5:01:06 PM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "Acquisition.h"
#include "SwitchPanels.h"
#include "toolbox.h"

#include <formatio.h>
#include <utility.h>
#include <analysis.h>

extern double *waveData;
extern int sampleRate;
extern int nframes;

static int currentIndex = 0;

int CVICALLBACK OnAcquisitionPanelCB (int panel, int event, void *callbackData,
									  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnTimer (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_TIMER_TICK)
    {
        if (waveData == NULL || nframes <= 0) return 0;

        int nFFT;
        GetCtrlVal(panel, ACQ_PANEL_NPOINTS, &nFFT); 

		int D = 2; 
        double fs_down = (double)sampleRate / D;
		
        if (currentIndex + (nFFT *D) > nframes) {
            currentIndex = 0; 
        }

        // 1. ALOCARE MEMORIE
		double *rawSegment = malloc(nFFT * D * sizeof(double));
        double *filteredSegment = malloc(nFFT * D* sizeof(double));
        double *autoSpectrum = malloc((nFFT/ 2) * sizeof(double));
        double *convertedSpectrum = malloc((nFFT/ 2) * sizeof(double));
		double *autoSpectrumRaw = malloc((nFFT / 2) * sizeof(double));
        double *convertedSpectrumRaw = malloc((nFFT / 2) * sizeof(double));
        double *downsampled = malloc(nFFT * sizeof(double));
		
		if (!filteredSegment || !autoSpectrum || !downsampled|| !convertedSpectrum|| !autoSpectrumRaw || !convertedSpectrumRaw || !rawSegment) {
            if(filteredSegment) free(filteredSegment);
            if(autoSpectrum) free(autoSpectrum);
            if(convertedSpectrum) free(convertedSpectrum);
			if(autoSpectrumRaw) free(autoSpectrumRaw);
            if(convertedSpectrumRaw) free(convertedSpectrumRaw);
			if(rawSegment) free(rawSegment);
            return 0;
        }

        Copy1D(&waveData[currentIndex], nFFT* D, rawSegment);

        int filterType, windowType;
        GetCtrlVal(panel, ACQ_PANEL_FILTER_TYPE, &filterType); // 0=RAW, 1=SG, 2=B4, 3=B8
        GetCtrlVal(panel, ACQ_PANEL_WINDOW_TYPE, &windowType);

        // 2. FILTRARE ( Bessel  1000-1200Hz de ordin 4 sau 8 sau Savitzky-Golay)
        if (filterType == 1) { 
            // Savitzky-Golay pentru netezire
            SavitzkyGolayFiltering(rawSegment, nFFT* D, 2, 5, NULL, filteredSegment);
        } else if (filterType == 2) { 
            // Bessel Ord 4 Trece-Banda (1000-1200 Hz)
            Bssl_BPF(rawSegment, nFFT * D, (double)sampleRate, 1000.0, 1200.0, 4, filteredSegment);
        } else if (filterType == 3) { 
            // Bessel Ord 8 Trece-Banda (1000-1200 Hz)
            Bssl_BPF(rawSegment, nFFT * D, (double)sampleRate, 1000.0, 1200.0, 8, filteredSegment);
        } else { 
            // RAW
            Copy1D(rawSegment, nFFT * D, filteredSegment);
        }
		//  DECIMARE (Downsampling propriu-zis)
        Decimate(filteredSegment, nFFT * D, D, 0 , downsampled);
		double *rawDown = malloc(nFFT * sizeof(double));
		Decimate(rawSegment, nFFT * D, D, 0, rawDown);
        // 3. FERESTRUIRE
        WindowConst winConst;
        int cviWinType = (windowType == 0) ? BLKMAN : TRIANGLE;
        ScaledWindowEx(downsampled, nFFT, cviWinType, 0, &winConst); 
		ScaledWindowEx(rawDown, nFFT, cviWinType, 0, &winConst);
        // 4. CALCUL SPECTRU 
        double df;
        AutoPowerSpectrum(downsampled, nFFT, 1.0 / fs_down, autoSpectrum, &df); 
		AutoPowerSpectrum(rawDown, nFFT, 1.0 / fs_down, autoSpectrumRaw, &df);
        // 5. ESTIMARE VARF
        double peakFreq, peakPower;
        PowerFrequencyEstimate(autoSpectrum, nFFT / 2, -1.0, winConst, df, 7, &peakFreq, &peakPower); 
        
        SetCtrlVal(panel, ACQ_PANEL_PEAK_FREQ, peakFreq); 
        SetCtrlVal(panel, ACQ_PANEL_PEAK_POWER, peakPower); 

        // 6. CONVERSIE
        char unitString[32] = "V";
		SpectrumUnitConversion(autoSpectrum, nFFT / 2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unitString); 
        SpectrumUnitConversion(autoSpectrumRaw, nFFT / 2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrumRaw, unitString); 

        // 7. AFISARE GRAFIC 
        DeleteGraphPlot(panel, ACQ_PANEL_GRAPH, -1, VAL_IMMEDIATE_DRAW);
		DeleteGraphPlot(panel, ACQ_PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
        PlotWaveform(panel, ACQ_PANEL_GRAPH_2, convertedSpectrum, nFFT / 2, VAL_DOUBLE, 
                     1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
		PlotWaveform(panel, ACQ_PANEL_GRAPH, convertedSpectrumRaw, nFFT / 2, VAL_DOUBLE, 
                     1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
		
        // 8. SALVARE IMAGINE 
				
		int bitmapID, bitmapIDf;
		char fileName[512];
		int luna, ziua, anul, ore, minute, secunde;

		GetSystemDate(&luna, &ziua, &anul);
		GetSystemTime(&ore, &minute, &secunde);

		sprintf(fileName, "ProiectAPDOutput\\Spectru_%02d-%02d-%04d_%02d-%02d-%02d.jpg", ziua, luna, anul, ore, minute, secunde);

		GetCtrlDisplayBitmap(panel, ACQ_PANEL_GRAPH, 1, &bitmapID);
		SaveBitmapToJPEGFile(bitmapID, fileName, JPEG_PROGRESSIVE, 100);
		DiscardBitmap(bitmapID);
		
		sprintf(fileName, "ProiectAPDOutput\\Spectru_filtrat_%02d-%02d-%04d_%02d-%02d-%02d.jpg", ziua, luna, anul, ore, minute, secunde);

		GetCtrlDisplayBitmap(panel, ACQ_PANEL_GRAPH_2, 1, &bitmapIDf);
		SaveBitmapToJPEGFile(bitmapIDf, fileName, JPEG_PROGRESSIVE, 100);
		DiscardBitmap(bitmapIDf);
		
        currentIndex += (nFFT * D);
		//Bonus
		double *WfR = malloc(nFFT * sizeof(double));
		double *WfI = malloc(nFFT * sizeof(double));
		double *phase = malloc(nFFT * sizeof(double));

		for(int i = 0; i < nFFT; i++) {
		    WfR[i] = downsampled[i];
		    WfI[i] = 0.0;               
		}

		FFT(WfR, WfI, nFFT); 
		for(int i = 0; i < nFFT / 2; i++) {
		    // atan2 returneaza faza i n radiani
		    phase[i] = atan2(WfI[i], WfR[i]); 
		}

		DeleteGraphPlot(panel, ACQ_PANEL_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
		PlotWaveform(panel, ACQ_PANEL_GRAPH_3, phase, nFFT / 2, VAL_DOUBLE, 
		             1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);

		// 9. ELIBERARE MEMORIE
		free(WfR);
		free(WfI);
		free(phase);
         
        free(filteredSegment);
        free(autoSpectrum);
        free(convertedSpectrum);
		free(autoSpectrumRaw);
        free(convertedSpectrumRaw);
		free(rawSegment);
		free(downsampled); free(rawDown);
    }
    return 0;
}
int CVICALLBACK OnStart (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	int state = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, control, &state);
			SetCtrlAttribute(panel, ACQ_PANEL_TIMER, ATTR_ENABLED, state);
			break;
	}
	return 0;
}
