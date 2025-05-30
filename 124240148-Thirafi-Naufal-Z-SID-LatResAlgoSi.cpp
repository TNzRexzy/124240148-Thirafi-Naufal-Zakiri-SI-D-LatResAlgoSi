#include <iostream>
#include <cstring>
using namespace std;

// Ini struktur buat nyimpen info bukunya
struct Buku
{
    string judul, pengarang;
    int tahun;
    Buku *kiri;
    Buku *kanan;
};

// Ini buat log undo-nya, jadi tiap kita nambah/hapus buku bakal dicatet di sini
struct Log
{
    string aksi; // bisa "tambah" atau "hapus"
    Buku data;
    Log *next;
};

// Root itu tempat awal BST-nya, top itu buat stack log-nya
Buku *root = nullptr;
Log *top = nullptr;

// Fungsi ini buat bikin node buku baru
Buku *buatNode(string judul, string pengarang, int tahun)
{
    // Jadi dia bakal bikin node kosong, terus isinya dikasih data dari input
    Buku *baru = new Buku;
    baru->judul = judul;
    baru->pengarang = pengarang;
    baru->tahun = tahun;
    baru->kiri = baru->kanan = nullptr;
    return baru;
}

// Ini buat nyimpen aksi terakhir ke "stack log"
// Jadi pas undo kita tinggal ngambil data paling atas dari sini
void pushLog(string aksi, Buku data)
{
    Log *logBaru = new Log{aksi, data, top}; // taruh paling atas
    top = logBaru;
}

// Ini buat ngambil aksi terakhir dari stack (undo)
// Kalau stack kosong, return false
bool popLog(Log &log)
{
    if (top == nullptr)
        return false;
    log = *top;
    Log *hapus = top;
    top = top->next;
    delete hapus;
    return true;
}

// Fungsi ini buat nambahin buku ke BST
// Judul dijadiin patokan biar urut (sesuai perintah)
Buku *tambahBuku(Buku *node, Buku data, bool log = true)
{
    if (!node)
    {
        if (log)
            pushLog("tambah", data); // Kalau log=true, masukin ke stack log
        return buatNode(data.judul, data.pengarang, data.tahun);
    }

    // Biar BST-nya urut, kita bandingin judulnya
    if (data.judul < node->judul)
    {
        node->kiri = tambahBuku(node->kiri, data, log);
    }
    else if (data.judul > node->judul)
    {
        node->kanan = tambahBuku(node->kanan, data, log);
    }
    else
    {
        // Kalau udah ada judul yang sama, kasih tau aja
        cout << "Judul sudah ada!\n";
    }

    return node;
}

// Ini fungsi buat nampilin semua buku dari A-Z
// Dipanggil pas milih "tampilkan buku"
void tampilkanBuku(Buku *node)
{
    if (!node)
        return;

    // In-order traversal: kiri → tengah → kanan (biar urut)
    tampilkanBuku(node->kiri);
    cout << "Judul     : " << node->judul << endl;
    cout << "Pengarang : " << node->pengarang << endl;
    cout << "Tahun     : " << node->tahun << endl;
    cout << "-------------------------\n";
    tampilkanBuku(node->kanan);
}

// Ini fungsi buat hapus buku berdasarkan judul
// Kalau berhasil dihapus, datanya juga masukin ke stack log
Buku *hapusBuku(Buku *node, string judul, bool log = true)
{
    if (!node)
        return nullptr;

    // Nyari dulu node yang mau dihapus
    if (judul < node->judul)
    {
        node->kiri = hapusBuku(node->kiri, judul, log);
    }
    else if (judul > node->judul)
    {
        node->kanan = hapusBuku(node->kanan, judul, log);
    }
    else
    {
        if (log)
            pushLog("hapus", *node); // simpen dulu ke log

        // Kasus node punya 1 anak / ga ada anak
        if (!node->kiri)
        {
            Buku *temp = node->kanan;
            delete node;
            return temp;
        }
        else if (!node->kanan)
        {
            Buku *temp = node->kiri;
            delete node;
            return temp;
        }

        // Kalau node punya 2 anak, cari pengganti (successor)
        Buku *succ = node->kanan;
        while (succ->kiri)
            succ = succ->kiri;

        // Ganti data node sekarang sama data successor
        node->judul = succ->judul;
        node->pengarang = succ->pengarang;
        node->tahun = succ->tahun;

        // Hapus successor-nya (karena udah dipindah ke atas)
        node->kanan = hapusBuku(node->kanan, succ->judul, false);
    }

    return node;
}

// Nah ini dia fungsi buat undo
// Kalau tadi tambah → sekarang dihapus
// Kalau tadi hapus → sekarang dikembalikan
void undo()
{
    Log log;
    if (!popLog(log))
    {
        cout << "Tidak ada aksi untuk di-undo.\n";
        return;
    }

    if (log.aksi == "tambah")
    {
        root = hapusBuku(root, log.data.judul, false);
        cout << "Undo tambah berhasil.\n";
    }
    else if (log.aksi == "hapus")
    {
        root = tambahBuku(root, log.data, false);
        cout << "Undo hapus berhasil.\n";
    }
}

// Ini menu utama, tempat semua interaksi user
void menu()
{
    int pilihan;
    string judul, pengarang;
    int tahun;

    do
    {
        cout << "\n=== Aplikasi Buku Komunitas ===\n";
        cout << "1. Tambah Buku\n";
        cout << "2. Tampilkan Buku\n";
        cout << "3. Hapus Buku\n";
        cout << "4. Undo Aksi Terakhir\n";
        cout << "5. Keluar\n";
        cout << "Pilih menu: ";
        cin >> pilihan;
        cin.ignore(); // buat ngilangin newline

        switch (pilihan)
        {
        case 1:
            // Input buku
            cout << "Judul: ";
            getline(cin, judul);
            cout << "Pengarang: ";
            getline(cin, pengarang);
            cout << "Tahun Terbit: ";
            cin >> tahun;
            cin.ignore();
            root = tambahBuku(root, {judul, pengarang, tahun});
            break;
        case 2:
            // Tampilkan semua buku
            cout << "\n--- Daftar Buku ---\n";
            tampilkanBuku(root);
            break;
        case 3:
            // Hapus buku berdasarkan judul
            cout << "Masukkan judul yang ingin dihapus: ";
            getline(cin, judul);
            root = hapusBuku(root, judul);
            break;
        case 4:
            // Undo
            undo();
            break;
        case 5:
            cout << "Terima kasih!\n";
            break;
        default:
            cout << "Pilihan tidak valid.\n";
        }

    } while (pilihan != 5);
}

// Ini fungsi utama (main)
int main()
{
    menu(); // langsung masuk ke menu utama
    return 0;
}