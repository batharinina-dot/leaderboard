// ===========================================================================
//  APLIKASI LEADERBOARD / PEMERINGKAT SKOR GAME
//  Tugas Individu UTS - Pemrograman Lanjut
//
//  Compile : g++ -O2 -std=c++17 leaderboard.cpp -o leaderboard
//  Jalankan: 
// ===========================================================================
//
//  PETA PROGRAM (menu -> fungsi yang bekerja -> poin tugas)
//  ---------------------------------------------------------------------------
//  MENU 1  Tambah pemain          tambahPemain(), validasiSkor(),
//                                 tampilPesanError(), hitungSkorAkhir()   -> POIN 1
//  MENU 2  Tampilkan leaderboard  tampilkanPeringkat()
//  MENU 3  Cari pemain            cariPemain(), binarySearchIter/Rek()    -> POIN 2
//  MENU 4  Statistik              tampilkanStatistik(), totalIter/Rek(),
//                                 maxIter/Rek()                           -> POIN 2
//  MENU 5  Isi data contoh        isiDataContoh()
//  MENU 6  Analisa iter vs rek    analisaIterRek()                        -> POIN 3
//  MENU 7  Uji 3 sorting          ujiSorting(), bubbleSort(),
//                                 mergeSort(), quickSort()                -> POIN 4
//  ---------------------------------------------------------------------------
//  Istilah:
//   - FUNGSI   = blok kode yang MENGEMBALIKAN nilai (int, bool, dll).
//   - PROSEDUR = blok kode yang TIDAK mengembalikan nilai (bertipe void).
// ===========================================================================

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <limits>
using namespace std;
using namespace std::chrono;

// ===========================================================================
// BAGIAN 0 : DATA DAN UTILITAS (dipakai oleh semua menu)
// ===========================================================================

// Satu pemain = satu nama + satu skor.
struct Pemain {
    string nama;
    int skor;
};

// Tempat menyimpan semua pemain. Diisi oleh MENU 1 dan MENU 5,
// dibaca oleh MENU 2, 3, dan 4.
vector<Pemain> daftar;

// Pengukur waktu: menjalankan kode "f" lalu mengembalikan lamanya (milidetik).
// Dipakai oleh MENU 6 dan MENU 7.
template <typename F>
double ukurMs(F f) {
    auto mulai = high_resolution_clock::now();
    f();
    auto selesai = high_resolution_clock::now();
    return duration<double, milli>(selesai - mulai).count();
}

// Membaca angka dari keyboard dengan aman.
// Kalau user mengetik huruf, program tidak crash, tetapi meminta input lagi.
int bacaInt(const string& prompt) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Input harus angka.\n";
    }
}

// ===========================================================================
// MENU 1 : TAMBAH PEMAIN                                    (POIN TUGAS 1)
// ===========================================================================
// Yang terjadi saat user memilih menu 1:
//
//   tambahPemain()                        <- PROSEDUR (void)
//     |-- validasiSkor(skor)              <- FUNGSI, dipanggil oleh prosedur
//     |      '-- tampilPesanError(...)    <- PROSEDUR, dipanggil oleh fungsi
//     '-- hitungSkorAkhir(skor, combo)    <- FUNGSI, dipanggil oleh prosedur
//            '-- bonusComboRek(combo)     <- FUNGSI rekursif (POIN 2)
//
// Jadi kedua arah ada di sini:
//   * PROSEDUR memanggil FUNGSI : tambahPemain()  -> validasiSkor()
//   * FUNGSI memanggil PROSEDUR : validasiSkor()  -> tampilPesanError()
// ===========================================================================

// ---- PROSEDUR: menampilkan pesan kesalahan (tidak mengembalikan nilai) ----
void tampilPesanError(const string& pesan) {
    cout << "  [!] " << pesan << "\n";
}

// ---- FUNGSI yang MEMANGGIL PROSEDUR ----
// Mengembalikan true jika skor valid (0 - 1.000.000), false jika tidak.
// Jika tidak valid, fungsi ini memanggil prosedur tampilPesanError().
bool validasiSkor(int skor) {
    if (skor < 0 || skor > 1000000) {
        tampilPesanError("Skor harus di antara 0 dan 1.000.000");  // memanggil prosedur
        return false;
    }
    return true;
}

// ---- Bonus combo = 10 + 20 + 30 + ... + (n x 10) ----
// Contoh: n = 3  ->  10 + 20 + 30 = 60
// Dibuat dalam dua versi untuk POIN 2 (iteratif vs rekursif).

