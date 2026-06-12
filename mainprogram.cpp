#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <sstream>

using namespace std;

const string DATA_FILE = "users.dat";

string escapeString(const string &value);
string unescapeString(const string &value);

struct transaction{
    string tanggal;
    double pemasukan;
    string alokasi;
    double pengeluaran;
    string keterangan;
    double saldoprimer;
    double saldohobi;
    double saldotabungan;};

struct user{
    string nama;
    double primer;
    double hobi;
    double tabungan;
    int pctprimer;
    int pcthobi;
    int pcttabungan;
    vector<transaction> riwayat;};

const int MAX_USER = 5;
vector<user> users;

void tambahuser();
void hapususer();
void userdashboard(int indexuser);
void tambahpemasukkan(int indexuser);
void tambahpengeluaran(int indexuser);
void tampilkanriwayat(int indexuser);
void settingdistribusi(int indexuser);
void loadusers();
void saveusers();
string gettanggal();
double gettotalsaldo(int indexuser);
void clearScreen();

void mainmenu(){

    int pilihan;

    do{
        clearScreen();
        cout << "\n================================\n";
        cout << "     Personal Finance Manager     \n";
        cout << "================================\n\n";

        //tampilkan daftar user
        for (int i = 0; i < users.size(); i++){
            cout << i + 1 << ". " << users[i].nama << endl;
        }
        cout << endl;

        int menutambah = -1;
        int menuhapus;

        //jika belum penuh
        if (users.size() < MAX_USER){
            menutambah = users.size() + 1;
            cout << menutambah << ". Tambah User\n";
            menuhapus = users.size() + 2;
            cout << menuhapus << ". Hapus User\n";
        }else{
            cout << "[Slot User Sudah Penuh: 5/5]\n";

            menuhapus = users.size() + 1;
            cout << menuhapus << ". Hapus User\n";
        }

        cout << "\n0. Keluar\n";
        cout << "\nPilihan: ";
        cin >> pilihan;

        //keluar
        if (pilihan == 0){
            cout << "\nTerima Kasih Telah Menggunakan Aplikasi Ini.\n";
        }else if(pilihan >= 1 && pilihan <= users.size()){
            userdashboard(pilihan - 1);
        }else if (menutambah != -1 && pilihan == menutambah){
            tambahuser();
        }else if (pilihan == menuhapus){
            hapususer();
        }else {
            cout << "\nPilihan Tidak Valid!\n";
        };
    }while (pilihan != 0);
};

void tambahuser() {
    clearScreen();
    if (users.size() >= MAX_USER) {
        cout << "\nTidak bisa menambah user: slot sudah penuh.\n";
        return;
    }

    cout << "\nMasukkan nama user: ";
    string nama;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, nama);

    if (nama.empty()) {
        cout << "Nama user tidak boleh kosong.\n";
        return;
    }

    user baru;
    baru.nama = nama;
    baru.primer = 0.0;
    baru.hobi = 0.0;
    baru.tabungan = 0.0;
    // Default percentage distribution 70/20/10
    baru.pctprimer = 70;
    baru.pcthobi = 20;
    baru.pcttabungan = 10;
    users.push_back(move(baru));

    cout << "User '" << nama << "' berhasil ditambahkan.\n";
    saveusers();
}

void hapususer() {
    clearScreen();
    if (users.empty()) {
        cout << "\nTidak ada user untuk dihapus.\n";
        return;
    }

    cout << "\nPilih nomor user yang akan dihapus:\n";
    for (int i = 0; i < users.size(); i++) {
        cout << "  " << i + 1 << ". " << users[i].nama << "\n";
    }

    int pilihan;
    cout << "Pilihan: ";
    cin >> pilihan;

    if (pilihan < 1 || pilihan > users.size()) {
        cout << "Pilihan tidak valid.\n";
        return;
    }

    string nama = users[pilihan - 1].nama;
    users.erase(users.begin() + (pilihan - 1));
    cout << "User '" << nama << "' berhasil dihapus.\n";
    saveusers();
}

