# Audio Signal Analysis and Processing Tool

Proiect dezvoltat în mediul **LabWindows/CVI** pentru analiza și prelucrarea digitală a semnalelor audio, acoperind atât domeniul timp, cât și cel al frecvenței.

## 🛠️ Tehnologii și Mediul de Lucru
* **Limbaj:** ANSI C.
* **Platformă:** LabWindows/CVI.
* **Biblioteci:** `analysis.h` (DSP, Statistici, Filtrare).
* **Format Date:** Procesare eșantioane extrase din fișiere `.wav` (`waveInfo.txt`, `waveData.txt`).

## 🚀 Funcționalități Principale

### 1. Analiza în Domeniul Timp
Permite extragerea și vizualizarea informațiilor fundamentale direct din semnalul audio:
* **Parametri Statistici:** Calculul mediei, medianei, dispersiei și a valorilor extreme (Maxim/Minim) cu indicii aferenți.
* **Analiza Distribuției:** Calculul momentelor de ordin superior (**Skewness** și **Kurtosis**) și generarea histogramei amplitudunilor.
* **Detecția Trecerilor prin Zero:** Estimarea frecvenței fundamentale prin contorizarea schimbărilor de semn ale eșantioanelor.
* **Navigație și Vizualizare:**
    * Vizualizarea semnalului complet sau segmentat pe intervale de o secundă.
    * Funcționalitate de parcurgere dinamică (Next/Prev) prin vectorul de date.
* **Filtrare în Timp:** * Filtru de mediere (Moving Average) cu ferestre glisante de 16 sau 32.
    * Filtru de ordinul 1 (Alpha) pentru netezirea semnalului și reducerea zgomotului de înaltă frecvență.

### 2. Analiza în Domeniul Frecvență
Procesare avansată utilizând tehnici de analiză spectrală:
* **Transformata Fourier (FFT):** Analiză pe dimensiuni de ferestre configurabile (1024, 2048 sau 4096 eșantioane).
* **Filtrare Digitală Avansată:**
    * **Bessel Band-Pass:** Ordin 4 sau 8 pentru izolarea benzii 1000-1200 Hz, asigurând un grup de întârziere constant și reducerea distorsiunilor.
    * **Savitzky-Golay:** Netezirea spectrului pentru o reprezentare mai clară a frecvențelor.
* **Optimizare DSP:**
    * **Decimare (Factor D=2):** Reducerea frecvenței de eșantionare pentru a obține o rezoluție spectrală mult mai fină.
    * **Ferestruire:** Aplicarea ferestrelor **Blackman** sau **Triangle** pentru minimizarea fenomenului de *spectral leakage*.
* **Calculul Fazei:** Extragerea structurii unghiulare a componentelor spectrale utilizând părțile reale și imaginare rezultate din FFT:
  $$phi[i] = \text{atan2}(WfI[i], WfR[i])$$

## 📊 Rezultate și Automatizare
* **Estimări în Timp Real:** Identificarea automată a frecvenței dominante și a puterii de vârf (Vrms).
* **Validare Vizuală:** Grafice comparative între spectrul brut și cel filtrat, alături de reprezentarea fazei.
* **Sistem de Export:** Salvarea automată a graficelor sub formă de fişiere JPEG cu marcaj temporal (timestamp) pentru documentarea experimentelor.