// Versi ITERATIF: memakai perulangan for.
int bonusComboIter(int n) {
    int total = 0;
    for (int i = 1; i <= n; i++) total += i * 10;
    return total;
}

// Versi REKURSIF: fungsi memanggil dirinya sendiri dengan n yang lebih kecil.
//   bonusComboRek(3) = 30 + bonusComboRek(2)
//                    = 30 + 20 + bonusComboRek(1)
//                    = 30 + 20 + 10 + bonusComboRek(0)
//                    = 30 + 20 + 10 + 0 = 60
int bonusComboRek(int n) {
    if (n <= 0) return 0;                  // BASE CASE: berhenti di sini
    return n * 10 + bonusComboRek(n - 1);  // RECURSIVE CASE: panggil diri sendiri
}

// ---- FUNGSI: skor akhir = skor dasar + bonus combo ----
int hitungSkorAkhir(int skorDasar, int combo) {
    return skorDasar + bonusComboRek(combo);
}

// ---- PROSEDUR yang MEMANGGIL FUNGSI : inti MENU 1 ----
void tambahPemain() {
    // 1) Ambil nama dari user
    string nama;
    cout << "Nama pemain : ";
    getline(cin, nama);

    // 2) Ambil skor dasar, lalu periksa dengan FUNGSI validasiSkor()
    int skor = bacaInt("Skor dasar  : ");
    if (!validasiSkor(skor)) return;  // skor salah -> pesan error muncul, batal

    // 3) Ambil combo (dibatasi 0-1000 agar rekursi tidak terlalu dalam)
    int combo = bacaInt("Combo (0-1000): ");
    if (combo < 0 || combo > 1000) {
        tampilPesanError("Combo harus 0 - 1000");
        return;
    }

    // 4) Hitung skor akhir dengan FUNGSI hitungSkorAkhir(), simpan ke daftar
    int akhir = hitungSkorAkhir(skor, combo);
    daftar.push_back({nama, akhir});
    cout << "  Berhasil! Skor akhir " << nama << " = " << akhir
         << " (bonus combo " << akhir - skor << ")\n";
}

// ===========================================================================
// MENU 2 : TAMPILKAN LEADERBOARD
// ===========================================================================
// Cara kerja:
//   1. Salin daftar pemain, lalu urutkan dari skor TERBESAR ke terkecil.
//   2. Cetak dengan nomor peringkat.
//   3. Skor yang sama mendapat peringkat yang sama (contoh: 1, 2, 2, 4).
// Data asli di "daftar" tidak berubah karena yang diurutkan hanya salinannya.
// ===========================================================================
void tampilkanPeringkat() {
    if (daftar.empty()) { tampilPesanError("Belum ada data pemain."); return; }

    vector<Pemain> urut = daftar;  // salinan
    sort(urut.begin(), urut.end(),
         [](const Pemain& a, const Pemain& b) { return a.skor > b.skor; });  // menurun

    cout << "\n========== LEADERBOARD ==========\n";
    cout << left << setw(6) << "Rank" << setw(20) << "Nama" << "Skor\n";
    cout << "---------------------------------\n";
    int rank = 1;
    for (size_t i = 0; i < urut.size(); i++) {
        // Naikkan rank hanya jika skor lebih kecil dari pemain sebelumnya
        if (i > 0 && urut[i].skor < urut[i - 1].skor) rank = (int)i + 1;
        cout << left << setw(6) << rank << setw(20) << urut[i].nama << urut[i].skor << "\n";
    }
    cout << "=================================\n";
}

// ===========================================================================
// MENU 3 : CARI PEMAIN BERDASARKAN SKOR (BINARY SEARCH)     (POIN TUGAS 2)
// ===========================================================================
// Binary search: cari dengan membuang SETENGAH data di setiap langkah.
// Syarat: data HARUS sudah terurut (di sini terurut naik).
//
// Contoh mencari 7 di [1, 3, 5, 7, 9]:
//   tengah = 5  -> 7 > 5, buang kiri, cari di [7, 9]
//   tengah = 7  -> ketemu!
// Untuk 100.000 data, maksimal hanya ~17 langkah.
// ===========================================================================

// Versi ITERATIF: perulangan while, batas kiri/kanan digeser.
int binarySearchIter(const vector<int>& a, int target) {
    int kiri = 0, kanan = (int)a.size() - 1;
    while (kiri <= kanan) {
        int tengah = kiri + (kanan - kiri) / 2;
        if (a[tengah] == target) return tengah;   // ketemu
        if (a[tengah] < target) kiri = tengah + 1;  // cari di kanan
        else kanan = tengah - 1;                    // cari di kiri
    }
    return -1;  // tidak ketemu
}