void userdashboard(int indexuser) {
    if (indexuser < 0 || indexuser >= users.size()) {
        cout << "User tidak ditemukan.\n";
        return;
    }

    while (true) {
        clearScreen();
        user &u = users[indexuser];
        cout << "\n==============================\n";
        cout << "       Dashboard User       \n";
        cout << "==============================\n";
        cout << "Nama User       : " << u.nama << "\n";
        cout << fixed << setprecision(2);
        cout << "Saldo Primer    : Rp " << u.primer << "\n";
        cout << "Saldo Hobi      : Rp " << u.hobi << "\n";
        cout << "Saldo Tabungan  : Rp " << u.tabungan << "\n";
        cout << "------------------------------\n";
        cout << "Total Saldo     : Rp " << gettotalsaldo(indexuser) << "\n";
        cout << "------------------------------\n";
        cout << "\nPilihan:\n";
        cout << "  1. Tambah Pemasukkan\n";
        cout << "  2. Tambah Pengeluaran\n";
        cout << "  3. Tampilkan Riwayat Transaksi\n";
        cout << "  4. Pengaturan Distribusi\n";
        cout << "  0. Kembali ke Menu Utama\n";
        cout << "Pilihan: ";

        int pilihan;
        cin >> pilihan;
        if (!cin) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            continue;
        }

        switch (pilihan) {
            case 1:
                tambahpemasukkan(indexuser);
                break;
            case 2:
                tambahpengeluaran(indexuser);
                break;
            case 3:
                tampilkanriwayat(indexuser);
                break;
            case 4:
                settingdistribusi(indexuser);
                break;
            case 0:
                return;
            default:
                cout << "Pilihan tidak valid.\n";
                break;
        }
    }
}

void tambahpemasukkan(int indexuser) {
    clearScreen();
    if (indexuser < 0 || indexuser >= users.size()) {
        cout << "User tidak ditemukan.\n";
        return;
    }

    user &u = users[indexuser];
    double jumlah;
    cout << "\nMasukkan jumlah pemasukan: Rp ";
    cin >> jumlah;
    if (!cin || jumlah <= 0.0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Jumlah tidak valid.\n";
        return;
    }

    cout << "\nPilih alokasi pemasukan:\n";
    cout << "  1. Otomatis\n";
    cout << "  2. Primer\n";
    cout << "  3. Hobi\n";
    cout << "  4. Tabungan\n";
    cout << "Pilihan: ";
    int alokasiPilihan;
    cin >> alokasiPilihan;
    if (!cin || alokasiPilihan < 1 || alokasiPilihan > 4) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Pilihan alokasi tidak valid.\n";
        return;
    }

    double tambahanPrimer = 0.0;
    double tambahanHobi = 0.0;
    double tambahanTabungan = 0.0;
    string alokasiDesc;

    if (alokasiPilihan == 1) {
        tambahanPrimer = jumlah * u.pctprimer / 100.0;
        tambahanHobi = jumlah * u.pcthobi / 100.0;
        tambahanTabungan = jumlah - tambahanPrimer - tambahanHobi;
        alokasiDesc = "Otomatis";
    } else if (alokasiPilihan == 2) {
        tambahanPrimer = jumlah;
        alokasiDesc = "Primer";
    } else if (alokasiPilihan == 3) {
        tambahanHobi = jumlah;
        alokasiDesc = "Hobi";
    } else {
        tambahanTabungan = jumlah;
        alokasiDesc = "Tabungan";
    }

    u.primer += tambahanPrimer;
    u.hobi += tambahanHobi;
    u.tabungan += tambahanTabungan;

    transaction tx;
    tx.tanggal = gettanggal();
    tx.pemasukan = jumlah;
    tx.alokasi = alokasiDesc;
    tx.pengeluaran = 0.0;
    tx.keterangan = "Pemasukan";
    tx.saldoprimer = u.primer;
    tx.saldohobi = u.hobi;
    tx.saldotabungan = u.tabungan;
    u.riwayat.push_back(move(tx));

    cout << "Pemasukan berhasil ditambahkan.\n";
    saveusers();
}

