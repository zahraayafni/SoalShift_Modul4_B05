#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/michael/Documents/inicobafuse";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  int fd = 0 ,x;
    if(strstr(fpath,".html")>0||strstr(fpath,".pdf")>0||strstr(fpath,".doc")>0){
        printf("Terjadi kesalahan! File berisi konten berbahaya.\n");
        char newName[1000];
        strcpy(newName,dirpath);
        strcat(newName, path);
        strcat(newName, ".ditandai");
        printf("%s %s\n", fpath, newName);
        rename(fpath,newName);
        char rahasia[1000];
        DIR *fr;
        strcpy(rahasia,dirpath);
        printf("%s\n",rahasia);
        strcat(rahasia,"/rahasia");
        fr = opendir(rahasia);
        if(fr==NULL){
            mkdir(rahasia,0755);
        }
        strcat(rahasia, path);
        strcat(rahasia, ".ditandai");
        printf("%s\n",rahasia);
        char pindah[1000]="mv ";
        strcat(pindah,newName);
        strcat(pindah," ");
        strcat(pindah,rahasia);

        system(pindah);
        return -1;
    }
    else if(strstr(fpath,".copy")>0){
        printf("File yang anda buka adalah file hasil salinan. File tidak bisa diubah maupun disalin kembali!\n");
        return -1;
    }
    else{
        (void) fi;
        fd = open(fpath, O_RDONLY);
        if (fd == -1)
            return -errno;

        res = pread(fd, buf, size, offset);
        if (res == -1)
            res = -errno;

        close(fd);
    }
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}