// Versi REKURSIF: fungsi memanggil dirinya dengan rentang yang lebih sempit.
int binarySearchRek(const vector<int>& a, int target, int kiri, int kanan) {
    if (kiri > kanan) return -1;                  // BASE CASE 1: tidak ketemu
    int tengah = kiri + (kanan - kiri) / 2;
    if (a[tengah] == target) return tengah;       // BASE CASE 2: ketemu
    if (a[tengah] < target)
        return binarySearchRek(a, target, tengah + 1, kanan);  // cari di kanan
    return binarySearchRek(a, target, kiri, tengah - 1);       // cari di kiri
}

// Prosedur MENU 3: urutkan naik, cari dengan kedua versi, tampilkan hasilnya.
void cariPemain() {
    if (daftar.empty()) { tampilPesanError("Belum ada data pemain."); return; }

    // Urutkan NAIK dulu (syarat binary search)
    vector<Pemain> urut = daftar;
    sort(urut.begin(), urut.end(),
         [](const Pemain& a, const Pemain& b) { return a.skor < b.skor; });
    vector<int> skor;
    for (auto& p : urut) skor.push_back(p.skor);

    int target = bacaInt("Cari pemain dengan skor: ");
    int i1 = binarySearchIter(skor, target);                         // iteratif
    int i2 = binarySearchRek(skor, target, 0, (int)skor.size() - 1); // rekursif
    if (i1 == -1) { cout << "  Tidak ditemukan.\n"; return; }
    cout << "  Ditemukan (iteratif) : " << urut[i1].nama << " - " << urut[i1].skor << "\n";
    cout << "  Ditemukan (rekursif) : " << urut[i2].nama << " - " << urut[i2].skor << "\n";
}

// ===========================================================================
// MENU 4 : STATISTIK (TOTAL, TERTINGGI, RATA-RATA)          (POIN TUGAS 2)
// ===========================================================================
// Dua masalah diselesaikan dengan DUA cara (iteratif dan rekursif):
//   - Total skor semua pemain
//   - Skor tertinggi
// Hasil kedua cara harus SAMA, itu yang ditunjukkan di layar.
// ===========================================================================

// Mengambil semua skor dari daftar pemain menjadi array angka.
vector<int> ambilSkor() {
    vector<int> s;
    for (auto& p : daftar) s.push_back(p.skor);
    return s;
}

// ---- TOTAL ----
// ITERATIF: jumlahkan satu per satu dengan loop.
long long totalIter(const vector<int>& a) {
    long long total = 0;
    for (size_t i = 0; i < a.size(); i++) total += a[i];
    return total;
}
// REKURSIF: total(n) = elemen terakhir + total(n-1)
//   total([5,3,8], 3) = 8 + total([5,3], 2) = 8 + 3 + total([5], 1) = 8 + 3 + 5 + 0
long long totalRek(const vector<int>& a, size_t n) {
    if (n == 0) return 0;                       // BASE CASE: tidak ada elemen
    return a[n - 1] + totalRek(a, n - 1);       // RECURSIVE CASE
}

// ---- SKOR TERTINGGI ----
// ITERATIF: anggap elemen pertama terbesar, bandingkan dengan sisanya.
int maxIter(const vector<int>& a) {
    int mx = a[0];
    for (size_t i = 1; i < a.size(); i++)
        if (a[i] > mx) mx = a[i];
    return mx;
}
// REKURSIF: max(n) = yang lebih besar antara elemen terakhir dan max(n-1)
int maxRek(const vector<int>& a, size_t n) {
    if (n == 1) return a[0];                    // BASE CASE: tinggal satu elemen
    return max(a[n - 1], maxRek(a, n - 1));     // RECURSIVE CASE
}

// Prosedur MENU 4: memanggil keempat fungsi di atas dan menampilkan hasilnya.
void tampilkanStatistik() {
    if (daftar.empty()) { tampilPesanError("Belum ada data pemain."); return; }
    vector<int> s = ambilSkor();
    cout << "\n--- Statistik (iteratif vs rekursif) ---\n";
    cout << "Total skor    : " << totalIter(s) << " (iteratif) | "
         << totalRek(s, s.size()) << " (rekursif)\n";
    cout << "Skor tertinggi: " << maxIter(s) << " (iteratif) | "
         << maxRek(s, s.size()) << " (rekursif)\n";
    cout << "Rata-rata     : " << fixed << setprecision(2)
         << (double)totalIter(s) / s.size() << "\n";
}

