import java.io.File;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

public class Gorev4 {
    public static void main(String[] args) {
        // 1. OpenCV DLL kütüphanesini yüklüyoruz
        System.load(new File("lib/opencv_java500.dll").getAbsolutePath());

        // Giriş ve çıkış yolları
        String inputPath = "low_CONTRAST.jpg";
        String outputPath = "gurultusu_bastirilmis_sonuc.jpeg";

        // Görüntüyü tek kanallı (grayscale) olarak oku
        Mat src = Imgcodecs.imread(inputPath, Imgcodecs.IMREAD_GRAYSCALE);

        if (src.empty()) {
            System.err.println("Hata: Görüntü okunamadı! Dosya adını kontrol edin: " + inputPath);
            return;
        }

        int rows = src.rows();
        int cols = src.cols();

        // Çıktı görüntüsü için boş matris oluştur
        Mat dst = new Mat(rows, cols, CvType.CV_8UC1);

        // 2. 3x3 Konvolüsyon ile Ortalama Filtresi Gezdirme
        for (int r = 1; r < rows - 1; r++) {
            for (int c = 1; c < cols - 1; c++) {
                double toplam = 0.0;

                // 3x3 komşuluktaki 9 pikseli topla
                for (int kr = -1; kr <= 1; kr++) {
                    for (int kc = -1; kc <= 1; kc++) {
                        toplam += src.get(r + kr, c + kc)[0];
                    }
                }

                // 9'a bölerek ortalamasını al
                int ortalamaDeger = (int) Math.round(toplam / 9.0);

                // 0 - 255 aralığına sınırla
                if (ortalamaDeger < 0) ortalamaDeger = 0;
                if (ortalamaDeger > 255) ortalamaDeger = 255;

                dst.put(r, c, ortalamaDeger);
            }
        }

        // 3. Kenar (Border) piksellerini doldur
        for (int c = 0; c < cols; c++) {
            dst.put(0, c, src.get(0, c)[0]);
            dst.put(rows - 1, c, src.get(rows - 1, c)[0]);
        }
        for (int r = 0; r < rows; r++) {
            dst.put(r, 0, src.get(r, 0)[0]);
            dst.put(r, cols - 1, src.get(r, cols - 1)[0]);
        }

        // 4. Sonuç görüntüsünü kaydet
        boolean kaydedildi = Imgcodecs.imwrite(outputPath, dst);

        if (kaydedildi) {
            System.out.println("Gorev 4 basariyla tamamlandi.");
            System.out.println("Filtrelenmis sonuc kaydedildi: " + outputPath);
        } else {
            System.err.println("Hata: Dosya kaydedilemedi!");
        }
    }
}