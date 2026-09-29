import cv2

# Resmi okutuyoruz (Dosya adının klasördekiyle birebir aynı olduğundan emin ol)
dosya_adi = "Hafta2_Gorev1/BEEDF2F5-261F-4FE6-B9C3-FF0BB9C527BC_4_5005_c.jpeg"
img = cv2.imread(dosya_adi)

# Resim doğru okundu mu diye kontrol edelim
if img is None:
    print("Hata: Resim bulunamadı! Dosyanın aynı klasörde olduğundan emin ol.")
else:
    # Orijinal boyutları alıp ekrana yazdıralım
    orijinal_h, orijinal_w = img.shape[:2]
    print(f"Orijinal Çözünürlük: {orijinal_w}x{orijinal_h}")

    # Hocanın istediği boyutlandırma oranları
    oranlar = [2.0, 4.0, 0.5, 0.25]

    for oran in oranlar:
        # Görüntü işleme kuralı: Küçültürken INTER_AREA, büyütürken INTER_LINEAR veya CUBIC daha pürüzsüz sonuç verir
        interpolasyon = cv2.INTER_AREA if oran < 1.0 else cv2.INTER_LINEAR

        # Resmi belirlediğimiz oranda (fx ve fy) boyutlandırıyoruz
        yeni_img = cv2.resize(img, None, fx=oran, fy=oran, interpolation=interpolasyon)

        # Yeni boyutları terminalde görelim
        yeni_h, yeni_w = yeni_img.shape[:2]
        print(f"Oran x{oran} -> Yeni Çözünürlük: {yeni_w}x{yeni_h}")

        # Çıktıları klasöre kaydedelim (Hocaya kanıt olarak sunmak için iyi olur)
        kayit_isim = f"boyutlandirilmis_x{oran}.jpeg"
        cv2.imwrite(kayit_isim, yeni_img)

        # Ekranda gösterelim (Büyük resimler ekrana sığsın diye WINDOW_NORMAL kullanıyoruz)
        pencere_adi = f"Oran: x{oran}"
        cv2.namedWindow(pencere_adi, cv2.WINDOW_NORMAL)
        cv2.imshow(pencere_adi, yeni_img)

    print("\nTüm resimler klasöre kaydedildi!")
    print("Açılan pencereleri kapatmak için herhangi bir pencere seçiliyken klavyeden bir tuşa bas.")
    
    # Pencerelerin kapanması için bir tuşa basılmasını bekle
    cv2.waitKey(0)
    cv2.destroyAllWindows()
