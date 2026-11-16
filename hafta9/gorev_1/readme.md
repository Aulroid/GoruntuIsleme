cat << 'EOF' > hafta9/gorev_1/README.md
# Görev 1: Görüntü İşlemede Yaygın Kullanılan Renk Uzayları

## 1. RGB (Red, Green, Blue)
* **Yapısı:** Eklemeli (additive) renk modelidir. Kırmızı, yeşil ve mavi ışık bileşenlerinin farklı yoğunluklarda birleşmesiyle renkler elde edilir.
* **Avantajları:** Donanım (monitör, kamera sensörleri) ile doğrudan uyumludur, ek bir matematiksel dönüşüm gerektirmez.
* **Dezavantajları:** Parlaklık (luminance) ve renk (chrominance) bilgisi birbirine karışıktır. Işık değişimlerine karşı çok hassastır; nesne takibi veya renk tabanlı segmentasyon için uygun değildir.
* **Kullanım Alanları:** Görüntü yakalama, ekranda gösterme, temel grafik işlemleri.

## 2. HSV (Hue, Saturation, Value) / HSI (Hue, Saturation, Intensity)
* **Yapısı:** İnsan renk algısına daha yakın silindirik/konik bir modeldir.
  * **Hue (Öz/Ton):** Rengin türünü (0-179 veya 0-360 derece) belirtir.
  * **Saturation (Doygunluk):** Rengin saflığını/canlılığını belirtir.
  * **Value/Intensity (Parlaklık/Yoğunluk):** Işık şiddetini belirtir.
* **Avantajları:** Renk bilgisini (H) parlaklık bilgisinden (V/I) ayırır. Aydınlatma değişimlerinden daha az etkilenir.
* **Dezavantajları:** Düşük doygunluk veya aşırı parlaklık/karanlık durumlarında Hue değeri kararsızlaşabilir (tekillik problemi).
* **Kullanım Alanları:** Renk tabanlı nesne tespiti ve takibi, şerit takibi, ten rengi algılama, görüntü iyileştirme.

## 3. YUV / YCbCr
* **Yapısı:** **Y** parlaklık (luma) bileşenini, **U/V** (veya **Cb/Cr**) ise renk farkı (chroma) bileşenlerini tutar.
* **Avantajları:** İnsan gözü parlaklık değişimlerine renk değişimlerinden daha duyarlıdır. Bu sayede renk kanalları sıkıştırılarak (chroma subsampling - 4:2:0 gibi) görüntü kalitesinde belirgin kayıp olmadan ciddi veri tasarrufu sağlanır.
* **Dezavantajları:** Monitörde göstermek için RGB'ye dönüştürülmesi gerekir.
* **Kullanım Alanları:** Video sıkıştırma (MPEG, H.264, H.265), televizyon yayıncılığı, JPEG sıkıştırma ve ten rengi tespiti.

## 4. CIE L*a*b*
* **Yapısı:** Cihazdan bağımsız (device-independent) bir renk uzayıdır. **L*** açıklık/parlaklık, **a*** yeşil-kırmızı ekseni, **b*** mavi-sarı eksenidir.
* **Avantajları:** Algısal olarak doğrusaldır (perceptually uniform); yani iki renk arasındaki Öklid mesafesi insan gözünün algıladığı renk farkıyla orantılıdır. Çok geniş bir renk gamutuna sahiptir.
* **Dezavantajları:** RGB'den dönüşümü doğrusal değildir ve işlem maliyeti diğerlerine göre yüksektir.
* **Kullanım Alanları:** Renk kalibrasyonu, baskı teknolojileri, renk farkı ölçümü, gelişmiş görüntü segmentasyonu ve renk transferi.

## 5. Grayscale (Gri Tonlama)
* **Yapısı:** Tek kanallıdır (genellikle 8-bit, 0-255 arası değerler). Yalnızca ışık yoğunluğu bilgisini taşır.
* **Avantajları:** 3 kanallı görüntülere göre 3 kat daha az bellek kaplar ve işlem hızını ciddi oranda artırır.
* **Dezavantajları:** Renk bilgisi tamamen kaybolur.
* **Kullanım Alanları:** Kenar tespiti (Canny, Sobel), morfolojik işlemler, OCR (optik karakter tanıma), yüz/nesne tanıma ön işlemleri.
EOF