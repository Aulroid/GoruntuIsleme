# Canny Kenar Bulma Algoritması Adımları ve Gereklilikleri

Sıfırdan yazdığımız Canny algoritmasının 4 temel adımının neden gerekli olduğunu aşağıda özetledim:

1. **Gürültü Bastırma (Gauss Filtresi):** Fotoğraflarda gözle zor görülen kumlanmalar vardır. Türev tabanlı algoritmalar piksel değerlerindeki ani değişimleri aradığı için, eğer fotoğrafı Gauss ile hafifçe bulanıklaştırmazsak algoritma bu kumlanmaları da kenar zanneder. Gürültüyü silip hatalı kenar bulmayı engellemek için ilk adımda Gauss şarttır.

2. **Gradyan Hesaplama (Sobel):** Kenar dediğimiz yapı aslında rengin koyudan açığa aniden değiştiği yerlerdir. Sobel filtresi uygulayarak bu değişimin türevini alıyoruz. Böylece resmin neresinde, hangi yöne doğru ne kadar şiddetli bir renk değişimi (kenar) olduğunu tespit ediyoruz.

3. **Non-Maximum Suppression (NMS):** Sobel filtresi kenarları bulur ama bu kenarlar genelde birkaç piksel kalınlığında ve bulanık çıkar. Bizim tek piksellik ince ve net bir çizgiye ihtiyacımız var. NMS, bu kalın çizginin üzerindeki en yüksek değere sahip tepe noktasını tutup, yanındaki daha zayıf pikselleri sıfırlar (siyaha boyar). Böylece kenar incelmiş olur.

4. **Hysteresis Thresholding:** NMS sonrasında kenarlar ince çıksa da bazen kesik kesik olabilir. Hysteresis aşamasında yüksek ve düşük iki sınır (eşik) belirleriz. Yüksek sınırı geçenler kesin kenardır. Düşük sınırda kalan pikseller ise, sadece kesin bir kenara temas ediyorsa (onun devamıysa) kenar kabul edilir, değmiyorsa gürültü denilip silinir. Bu sayede kopuk çizgiler birbirine bağlanır ve temiz bir sonuç elde edilir.

