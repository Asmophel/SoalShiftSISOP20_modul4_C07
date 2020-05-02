# SoalShiftSISOP20_modul3_C07

##### Fandi Wahyu R - 05111840000108, Brananda Denta WP - 05111840000143

### Outline
+ [Soal 1](#soal-1)

+ [Soal 2](#soal-2)
+ [Soal 3](#soal-3)
+ [Soal 4](#soal-4)


## Soal
Di suatu perusahaan, terdapat pekerja baru yang super jenius, ia bernama jasir. Jasir baru bekerja selama seminggu di perusahan itu, dalam waktu seminggu tersebut ia selalu terhantui oleh ketidak amanan dan ketidak efisienan file system yang digunakan perusahaan tersebut. Sehingga ia merancang sebuah file system yang sangat aman dan efisien. Pada segi keamanan, Jasir telah menemukan 2 buah metode enkripsi file. Pada mode enkripsi pertama, nama file-file pada direktori terenkripsi akan dienkripsi menggunakan metode substitusi. Sedangkan pada metode enkripsi yang kedua, file-file pada direktori terenkripsi akan di-split menjadi file-file kecil. Sehingga orang-orang yang tidak menggunakan filesystem rancangannya akan kebingungan saat mengakses direktori terenkripsi tersebut. Untuk segi efisiensi, dikarenakan pada perusahaan tersebut sering dilaksanakan sinkronisasi antara 2 direktori, maka jasir telah merumuskan sebuah metode agar filesystem-nya mampu mengsingkronkan kedua direktori tersebut secara otomatis. Agar integritas filesystem tersebut lebih terjamin, maka setiap command yang dilakukan akan dicatat kedalam sebuah file log.
(catatan: filesystem berfungsi normal layaknya linux pada umumnya)
(catatan: mount source (root) filesystem adalah direktori /home/[user]/Documents)

### Soal 1
1. Berikut adalah detail filesystem rancangan jasir:
Enkripsi versi 1:

a. Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.

b. Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.

c. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.

d. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.

e. Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.
```
9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO
```
Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10

“encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg

Note : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.

f. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.

### Soal 2
2. Enkripsi versi 2:

a. Jika sebuah direktori dibuat dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.

b. Jika sebuah direktori di-rename dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.

c. Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekrip.

d. Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.

e. Pada enkripsi v2, file-file pada direktori asli akan menjadi bagian-bagian kecil sebesar 1024 bytes dan menjadi normal ketika diakses melalui filesystem rancangan jasir. Sebagai contoh, file File_Contoh.txt berukuran 5 kB pada direktori asli akan menjadi 5 file kecil yakni: 
File_Contoh.txt.000, File_Contoh.txt.001, File_Contoh.txt.002, File_Contoh.txt.003, dan File_Contoh.txt.004.

f. Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lain yang ada didalam direktori tersebut (rekursif).

### Penyelesaian
membuat fungsi encrypt dan dekrypt

untuk fungsi enkripsi, langkah awal yang perlu diperhatikan adalah mengabaikan extensi sebuah file, sehingga yang di encrypt hanyalah nama file nya saja
``` c
void encrypt(char *x) {

	int xlength = strlen(x), xbegin = 0;
	int i;
	for(i = strlen(x); i >= 0; i--) { 

		if(x[i] == '/') {	
			break;
		}

		if(x[i] == '.') {	
			xlength = i - 1;
		}
	}
``` 

lalu setelah itu, baru diambil bagian yang paling akhir dari suatu path dan bagian akhir tersebut merupakan file atau folder yang akan di encrypt
``` c
	for (int i = 1; i < xlength; i++) { 	//ngambil yang paling kanan
		if(x[i] == '/') {
			xbegin = i;
		}
	}
	
	int ind; //index
	char *ptr;

	for(i = xbegin ; i < xlength; i++) {
		if(x[i] == '/') {
			continue;
		}

		ptr = strchr(cipher, x[i]);

		if(ptr) {
			ind = ptr - cipher;  
			x[i] = cipher[(ind + key) % strlen(cipher)];
		}
	}
}
```

begitu juga dengan decrypt, hampir sama dengan encrypt namun yang beda adalah karena kalo di encrypt di tambah key, maka di decrypt dikurangi key
``` c
	for(i = ybegin ; i < ylength; i++) {	
		if(y[i] == '/') {	
			continue;
		}
		
		ptr = strchr(cipher, y[i]);
		
		if(ptr) {
			ind = ptr - cipher - key;
			if (ind < 0) {
				ind = ind + strlen(cipher); //handle kalo index minus
			}
			y[i] = cipher[ind];
		}
```

### Soal 3
3. Sinkronisasi direktori otomatis:

Tanpa mengurangi keumuman, misalkan suatu directory bernama dir akan tersinkronisasi dengan directory yang memiliki nama yang sama dengan awalan sync_ yaitu sync_dir. Persyaratan untuk sinkronisasi yaitu:

a. Kedua directory memiliki parent directory yang sama.

b. Kedua directory kosong atau memiliki isi yang sama. Dua directory dapat dikatakan memiliki isi yang sama jika memenuhi:

    i. Nama dari setiap berkas di dalamnya sama.
    ii. Modified time dari setiap berkas di dalamnya tidak berselisih
        lebih dari 0.1 detik.

c. Sinkronisasi dilakukan ke seluruh isi dari kedua directory tersebut, tidak hanya di satu child directory saja.

d. Sinkronisasi mencakup pembuatan berkas/directory, penghapusan berkas/directory, dan pengubahan berkas/directory.

Jika persyaratan di atas terlanggar, maka kedua directory tersebut tidak akan tersinkronisasi lagi.
Implementasi dilarang menggunakan symbolic links dan thread.

### Soal 4
4. Log System :


a. Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.

b. Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.

c. Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.

d. Sisanya, akan dicatat dengan level INFO.
Format untuk logging yaitu:

    [LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]

LEVEL    : Level logging

yy   	 : Tahun dua digit

mm    	 : Bulan dua digit

dd    	 : Hari dua digit

HH    	 : Jam dua digit

MM    	 : Menit dua digit

SS    	 : Detik dua digit

CMD      : System call yang terpanggil

DESC     : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)

Contoh format logging nantinya seperti:

    INFO::200419-18:29:28::MKDIR::/iz1
    INFO::200419-18:29:33::CREAT::/iz1/yena.jpg 
    INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg

### Penyelesaian
membuat fungsi log_warning dan log_info

di dalam masing masing fungsi log diisi dengan cara mengisi file fs.log dengan ketentuan dalam soal. caranya adalah memanggil time_t dan mendeklarasikan variabel untuk masing masing ketentuan yang ada pada soal.
``` c
void log_warning(char* desc, char* path) {
	FILE *file_log = fopen(LOG, "a");

	time_t rawtime = time(NULL);
	struct tm tm = *localtime(&rawtime);

	int tahun = tm.tm_year+1900;
	int bulan = tm.tm_mon+1;
	int hari = tm.tm_mday;
	int jam = tm.tm_hour;
	int menit = tm.tm_min;
	int detik = tm.tm_sec;

	fprintf(file_log, "WARNING::%02d%02d%02d-%02d:%02d:%02d::%s::%s\n", 
		tahun, bulan, hari, jam, menit, detik, desc, path);
	fclose(file_log);
}
```
``` c
void log_info(char* desc, char* path) {
	FILE *file_log = fopen(LOG, "a");

	time_t rawtime = time(NULL);
	struct tm tm = *localtime(&rawtime);

	int tahun = tm.tm_year+1900;
	int bulan = tm.tm_mon+1;
	int hari = tm.tm_mday;
	int jam = tm.tm_hour;
	int menit = tm.tm_min;
	int detik = tm.tm_sec;

	fprintf(file_log, "INFO::%02d%02d%02d-%02d:%02d:%02d::%s::%s\n", 
		tahun, bulan, hari, jam, menit, detik, desc, path);
	fclose(file_log);
}
```
lalu di masing masing fungsi fuse akan memanggil fungsi log tersebut sesuai kebutuhan nya.
