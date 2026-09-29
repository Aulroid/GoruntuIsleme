import cv2
import numpy as np

# N değerini buradan istediğin gibi değiştirebilirsin (Örn: resmi 4 parçaya bölelim)
N = 4 

# Resmi bir önceki Görev 2 klasöründen çekiyoruz, tekrar kopyalamana gerek yok!
dosya_adi = "Hafta2_Gorev2/pexels-m-e-r-v-e-42708268-32370611.jpg"
img = cv2.imread(dosya_adi, cv2.IMREAD_GRAYSCALE)

if img is None:
    print("Resim bulunamadı! Dosya yolunu kontrol et.")
else:
    h, w = img.shape
    parca_genisligi = w // N
    
    # İşlenen küçük parçaları hafızada tutacağımız boş listeler
    bolunmus_parcalar_bol = []
    bolunmus_parcalar_carp = []

    print(f"Resim dikey olarak {N} eşit parçaya bölünüyor ve işleniyor...")

    # N defa dönecek bir döngü kurup resmi dilimliyoruz
    for i in range(N):
        # Her bir parçanın başlangıç ve bitiş X (sütun) koordinatlarını hesaplıyoruz
        baslangic_x = i * parca_genisligi
        # Son parçadaysak bölmeden kalan küsuratları kaçırmamak için genişliğin sonuna (w) kadar alıyoruz
        bitis_x = w if i == (N - 1) else (i + 1) * parca_genisligi
        
        # Görüntü matrisini dilimliyoruz (Sadece o aralıktaki pikselleri kopardık)
        parca = img[:, baslangic_x:bitis_x]
        
        # Görev 2'deki aynı işlemi bu minik parçaya uyguluyoruz
        parca_bol = parca // 4
        # 255 sınırını aşmamak (taşma hatası) için yine minimum fonksiyonu ile güvenceye alıyoruz
        parca_carp = np.minimum(parca.astype(np.uint16) * 4, 255).astype(np.uint8)
        
        # İşlenmiş hazır parçaları listelere ekliyoruz
        bolunmus_parcalar_bol.append(parca_bol)
        bolunmus_parcalar_carp.append(parca_carp)
    
    # Tüm işlem bitince listelerdeki o yapboz parçalarını yan yana (yatayda) tekrar yapıştırıyoruz
    sonuc_bol = np.hstack(bolunmus_parcalar_bol)
    sonuc_carp = np.hstack(bolunmus_parcalar_carp)

    # Sonuçları ekranda göster
    cv2.namedWindow("Birlestirilmis: Bolme", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Birlestirilmis: Carpma", cv2.WINDOW_NORMAL)

    cv2.imshow("Birlestirilmis: Bolme", sonuc_bol)
    cv2.imshow("Birlestirilmis: Carpma", sonuc_carp)

    # Çıktıları bu sefer direkt Görev 3 klasörünün içine kaydediyoruz
    cv2.imwrite("Hafta2_Gorev3/parcali_bolme.jpeg", sonuc_bol)
    cv2.imwrite("Hafta2_Gorev3/parcali_carpma.jpeg", sonuc_carp)

    print("İşlem tamamlandı, resim birleştirildi!")
    cv2.waitKey(0)
    cv2.destroyAllWindows()
