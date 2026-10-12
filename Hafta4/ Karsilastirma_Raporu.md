# Kenar Bulma Filtreleri ve Algoritmaları Karşılaştırma Raporu

Bu raporda Prewitt, Sobel, Laplacian, Laplacian of Gaussian (LoG) ve OpenCV Canny algoritmalarının sonuçları kıyaslanmıştır.

## 1. Sobel ve Prewitt Operatörlerinin Kıyaslaması
Her iki operatör de birinci dereceden türev kullanarak gradyan hesaplar. Prewitt yatay ve dikey yöndeki tüm piksellere eşit ağırlık (1) verirken, Sobel merkez piksellere daha fazla ağırlık (2) verir. Görsel olarak birbirlerine çok benzeseler de, Sobel'in merkez ağırlıklı yapısı onu görüntüdeki küçük kumlanmalara (gürültüye) karşı Prewitt'e kıyasla daha dayanıklı yapar. Gürültülü fotoğraflarda Sobel daha pürüzsüz kenarlar verir.

## 2. Laplacian Kernelinin Sobel ve Prewitt ile Kıyaslaması
Sobel ve Prewitt birinci dereceden türev alırken, Laplacian ikinci dereceden türev alır. Bu durum Laplacian'ı ani değişimlere karşı çok daha hassas yapar. Ayrıca Sobel ve Prewitt kenarların yönünü bulurken, Laplacian yönden bağımsızdır (izotropik). İkinci türev, gürültüyü şiddetlendirdiği için Laplacian uygulanmadan önce Gauss filtresi ile yumuşatma yapmak zorunludur; aksi takdirde resimdeki tüm pürüzler kenar olarak algılanır.

## 3. 7x7 LoG (Laplacian of Gaussian) Analizi
Görüntüye 7x7 gibi geniş bir Gauss filtresi uygulamak, görüntüyü ciddi oranda bulanıklaştırır ve ince detayları yok eder. Ardından uygulanan Laplacian işlemi, sadece geriye kalan en kalın, en baskın kenarları bulur. Standart filtrelerde ince detaylar (saç, çimen vb.) kenar olarak algılanırken, 7x7 LoG filtresinde bu mikro yapılar kaybolur, sadece objelerin ana dış hatları ortaya çıkar.

## 4. OpenCV Canny Algoritması Karşılaştırması
Canny, tekil bir filtreden ziyade çok aşamalı bir algoritmadır. NMS adımı sayesinde kalın kenarları tek piksellik ince çizgilere dönüştürür. Hysteresis ile de kesik çizgileri birleştirir. Sonuç olarak en net ve sürekli kenar haritasını Canny üretir. Ancak içerdiği döngüler nedeniyle Sobel ve Laplacian gibi basit konvolüsyon işlemlerine göre çok daha yavaş çalışır. Hızın kritik olduğu yerlerde Sobel, hassasiyetin gerektiği yerlerde Canny tercih edilmelidir.