void tambahpengeluaran(int indexuser) {
    clearScreen();
    if (indexuser < 0 || indexuser >= users.size()) {
        cout << "User tidak ditemukan.\n";
        return;
    }

    user &u = users[indexuser];
    int pilihan;
    double jumlah;

    cout << "\nPilih kategori pengeluaran:\n";
    cout << "  1. Primer\n";
    cout << "  2. Hobi\n";
    cout << "Pilihan: ";
    cin >> pilihan;
    if (!cin || pilihan < 1 || pilihan > 2) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Pilihan kategori tidak valid.\n";
        return;
    }

    cout << "Masukkan jumlah pengeluaran: Rp ";
    cin >> jumlah;
    if (!cin || jumlah <= 0.0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Jumlah tidak valid.\n";
        return;
    }

    string kategori;
    double *saldoPtr = nullptr;
    if (pilihan == 1) {
        kategori = "Primer";
        saldoPtr = &u.primer;
    } else {
        kategori = "Hobi";
        saldoPtr = &u.hobi;
    }

    if (jumlah > *saldoPtr) {
        cout << "Saldo " << kategori << " tidak cukup.\n";
        return;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string deskripsi;
    cout << "Masukkan keterangan pengeluaran: ";
    getline(cin, deskripsi);
    if (deskripsi.empty()) {
        deskripsi = "Pengeluaran";
    }

    *saldoPtr -= jumlah;

    transaction tx;
    tx.tanggal = gettanggal();
    tx.pemasukan = 0.0;
    tx.alokasi = kategori;
    tx.pengeluaran = jumlah;
    tx.keterangan = deskripsi;
    tx.saldoprimer = u.primer;
    tx.saldohobi = u.hobi;
    tx.saldotabungan = u.tabungan;
    u.riwayat.push_back(move(tx));

    cout << "Pengeluaran berhasil dicatat.\n";
    saveusers();
}

void tampilkanriwayat(int indexuser) {
    clearScreen();
    if (indexuser < 0 || indexuser >= users.size()) {
        cout << "User tidak ditemukan.\n";
        return;
    }

    user &u = users[indexuser];
    cout << "\nRiwayat Transaksi untuk " << u.nama << ":\n";
    if (u.riwayat.empty()) {
        cout << "Belum ada transaksi.\n";
    } else {
        cout << left << setw(12) << "Tanggal"
             << setw(12) << "Pemasukan"
             << setw(14) << "Pengeluaran"
             << setw(12) << "Alokasi"
             << setw(24) << "Keterangan"
             << setw(14) << "Primer"
             << setw(14) << "Hobi"
             << setw(14) << "Tabungan"
             << setw(14) << "Total"
             << "\n";
        cout << string(136, '-') << "\n";
        for (const auto &tx : u.riwayat) {
            double totalSaldo = tx.saldoprimer + tx.saldohobi + tx.saldotabungan;
            cout << left << setw(12) << tx.tanggal
                 << "Rp " << setw(8) << fixed << setprecision(2) << tx.pemasukan
                 << "Rp " << setw(10) << tx.pengeluaran
                 << setw(12) << tx.alokasi
                 << setw(24) << tx.keterangan
                 << "Rp " << setw(10) << tx.saldoprimer
                 << "Rp " << setw(10) << tx.saldohobi
                 << "Rp " << setw(10) << tx.saldotabungan
                 << "Rp " << setw(10) << totalSaldo
                 << "\n";
        }
    }

    cout << "\nTekan Enter untuk kembali...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void settingdistribusi(int indexuser) {
    clearScreen();
    if (indexuser < 0 || indexuser >= users.size()) {
        cout << "User tidak ditemukan.\n";
        return;
    }

    user &u = users[indexuser];
    int primer, hobi, tabungan;
    
    cout << "\n=== Distribusi Saat Ini ===\n";
    cout << "Primer (%): " << u.pctprimer << endl;
    cout << "Hobi (%): " << u.pcthobi << endl;
    cout << "Tabungan (%): " << u.pcttabungan << endl;
    
    cout << "\nMasukkan persentase distribusi baru.\n";
    cout << "Primer (%): ";
    cin >> primer;
    cout << "Hobi (%): ";
    cin >> hobi;
    cout << "Tabungan (%): ";
    cin >> tabungan;

    if (!cin || primer < 0 || hobi < 0 || tabungan < 0 || primer + hobi + tabungan != 100) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Distribusi tidak valid. Total harus 100% dan masing-masing tidak negatif.\n";
        return;
    }

    u.pctprimer = primer;
    u.pcthobi = hobi;
    u.pcttabungan = tabungan;
    cout << "Distribusi berhasil diperbarui.\n";
        saveusers();
}

