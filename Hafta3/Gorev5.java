import java.io.File;
import java.util.Arrays;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Gorev5 {
    public static void main(String[] args) {
        // 1. OpenCV DLL dosyasını yüklüyoruz
        System.load(new File("lib/opencv_java500.dll").getAbsolutePath());

        // Giriş ve çıkış dosya yolları
        String inputPath = "salt_and_pepper_gorev5.jpg";
        String outHazirPath = "medyan_hazir_5x5_sonuc.jpeg";
        String outManuelPath = "medyan_manuel_5x5_sonuc.jpeg";

        // Görüntüyü tek kanallı (grayscale) olarak oku
        Mat src = Imgcodecs.imread(inputPath, Imgcodecs.IMREAD_GRAYSCALE);

        if (src.empty()) {
            System.err.println("Hata: Görüntü bulunamadı! '" + inputPath + "' dosyasını proje kök dizinine ekleyin.");
            return;
        }

        int rows = src.rows();
        int cols = src.cols();

        // ==========================================
        // BÖLÜM 1: OpenCV Hazır Medyan Filtresi (5x5)
        // ==========================================
        Mat dstHazir = new Mat();
        // ksize = 5 (5x5 pencere)
        Imgproc.medianBlur(src, dstHazir, 5);
        Imgcodecs.imwrite(outHazirPath, dstHazir);
        System.out.println("1. OpenCV hazır medyan çıktısı kaydedildi: " + outHazirPath);

        // ==========================================
        // BÖLÜM 2: Manuel 5x5 Medyan Filtresi
        // ==========================================
        Mat dstManuel = new Mat(rows, cols, CvType.CV_8UC1);

        // 5x5 için çekirdek yarıçapı: offset = 2 (-2, -1, 0, 1, 2)
        int offset = 2;
        int windowSize = 25; // 5 x 5 = 25 piksel
        int[] window = new int[windowSize];

        for (int r = offset; r < rows - offset; r++) {
            for (int c = offset; c < cols - offset; c++) {
                int index = 0;

                // 5x5 komşuluktaki 25 pikseli topla
                for (int kr = -offset; kr <= offset; kr++) {
                    for (int kc = -offset; kc <= offset; kc++) {
                        window[index++] = (int) src.get(r + kr, c + kc)[0];
                    }
                }

                // Değerleri küçükten büyüğe sırala
                Arrays.sort(window);

                // Ortadaki (12. indeks) medyan değeri merkeze ata
                int medyanDeger = window[12];
                dstManuel.put(r, c, medyanDeger);
            }
        }

        // Sınır piksellerini orijinal görüntüden aktar
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (r < offset || r >= rows - offset || c < offset || c >= cols - offset) {
                    dstManuel.put(r, c, src.get(r, c)[0]);
                }
            }
        }

        Imgcodecs.imwrite(outManuelPath, dstManuel);
        System.out.println("2. Manuel 5x5 medyan çıktısı kaydedildi: " + outManuelPath);
    }
}