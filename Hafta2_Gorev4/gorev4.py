import cv2
import numpy as np

# N değerini (parça sayısını) belirliyoruz
N = 4 

dosya_adi = "Hafta2_Gorev2/pexels-m-e-r-v-e-42708268-32370611.jpg"
img = cv2.imread(dosya_adi, cv2.IMREAD_GRAYSCALE)

if img is None:
    print("Resim bulunamadı! Dosya yolunu kontrol et.")
else:
    h, w = img.shape
    parca_genisligi = w // N
    
    # 1. BÜTÜN RESME HİSTOGRAM EŞİTLEME (GLOBAL KONTRAST)
    # Bu işlem, tüm resmin histogramını çıkarıp karanlık/aydınlık dengesini otomatik ayarlar
    img_esitlenmis_tam = cv2.equalizeHist(img)
    
    # 2. RESMİ N PARÇAYA BÖLÜP HER BİRİNE AYRI AYRI EŞİTLEME (LOKAL KONTRAST)
    esitlenmis_parcalar = []
    
    for i in range(N):
        baslangic_x = i * parca_genisligi
        bitis_x = w if i == (N - 1) else (i + 1) * parca_genisligi
        
        # Parçayı kopar
        parca = img[:, baslangic_x:bitis_x]
        
        # Sadece bu minik parçanın kendi histogramına göre eşitleme (kontrast artırma) yap
        parca_esitlenmis = cv2.equalizeHist(parca)
        
        # İşlenmiş parçayı listeye ekle
        esitlenmis_parcalar.append(parca_esitlenmis)
        
    # 3. İŞLENMİŞ PARÇALARI TEKRAR YAN YANA BİRLEŞTİR
    img_esitlenmis_parcali = np.hstack(esitlenmis_parcalar)
    
    # 4. GÖRSELLEŞTİRME (RESMİN NASIL DEĞİŞTİĞİNİ GÖSTERME)
    cv2.namedWindow("Orijinal Resim", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Tum Resim Esitlenmis", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Parcali Esitlenmis (N=4)", cv2.WINDOW_NORMAL)
    
    cv2.imshow("Orijinal Resim", img)
    cv2.imshow("Tum Resim Esitlenmis", img_esitlenmis_tam)
    cv2.imshow("Parcali Esitlenmis (N=4)", img_esitlenmis_parcali)
    
    # Çıktıları klasöre kaydet
    cv2.imwrite("Hafta2_Gorev4/esitlenmis_tam.jpeg", img_esitlenmis_tam)
    cv2.imwrite("Hafta2_Gorev4/esitlenmis_parcali.jpeg", img_esitlenmis_parcali)

    print("Histogram eşitleme işlemi tamamlandı! Değişen resimler ekranda.")
    cv2.waitKey(0)
    cv2.destroyAllWindows()