string escapeString(const string &value) {
    string result;
    for (char ch : value) {
        switch (ch) {
            case '\\': result += "\\\\"; break;
            case '|': result += "\\|"; break;
            case '\n': result += "\\n"; break;
            default: result += ch; break;
        }
    }
    return result;
}

string unescapeString(const string &value) {
    string result;
    result.reserve(value.size());
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '\\' && i + 1 < value.size()) {
            char next = value[i + 1];
            if (next == '\\' || next == '|' || next == 'n') {
                if (next == 'n') result += '\n';
                else result += next;
                ++i;
                continue;
            }
        }
        result += value[i];
    }
    return result;
}

void loadusers() {
    users.clear();
    ifstream file(DATA_FILE);
    if (!file.is_open()) {
        return;
    }

    string line;
    if (!getline(file, line)) {
        return;
    }

    int userCount = 0;
    try {
        userCount = stoi(line);
    } catch (...) {
        return;
    }

    for (int i = 0; i < userCount; ++i) {
        user u;
        if (!getline(file, line)) break;
        u.nama = unescapeString(line);

        if (!getline(file, line)) break;
        {
            istringstream iss(line);
            iss >> u.primer >> u.hobi >> u.tabungan >> u.pctprimer >> u.pcthobi >> u.pcttabungan;
            if (!iss) break;
        }

        int txCount = 0;
        if (!getline(file, line)) break;
        try {
            txCount = stoi(line);
        } catch (...) {
            break;
        }

        for (int t = 0; t < txCount; ++t) {
            if (!getline(file, line)) break;
            transaction tx;
            vector<string> fields;
            string field;
            bool escaped = false;
            for (size_t j = 0; j < line.size(); ++j) {
                char ch = line[j];
                if (escaped) {
                    field += ch;
                    escaped = false;
                } else if (ch == '\\') {
                    escaped = true;
                } else if (ch == '|') {
                    fields.push_back(field);
                    field.clear();
                } else {
                    field += ch;
                }
            }
            fields.push_back(field);

            if (fields.size() != 8) continue;
            tx.tanggal = unescapeString(fields[0]);
            tx.pemasukan = stod(fields[1]);
            tx.alokasi = unescapeString(fields[2]);
            tx.pengeluaran = stod(fields[3]);
            tx.keterangan = unescapeString(fields[4]);
            tx.saldoprimer = stod(fields[5]);
            tx.saldohobi = stod(fields[6]);
            tx.saldotabungan = stod(fields[7]);
            u.riwayat.push_back(move(tx));
        }

        users.push_back(move(u));
    }
}

void saveusers() {
    ofstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "Gagal menyimpan data ke " << DATA_FILE << "\n";
        return;
    }

    file << users.size() << '\n';
    for (const auto &u : users) {
        file << escapeString(u.nama) << '\n';
        file << u.primer << ' ' << u.hobi << ' ' << u.tabungan << ' '
             << u.pctprimer << ' ' << u.pcthobi << ' ' << u.pcttabungan << '\n';
        file << u.riwayat.size() << '\n';
        for (const auto &tx : u.riwayat) {
            file << escapeString(tx.tanggal) << '|' << tx.pemasukan << '|' << escapeString(tx.alokasi)
                 << '|' << tx.pengeluaran << '|' << escapeString(tx.keterangan)
                 << '|' << tx.saldoprimer << '|' << tx.saldohobi << '|' << tx.saldotabungan << '\n';
        }
    }
}

void clearScreen() {
    cout << "\033[2J\033[H";
}

string gettanggal() {
    time_t now = time(nullptr);
    tm *lt = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", lt);
    return string(buffer);
}

double gettotalsaldo(int indexuser) {
    if (indexuser < 0 || indexuser >= users.size()) {
        return 0.0;
    }
    return users[indexuser].primer + users[indexuser].hobi + users[indexuser].tabungan;
}

int main() {
    loadusers();
    mainmenu();
    saveusers();
    return 0;
}