// ===========================================================================
// MENU 5 : ISI DATA CONTOH
// ===========================================================================
// Menambahkan 8 pemain dengan skor acak 1000-9999 agar demo tidak perlu
// mengetik satu per satu. Angka acak memakai seed tetap (7), jadi hasilnya
// selalu sama setiap kali dijalankan.
// ===========================================================================
void isiDataContoh() {
    string nama[] = {"Andi", "Budi", "Citra", "Dewi", "Eko", "Fajar", "Gita", "Hana"};
    mt19937 rng(7);
    for (auto& n : nama) daftar.push_back({n, (int)(rng() % 9000 + 1000)});
    cout << "  8 data contoh ditambahkan.\n";
}

// ===========================================================================
// MENU 6 : ANALISA ITERATIF vs REKURSIF                     (POIN TUGAS 3)
// ===========================================================================
// Yang dibandingkan: waktu eksekusi iteratif vs rekursif pada
//   (a) total dan max dengan N = 1.000, 5.000, 10.000
//   (b) binary search pada 100.000 data
// Setiap uji diulang ULANG kali supaya waktunya cukup besar untuk diukur
// (satu kali jalan terlalu cepat, hasilnya 0.000 ms).
//
// Rekursi linear (total/max) HANYA diuji sampai 10.000 karena rekursi sedalam
// ratusan ribu level berisiko STACK OVERFLOW (program crash).
// ===========================================================================
volatile long long sink;  // penampung hasil agar compiler tidak membuang perhitungan

void analisaIterRek() {
    cout << "\n===== ANALISA ITERATIF vs REKURSIF =====\n";
    cout << "Setiap pengujian diulang 5000x, waktu = total ms.\n\n";
    cout << left << setw(10) << "N" << setw(18) << "Total (iter)" << setw(18) << "Total (rek)"
         << setw(18) << "Max (iter)" << "Max (rek)\n";
    cout << string(78, '-') << "\n";

    // --- Bagian (a): total dan max pada beberapa ukuran data ---
    mt19937 rng(42);
    int ukuran[] = {1000, 5000, 10000};
    for (int n : ukuran) {
        vector<int> a(n);
        for (auto& x : a) x = rng() % 1000001;  // skor acak 0 - 1.000.000

        const int ULANG = 5000;  // jumlah pengulangan agar waktu terukur
        double t1 = ukurMs([&] { for (int k = 0; k < ULANG; k++) sink = totalIter(a); });
        double t2 = ukurMs([&] { for (int k = 0; k < ULANG; k++) sink = totalRek(a, a.size()); });
        double t3 = ukurMs([&] { for (int k = 0; k < ULANG; k++) sink = maxIter(a); });
        double t4 = ukurMs([&] { for (int k = 0; k < ULANG; k++) sink = maxRek(a, a.size()); });
        cout << left << setw(10) << n << fixed << setprecision(3)
             << setw(18) << t1 << setw(18) << t2 << setw(18) << t3 << t4 << "\n";
    }

    // --- Bagian (b): binary search pada 100.000 data, 100.000 pencarian ---
    int N = 100000;
    vector<int> b(N);
    for (int i = 0; i < N; i++) b[i] = i * 3;  // data sudah terurut naik
    double ts1 = ukurMs([&] { long long s = 0; for (int q = 0; q < N; q++) s += binarySearchIter(b, q * 3); sink = s; });
    double ts2 = ukurMs([&] { long long s = 0; for (int q = 0; q < N; q++) s += binarySearchRek(b, q * 3, 0, N - 1); sink = s; });
    cout << "\nBinary Search (100.000 data, 100.000 pencarian):\n";
    cout << "  Iteratif : " << ts1 << " ms\n";
    cout << "  Rekursif : " << ts2 << " ms\n";

    // --- Kesimpulan analisa ---
    cout << "\nKesimpulan:\n";
    cout << " - Waktu: iteratif umumnya sedikit lebih cepat (tanpa overhead pemanggilan fungsi).\n";
    cout << " - Memori: rekursif linear memakai N frame stack -> O(N); iteratif O(1).\n";
    cout << "   Binary search rekursif hanya O(log N) stack, jadi aman.\n";
    cout << " - Risiko: rekursif linear dengan N sangat besar (ratusan ribu) bisa stack overflow,\n";
    cout << "   makanya pengujian rekursif linear dibatasi sampai N = 10.000.\n";
    cout << " - Keterbacaan: rekursif lebih ringkas untuk masalah yang bersifat membagi/berulang\n";
    cout << "   (binary search, combo), iteratif lebih mudah ditelusuri untuk loop sederhana.\n";
}

