# Hafta 9 Ödev

## Derleme

```bash
g++ -std=c++17 gorev_1/main.cpp -o gorev_1/gorev_1 $(pkg-config --cflags --libs opencv4)
g++ -std=c++17 gorev_2/main.cpp -o gorev_2/gorev_2 $(pkg-config --cflags --libs opencv4)
```

## Görev 1 - Hareketli Bölgede (ROI) Ten Tespiti

1. Web kamerası OpenCV ile açılır.
2. Ardışık iki kare gri tona çevrilir, Gauss ile bulanıklaştırılır ve farkı alınır (`cv::absdiff`).
3. Fark eşiklenir; açma ve genişletme ile gürültü temizlenir.
4. Hareketli piksel sayısı `MIN_HAREKETLI_PIKSEL` (1500) değerini geçerse hareket var sayılır.
   Hareketli konturların sınırlayıcı kutuları **ROI** olur.
5. ROI dışında kalan kısım **arka plan** kabul edilir ve soluk gri gösterilir.
6. Ten tespiti **sadece ROI içinde** yapılır: YCrCb (`Cr: 133-173`, `Cb: 77-127`) ve HSV (`H: 0-25`) maskeleri birlikte kullanılır.
7. Ten pikselleri yeşile boyanır.

```bash
./gorev_1/gorev_1            # varsayılan kamera
./gorev_1/gorev_1 1          # kamera indeksi
./gorev_1/gorev_1 video.mp4  # video dosyası
```

Tuşlar: `q`/`ESC` çıkış, `s` sonucu kaydet.

## Görev 2 - Kırmızı Çemberli Trafik İşaretleri

Veri seti: [Traffic Signs Dataset (Germany, UK, USA)](https://www.kaggle.com/datasets/tabishalam0/traffic-signs-dataset-germany-uk-usa)

1. **Farklı renk uzayları** ile kırmızı maske:
   - **HSV**: kırmızı ton iki uçtadır, iki aralık kullanılır (`0-10` ve `160-180`).
   - **Lab**: `a*` kanalı ≥ 150.
   - **YCrCb**: `Cr` kanalı ≥ 155.
   - Bir piksel en az 2 renk uzayında kırmızıysa kırmızı kabul edilir (çoğunluk oyu).
2. **Hough Çember Dönüşümü** (`cv::HoughCircles`) hem gri görüntüde hem kırmızı maskede çalışır.
3. Her aday çember doğrulanır:
   - Çember çeperindeki kırmızı piksel oranı ≥ 0.35
   - Çevredeki açı dilimlerinin en az %80'inde kırmızı var (üçgenleri eler)
   - Çemberin hemen dışındaki kırmızı oranı ≤ 0.30 (büyük kırmızı alanlardaki sahte çemberleri eler)
4. İç içe çakışan adaylardan en büyüğü tutulur.

```bash
./gorev_2/gorev_2 veri_seti/ sonuclar/   # klasör: işaretli görüntüler + ozet.csv
./gorev_2/gorev_2 resim.jpg --goster     # tek görüntü, pencerede göster
```
