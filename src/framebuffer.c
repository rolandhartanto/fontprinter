/*
To test that the Linux framebuffer is set up correctly, and that the device permissions
are correct, use the program below which opens the frame buffer and draws a gradient-
filled red square:

retrieved from:
Testing the Linux Framebuffer for Qtopia Core (qt4-x11-4.2.2)

http://cep.xor.aps.anl.gov/software/qt4-x11-4.2.2/qtopiacore-testingframebuffer.html
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>

const int hitam = 0;
const int putih = 255;

int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;
int x = 0, y = 0;
long int location = 0;

void clearScreen(int jumlahBaris, int jumlahKolom, int warna){
    int x,y;
    for (y = 0; y < jumlahBaris; y++){
        for (x = 0; x < jumlahKolom; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;

                if (vinfo.bits_per_pixel == 32) {
                    //printf("a[%d][%d]: %d\n",absis,ordinat,a[absis][ordinat]);
                
                    *(fbp + location) = warna;        // Some blue
                    *(fbp + location + 1) = warna;     // A little green
                    *(fbp + location + 2) = warna;    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency    
                    
                }
        }
    }
}

void beriWarna(int warna){

}
    
int main()
{
    

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    printf("screensize: %ld\n",screensize);
    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    //membaca dari File

    FILE *fp;
    int i,j;
    char a[835][35];
    fp = fopen("../data/alphabet.txt","r");
    if(fp==NULL){
        printf("tidak terbaca\n");
        return 0;
    }

    printf("file terbaca\n");
    for(i=0;i<832;i++){
        fscanf(fp, "%s", a[i]);
    }
    
    // Figure out where in memory to put the pixel
    //clear screen hitam
    clearScreen(632, 1920, hitam);
    
    printf("masukkan input kata\n");

    char input[1000];
    scanf("%s",input);
    int pjg = strlen(input);

    //clear screen putih
    clearScreen(632, 1920, putih);
    
    int idxBaris = 0;
    int idxKolom = 0;
    for(i=0;i<pjg;i++){
        char kar = input[i];
        int absis = (kar-'A')*32, ordinat = 0; //menentukan indeks baris dan kolom dari array of char
        for (y = idxBaris; y < idxBaris+32; y++){
            for (x = idxKolom; x < idxKolom+32; x++) {

                location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;

                if (vinfo.bits_per_pixel == 32) {
                    //printf("a[%d][%d]: %d\n",absis,ordinat,a[absis][ordinat]);
                    if(a[absis][ordinat]=='0'){
                        *(fbp + location) = putih;        // Some blue
                        *(fbp + location + 1) = putih;     // A little green
                        *(fbp + location + 2) = putih;    // A lot of red
                        *(fbp + location + 3) = 0;      // No transparency    
                    }else{
                        *(fbp + location) = hitam;        // Some blue
                        *(fbp + location + 1) = hitam;     // A little green
                        *(fbp + location + 2) = hitam;    // A lot of red
                        *(fbp + location + 3) = 0;      // No transparency    
                    }
                    ordinat++;
            //location += 4;
                } else  { //assume 16bpp
                    int b = 10;
                    int g = (x-100)/6;     // A little green
                    int r = 31-(y-100)/16;    // A lot of red
                    unsigned short int t = r<<11 | g << 5 | b;
                    *((unsigned short int*)(fbp + location)) = t;
                }

            }
            absis++;
            ordinat = 0;
        }
        idxKolom+=32;
        if(idxKolom>=1920){idxKolom = 0; idxBaris+=34;} //jika melebihi pixel column..lanjutkan di baris selanjutnya dan idxKolom direset jadi 0
    }
    
    

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