// ===========================================================================
// MENU 7 : UJI 3 ALGORITMA SORTING (>= 100.000 DATA)        (POIN TUGAS 4)
// ===========================================================================
// Tiga algoritma yang diuji:
//   1. Bubble Sort : O(n^2)       - sederhana tapi lambat
//   2. Merge Sort  : O(n log n)   - bagi dua, urutkan, gabungkan (butuh memori tambahan)
//   3. Quick Sort  : O(n log n)   - rata-rata, pakai pivot, tanpa memori tambahan
//
// Cara pengujian yang adil:
//   - Program membuat N skor acak SEKALI.
//   - Ketiga algoritma memakai SALINAN data yang sama.
//   - Waktu diukur, lalu hasil diperiksa benar-benar terurut (is_sorted).
// ===========================================================================

// ---- 1) BUBBLE SORT ----
// Bandingkan dua elemen bersebelahan, tukar jika urutannya salah.
// Ulangi sampai tidak ada lagi yang perlu ditukar.
// Jumlah perbandingan ~ n^2 / 2  -> untuk 100.000 data sekitar 5 miliar langkah.
void bubbleSort(vector<int>& a) {
    int n = (int)a.size();
    for (int i = 0; i < n - 1; i++) {
        bool tukar = false;
        for (int j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) { swap(a[j], a[j + 1]); tukar = true; }
        }
        if (!tukar) break;  // sudah terurut, berhenti lebih awal
    }
}

// ---- 2) MERGE SORT ----
// Bagi array jadi dua, urutkan masing-masing (rekursif), lalu GABUNGKAN.
// Kedalaman rekursi hanya log2(n) ~ 17 level, dan butuh array bantu "tmp".

// gabung(): menggabungkan dua bagian yang sudah terurut [l..m] dan [m+1..r]
void gabung(vector<int>& a, vector<int>& tmp, int l, int m, int r) {
    int i = l, j = m + 1, k = l;
    while (i <= m && j <= r) tmp[k++] = (a[i] <= a[j]) ? a[i++] : a[j++];  // ambil yang lebih kecil
    while (i <= m) tmp[k++] = a[i++];  // sisa bagian kiri
    while (j <= r) tmp[k++] = a[j++];  // sisa bagian kanan
    for (k = l; k <= r; k++) a[k] = tmp[k];  // salin balik ke array asli
}
void mergeSort(vector<int>& a, vector<int>& tmp, int l, int r) {
    if (l >= r) return;                 // BASE CASE: satu elemen sudah terurut
    int m = l + (r - l) / 2;
    mergeSort(a, tmp, l, m);            // urutkan separuh kiri
    mergeSort(a, tmp, m + 1, r);        // urutkan separuh kanan
    gabung(a, tmp, l, m, r);            // gabungkan keduanya
}

// ---- 3) QUICK SORT ----
// Pilih PIVOT (elemen tengah). Pindahkan yang lebih kecil ke kiri pivot dan
// yang lebih besar ke kanan, lalu ulangi pada kedua sisi.
// Rata-rata O(n log n). Terburuk O(n^2) jika pivot selalu jelek.
// Rekursi dilakukan pada bagian TERKECIL agar kedalaman stack tetap kecil.
void quickSort(vector<int>& a, int lo, int hi) {
    while (lo < hi) {
        int p = a[lo + (hi - lo) / 2];  // pivot = elemen tengah
        int i = lo, j = hi;
        while (i <= j) {
            while (a[i] < p) i++;       // cari elemen kiri yang >= pivot
            while (a[j] > p) j--;       // cari elemen kanan yang <= pivot
            if (i <= j) { swap(a[i], a[j]); i++; j--; }
        }
        if (j - lo < hi - i) { quickSort(a, lo, j); lo = i; }
        else { quickSort(a, i, hi); hi = j; }
    }
}

