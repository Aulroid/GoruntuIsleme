
import java.io.File;
import org.opencv.core.Core;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

public class Gorev1 {
    public static void main(String[] args) {
        // 1. OpenCV DLL kütüphanesini yüklüyoruz
        System.load(new File("lib/opencv_java500.dll").getAbsolutePath());

        // 2. Giriş ve çıkış dosya yolları
        String inputPath = "a-low-contrast-image-b-after-enhancement.jpeg";
        String outputPath = "dogrusal_kontrast_sonuc.jpeg";

        // Görüntüyü tek kanallı (grayscale) olarak okuyoruz
        Mat src = Imgcodecs.imread(inputPath, Imgcodecs.IMREAD_GRAYSCALE);

        if (src.empty()) {
            System.err.println("Hata: Görüntü okunamadı! Dosyanın kök dizinde olduğundan emin olun: " + inputPath);
            return;
        }

        // 3. Resmin mevcut Minimum ve Maksimum piksel değerlerini buluyoruz
        MinMaxLocResult mmr = Core.minMaxLoc(src);
        double minVal = mmr.minVal;
        double maxVal = mmr.maxVal;

        System.out.println("=== Orijinal Resim Değerleri ===");
        System.out.println("Mevcut Minimum Piksel : " + minVal);
        System.out.println("Mevcut Maksimum Piksel : " + maxVal);

        // 4. Doğrusal Kontrast Germe: Min -> 0, Max -> 255
        int rows = src.rows();
        int cols = src.cols();
        Mat dst = new Mat(rows, cols, CvType.CV_8UC1);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                double[] pixel = src.get(i, j);
                double pEski = pixel[0];

                // Formül: ((P - Min) / (Max - Min)) * 255
                double pYeni = ((pEski - minVal) / (maxVal - minVal)) * 255.0;

                // Sınır kontrolü (0 - 255)
                if (pYeni < 0) pYeni = 0;
                if (pYeni > 255) pYeni = 255;

                dst.put(i, j, pYeni);
            }
        }

        // 5. Yeni minimum ve maksimum değerleri kontrol ediyoruz
        MinMaxLocResult mmrYeni = Core.minMaxLoc(dst);
        System.out.println("\n=== Kontrast Germe Sonrası Değerler ===");
        System.out.println("Yeni Minimum Piksel : " + mmrYeni.minVal + " (Hedef: 0)");
        System.out.println("Yeni Maksimum Piksel : " + mmrYeni.maxVal + " (Hedef: 255)");

        // 6. Sonuç görüntüsünü kaydediyoruz
        boolean kaydedildi = Imgcodecs.imwrite(outputPath, dst);

        if (kaydedildi) {
            System.out.println("\nİşlem başarılı! İyileştirilen görsel kaydedildi: " + outputPath);
        } else {
            System.err.println("\nHata: Çıktı dosyası kaydedilemedi!");
        }
    }
}