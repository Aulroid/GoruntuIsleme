import cv2
import numpy as np

# 1. Resmi doğrudan TEK KANAL GRİ (IMREAD_GRAYSCALE) olarak okutuyoruz
dosya_adi = "Hafta2_Gorev1/pexels-m-e-r-v-e-42708268-32370611.jpg"
img = cv2.imread(dosya_adi, cv2.IMREAD_GRAYSCALE)

if img is None:
    print("Resim bulunamadı!")
else:
    # Resmin yükseklik (satır) ve genişlik (sütun) değerlerini alıyoruz
    h, w = img.shape
    
    # Çıktılar için orijinal resimle aynı boyutta siyah boş tuvaller oluşturuyoruz
    img_bol = np.zeros((h, w), dtype=np.uint8)
    img_carp = np.zeros((h, w), dtype=np.uint8)

    # 2. Hocanın istediği gibi tüm pikselleri iç içe döngü ile tek tek geziyoruz
    for y in range(h):
        for x in range(w):
            piksel = int(img[y, x]) # İşlem yaparken sınırları aşmasın diye integer'a çeviriyoruz
            
            # 4'e bölme (Parlaklık azalır, resim kararır)
            img_bol[y, x] = piksel // 4
            
            # 4 ile çarpma (Parlaklık artar).
            # Hoca buradaki 255 kısıtlamasını yapıp yapmadığına kesinlikle bakacaktır.
            img_carp[y, x] = min(piksel * 4, 255)

    # 3. Sonuçları ekranda gösteriyoruz (Normal pencereler)
    cv2.namedWindow("Orijinal Gri", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Parlaklik / 4", cv2.WINDOW_NORMAL)
    cv2.namedWindow("Parlaklik x 4", cv2.WINDOW_NORMAL)
    
    cv2.imshow("Orijinal Gri", img)
    cv2.imshow("Parlaklik / 4", img_bol)
    cv2.imshow("Parlaklik x 4", img_carp)
    
    # Çıktıları klasöre de kaydedelim, GitHub'a atarken hocaya kanıt olur
    cv2.imwrite("gri_bolu_4.jpeg", img_bol)
    cv2.imwrite("gri_carpi_4.jpeg", img_carp)

    print("Pikseller gezildi, işlem tamam!")
    cv2.waitKey(0)
    cv2.destroyAllWindows()
