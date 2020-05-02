#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/denta/Documents";

int key = 10;
char cipher[] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

void encrypt(char *x) {
	// /home/denta/Documents/encv1_rhs/FOTO_PENTING/kelincilucu.jpg  
	int xlength = strlen(x), xbegin = 0;
	int i;
	for(i = strlen(x); i >= 0; i--) { 

		if(x[i] == '/') {	//ngambil kanan sampe ketemu slash
			break;
		}

		if(x[i] == '.') {	//kalo file ada extensi
			xlength = i - 1;
		}
	}

	// /encv1_rhs/FOTO_PENTING/kelincilucu
	for (int i = 1; i < xlength; i++) { 	//ngambil yang paling kanan
		if(x[i] == '/') {
			xbegin = i;
		}
	}
	
	int ind; //index
	char *ptr;
	// /kelincilucu
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

void decrypt(char *y) {

	int ylength = strlen(y), ybegin = 0;
	int i;
	
	//kalo ketemu slash atau end of array di fullpath dibreak
	for (int i = 1; i < ylength; i++) {
		if(y[i] == '/' || y[i+1] == '\0') {
			ybegin = i + 1;
			break;
		}
	}

	for(int i = strlen(y); i >= 0; i--) {

		if(y[i] == '/') {	
			break;	
		}

		if(y[i] == '.') { //kelincilucu.jpg
			ylength = i - 1;
			break;
		}
	}

	int ind;
	char *ptr;
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
	}
}

char *LOG = "/home/denta/fs.log";

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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000], temp[1000];
	strcpy(temp, path);

	// membandingkan 7 karakter pertama
	if(strncmp(path, "/encv1_", 7) == 0) {
		decrypt(temp);
	}

	sprintf(fpath, "%s%s", dirpath, temp);
	
    res = lstat(fpath, stbuf);

    if(res == -1) return -errno;

	log_info("LS", fpath);

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000], temppath[1000];
	
	// printf("path= %s\n", path);
    if(strcmp(path, "/") == 0) { //baca direktori
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else{
		strcpy(temppath, path);

		if(strncmp(path, "/encv1_", 7) == 0) { //dibandingkan 
			decrypt(temppath);
		}

		sprintf(fpath, "%s%s", dirpath, temppath);
	}
	
    int res = 0;

    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fpath);

    if(dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
		
		//titik sama dua titik direktori
		if(strcmp(de->d_name, ".") == 0 || 
			strcmp(de->d_name, "..") == 0) {
				continue;
		}

		char temp[1000];
		
		strcpy(temp, de->d_name);	// ngambil nama file doang
		// printf("ini temp = %s\n", temp);

		if(strncmp(path, "/encv1_", 7) == 0) {
			encrypt(temp);
		}

		res = (filler(buf, temp, &st, 0));

        if (res != 0)
            break;
    }

    closedir(dp);
	log_info("CD", fpath);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}
	
	// printf("read fpath : %s\n", fpath);
    int res = 0;
    int fd = 0;

    (void)fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) 
        res = -errno;

    close(fd);

    return res;
}
//file 5kb truncate jd 1kb
static int xmp_truncate(const char *path, off_t size)
{
	char fpath[1000];
	if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

    int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);

    // logs(0, "WRITE", fpath, "");
	log_info("WRITE", fpath);

	return res;
}
//hapus file
static int xmp_unlink(const char *path)
{
	char fpath[1000];
	if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;

	res = unlink(fpath);
	if (res == -1)
		return -errno;

    // logs(1, "UNLINK", fpath, "");
	log_warning("UNLINK", fpath);

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	char src[1000], dst[1000];
	printf("ini from : %s\n", from);
	printf("ini to : %s\n", to);
	if (strcmp(from, "/") == 0)
    {
        from = dirpath;
        sprintf(src, "%s", from);
    }
    else {
		char tempa[1000];		//rename folder encv1_ ke biasa
		strcpy(tempa, from);

		if(strncmp(from, "/encv1_", 7) == 0) {	//ada atau gak ada folder encv1_
			decrypt(tempa);
		}

		sprintf(src, "%s%s", dirpath, tempa);
	}

	if (strcmp(to, "/") == 0)
    {
        to = dirpath;
        sprintf(dst, "%s", to);
    }
    else {
		char tempb[1000];		//rename dari biasa ke encv
		strcpy(tempb, to);

		if(strncmp(to, "/encv1_", 7) == 0) {
			decrypt(tempb);		//dekrip yg didocomuents
		}

		sprintf(dst, "%s%s", dirpath, tempb);
	}
	
	int res;

	res = rename(src, dst);
	if (res == -1)
		return -errno;

	// logs(0, "RENAME", src, dst);
	log_info("RENAME", src);

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000];
	if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;

	res = mkdir(fpath, 0750);
	if (res == -1)
		return -errno;

	// logs(0, "MKDIR", fpath, "");
	log_info("MKDIR", fpath);

	return 0;
}

static int xmp_rmdir(const char *path)
{
	char fpath[1000];
	if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);
		// /home/denta/Documents/encv1_rhs/FOTO_PENTING/kelincilucu.jpg
		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	// logs(1, "RMDIR", fpath, "");
	log_warning("RMDIR", fpath);

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	char fpath[1000];
	if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	
    // logs(0, "OPEN", fpath, "");
	log_info("OPEN", fpath);

	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	char fpath[1000];
	if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else {
		char temp[1000];
		strcpy(temp, path);

		if(strncmp(path, "/encv1_", 7) == 0) {
			decrypt(temp);
		}

		sprintf(fpath, "%s%s", dirpath, temp);
	}

	int res;

	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

	// logs(0, "CREATE", fpath, "");
	log_info("CREATE", fpath);

	return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr 	= xmp_getattr,
    .readdir 	= xmp_readdir,
    .read 		= xmp_read,
	.truncate   = xmp_truncate,
	.write      = xmp_write,
	.unlink     = xmp_unlink,
	.rename 	= xmp_rename,
	.mkdir		= xmp_mkdir,
	.rmdir		= xmp_rmdir,
	.open       = xmp_open,
	.mknod 		= xmp_mknod,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