// ---- Prosedur MENU 7: menjalankan dan membandingkan ketiganya ----
void ujiSorting() {
    int n = bacaInt("Jumlah data (minimal 100000): ");
    if (n < 100000) { tampilPesanError("Minimal 100.000 data sesuai tugas."); return; }

    // Buat N skor acak (seed 2024 -> data sama setiap kali dijalankan)
    cout << "Membuat " << n << " skor acak...\n";
    mt19937 rng(2024);
    vector<int> asli(n);
    for (auto& x : asli) x = rng() % 1000001;

    struct Hasil { string nama; string kompleksitas; double ms; bool ok; };
    vector<Hasil> hasil;

    {   // --- Uji Bubble Sort (paling lama, bisa 10-30 detik) ---
        cout << "Menjalankan Bubble Sort (bisa memakan waktu puluhan detik)...\n";
        vector<int> a = asli;  // salinan data
        double t = ukurMs([&] { bubbleSort(a); });
        hasil.push_back({"Bubble Sort", "O(n^2)", t, is_sorted(a.begin(), a.end())});
    }
    {   // --- Uji Merge Sort ---
        cout << "Menjalankan Merge Sort...\n";
        vector<int> a = asli, tmp(n);
        double t = ukurMs([&] { mergeSort(a, tmp, 0, n - 1); });
        hasil.push_back({"Merge Sort", "O(n log n)", t, is_sorted(a.begin(), a.end())});
    }
    {   // --- Uji Quick Sort ---
        cout << "Menjalankan Quick Sort...\n";
        vector<int> a = asli;
        double t = ukurMs([&] { quickSort(a, 0, n - 1); });
        hasil.push_back({"Quick Sort", "O(n log n)", t, is_sorted(a.begin(), a.end())});
    }

    // --- Tampilkan tabel hasil ---
    cout << "\n===== HASIL PENGUJIAN SORTING (" << n << " data) =====\n";
    cout << left << setw(16) << "Algoritma" << setw(14) << "Kompleksitas"
         << setw(16) << "Waktu (ms)" << setw(14) << "Waktu (detik)" << "Terurut?\n";
    cout << string(70, '-') << "\n";
    for (auto& h : hasil) {
        cout << left << setw(16) << h.nama << setw(14) << h.kompleksitas
             << fixed << setprecision(2) << setw(16) << h.ms
             << setw(14) << h.ms / 1000.0 << (h.ok ? "Ya" : "TIDAK") << "\n";
    }

    // --- Analisa singkat ---
    double tercepat = hasil[0].ms;
    for (auto& h : hasil) tercepat = min(tercepat, h.ms);
    cout << "\nAnalisa:\n";
    cout << " - Bubble Sort paling lambat: " << hasil[0].ms / tercepat
         << "x lebih lama dari yang tercepat, karena O(n^2) (~n^2/2 perbandingan).\n";
    cout << " - Merge Sort stabil dan konsisten O(n log n), tetapi butuh array bantu O(n).\n";
    cout << " - Quick Sort biasanya tercepat dalam praktik (in-place, cache friendly),\n";
    cout << "   tetapi worst case O(n^2) bila pivot buruk.\n";
}

// ===========================================================================
// MENU UTAMA
// ===========================================================================
// Prosedur yang menampilkan daftar menu.
void tampilMenu() {
    cout << "\n=========== LEADERBOARD SKOR GAME ===========\n";
    cout << " 1. Tambah pemain\n";
    cout << " 2. Tampilkan leaderboard\n";
    cout << " 3. Cari pemain berdasarkan skor (binary search)\n";
    cout << " 4. Statistik (total, tertinggi, rata-rata)\n";
    cout << " 5. Isi data contoh\n";
    cout << " 6. Analisa iteratif vs rekursif\n";
    cout << " 7. Uji 3 algoritma sorting (100.000 data)\n";
    cout << " 0. Keluar\n";
    cout << "=============================================\n";
}

// Program mulai dari sini. Menu terus diulang (do-while) sampai user pilih 0.
// Setiap "case" memanggil prosedur milik menu tersebut.
int main() {
    int pilih;
    do {
        tampilMenu();
        pilih = bacaInt("Pilih menu: ");
        switch (pilih) {
            case 1: tambahPemain(); break;       // MENU 1
            case 2: tampilkanPeringkat(); break; // MENU 2
            case 3: cariPemain(); break;         // MENU 3
            case 4: tampilkanStatistik(); break; // MENU 4
            case 5: isiDataContoh(); break;      // MENU 5
            case 6: analisaIterRek(); break;     // MENU 6
            case 7: ujiSorting(); break;         // MENU 7
            case 0: cout << "Terima kasih!\n"; break;
            default: tampilPesanError("Menu tidak valid.");
        }
    } while (pilih != 0);
    return 0;
}
