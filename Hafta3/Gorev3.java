import java.io.File;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class Gorev3 {
    public static void main(String[] args) {
        // 1. OpenCV DLL dosyasını yüklüyoruz
        System.load(new File("lib/opencv_java500.dll").getAbsolutePath());

        // 2. 3x3 Düşük Kontrastlı Örnek Matris (Tek kanallı - CV_8UC1)
        Mat src = new Mat(3, 3, CvType.CV_8UC1);
        
        // Örnek düşük kontrastlı piksel değerleri (birbirine yakın değerler)
        byte[] data = new byte[]{
            50, 52, 50,
            55, 52, 58,
            52, 50, 60
        };
        src.put(0, 0, data);

        System.out.println("=== Orijinal 3x3 Matris ===");
        matrisiYazdir(src);

        // 3. Histogram Çıkarma (0-255 aralığı)
        int[] hist = new int[256];
        int totalPixels = 3 * 3; // 9 piksel

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int val = (int) src.get(i, j)[0];
                hist[val]++;
            }
        }

        // 4. Clip Limit Uygulama (Histogram Kırpma)
        // 9 piksel için clipLimit eşiğini 2 olarak belirliyoruz
        int clipLimit = 2; 
        int excess = 0;

        for (int i = 0; i < 256; i++) {
            if (hist[i] > clipLimit) {
                excess += (hist[i] - clipLimit);
                hist[i] = clipLimit;
            }
        }

        // Kırpılan fazlalığı histogramın geneline dağıtıyoruz
        int bonus = excess / 256;
        int remainder = excess % 256;
        for (int i = 0; i < 256; i++) {
            hist[i] += bonus;
            if (i < remainder) {
                hist[i]++;
            }
        }

        // 5. CDF (Kümülatif Dağılım Fonksiyonu) Hesabı
        int[] cdf = new int[256];
        cdf[0] = hist[0];
        for (int i = 1; i < 256; i++) {
            cdf[i] = cdf[i - 1] + hist[i];
        }

        // 0'dan büyük ilk CDF değeri
        int cdfMin = 0;
        for (int i = 0; i < 256; i++) {
            if (cdf[i] > 0) {
                cdfMin = cdf[i];
                break;
            }
        }

        // 6. Eşitleme Haritası (LUT) ve Yeni Matris Değerleri
        int[] lut = new int[256];
        for (int i = 0; i < 256; i++) {
            if (totalPixels - cdfMin <= 0) {
                lut[i] = i;
            } else {
                int val = (int) Math.round(((double) (cdf[i] - cdfMin) / (totalPixels - cdfMin)) * 255.0);
                lut[i] = Math.min(255, Math.max(0, val));
            }
        }

        Mat dst = new Mat(3, 3, CvType.CV_8UC1);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int eskiDeger = (int) src.get(i, j)[0];
                int yeniDeger = lut[eskiDeger];
                dst.put(i, j, yeniDeger);
            }
        }

        System.out.println("\n=== 3x3 CLAHE Sonrası Matris ===");
        matrisiYazdir(dst);
    }

    // Matrisi konsola düzenli tablo gibi yazdıran yardımcı fonksiyon
    private static void matrisiYazdir(Mat m) {
        for (int r = 0; r < m.rows(); r++) {
            for (int c = 0; c < m.cols(); c++) {
                System.out.printf("%4d ", (int) m.get(r, c)[0]);
            }
            System.out.println();
        }
    }
}