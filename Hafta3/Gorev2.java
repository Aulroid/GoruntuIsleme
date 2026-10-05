import java.io.File;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

public class Gorev2 {
    public static void main(String[] args) {
        // 1. OpenCV DLL dosyasını yüklüyoruz
        System.load(new File("lib/opencv_java500.dll").getAbsolutePath());

        // 2. Giriş ve çıkış yolları
        String inputPath = "Hafta3_gorev2.jpg";
        String outputPath = "Hafta3_gorev2_sonuc.jpeg";

        // Görüntüyü tek kanallı (grayscale) olarak oku
        Mat src = Imgcodecs.imread(inputPath, Imgcodecs.IMREAD_GRAYSCALE);

        if (src.empty()) {
            System.err.println("Hata: Görüntü okunamadı! Dosya adını kontrol edin: " + inputPath);
            return;
        }

        int rows = src.rows();
        int cols = src.cols();
        int totalPixels = rows * cols;

        // 3. 256-değerli Histogram Çıkarma (Hazır fonksiyonsuz)
        int[] histogram = new int[256];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int intensity = (int) src.get(i, j)[0];
                histogram[intensity]++;
            }
        }

        // 4. Kümülatif Dağılım Fonksiyonu (CDF) Hesabı (Hazır fonksiyonsuz)
        int[] cdf = new int[256];
        cdf[0] = histogram[0];
        for (int i = 1; i < 256; i++) {
            cdf[i] = cdf[i - 1] + histogram[i];
        }

        // 0'dan büyük ilk CDF değerini (cdfMin) buluyoruz
        int cdfMin = 0;
        for (int i = 0; i < 256; i++) {
            if (cdf[i] > 0) {
                cdfMin = cdf[i];
                break;
            }
        }

        System.out.println("Toplam Piksel Sayısı: " + totalPixels);
        System.out.println("CDF Minimum Değeri: " + cdfMin);

        // 5. Eşitleme Haritası (Lookup Table) Oluşturma
        int[] lut = new int[256];
        for (int i = 0; i < 256; i++) {
            if (totalPixels - cdfMin == 0) {
                lut[i] = i;
            } else {
                lut[i] = (int) Math.round(((double) (cdf[i] - cdfMin) / (totalPixels - cdfMin)) * 255.0);
            }
            // 0 - 255 sınır kontrolü
            if (lut[i] < 0) lut[i] = 0;
            if (lut[i] > 255) lut[i] = 255;
        }

        // 6. Yeni Matrise Eşitlenmiş Değerleri Yazma
        Mat dst = new Mat(rows, cols, CvType.CV_8UC1);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int eskiDeger = (int) src.get(i, j)[0];
                int yeniDeger = lut[eskiDeger];
                dst.put(i, j, yeniDeger);
            }
        }

        // 7. Çıktı görüntüsünü kaydet
        boolean kaydedildi = Imgcodecs.imwrite(outputPath, dst);
        if (kaydedildi) {
            System.out.println("Başarılı: Histogram eşitlenmiş görüntü kaydedildi -> " + outputPath);
        } else {
            System.err.println("Hata: Görüntü kaydedilemedi!");
        }
    }
}