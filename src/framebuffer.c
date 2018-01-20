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

int main()
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;
    int x = 0, y = 0;
    long int location = 0;

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

    // printf("a:\n");
    // for(i=0;i<832;i++){
    //     for(j=0;j<32;j++){
    //         printf("%c",a[i][j]);
    //     }
    //     printf("\n");
    // }

    // printf("%d\n",'A');

    x = 0; y = 0;       // Where we are going to put the pixel
    int idx = 0;
    // Figure out where in memory to put the pixel
    //clear screen hitam
    for (y = 0; y < 632; y++){
        for (x = 0; x < 1920; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;

                if (vinfo.bits_per_pixel == 32) {
                    //printf("a[%d][%d]: %d\n",absis,ordinat,a[absis][ordinat]);
                
                    *(fbp + location) = 0;        // Some blue
                    *(fbp + location + 1) = 0;     // A little green
                    *(fbp + location + 2) = 0;    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency    
                    
                }
        }
    }
    printf("masukkan input kata\n");

    char input[100];
    scanf("%s",input);
    int pjg = strlen(input);

    //clear screen putih
    for (y = 0; y < 632; y++){
        for (x = 0; x < 1920; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;

                if (vinfo.bits_per_pixel == 32) {
                    //printf("a[%d][%d]: %d\n",absis,ordinat,a[absis][ordinat]);
                
                    *(fbp + location) = 255;        // Some blue
                    *(fbp + location + 1) = 255;     // A little green
                    *(fbp + location + 2) = 255;    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency    
                    
                }
        }
    }
    int idxBaris = 0;
    for(i=0;i<pjg;i++){
        char kar = input[i];
        int absis = (kar-'A')*32, ordinat = 0;
        for (y = idxBaris; y < idxBaris+32; y++){
            for (x = idx; x < idx+32; x++) {

                location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                           (y+vinfo.yoffset) * finfo.line_length;

                if (vinfo.bits_per_pixel == 32) {
                    //printf("a[%d][%d]: %d\n",absis,ordinat,a[absis][ordinat]);
                    if(a[absis][ordinat]=='0'){
                        *(fbp + location) = 255;        // Some blue
                        *(fbp + location + 1) = 255;     // A little green
                        *(fbp + location + 2) = 255;    // A lot of red
                        *(fbp + location + 3) = 0;      // No transparency    
                    }else{
                        *(fbp + location) = 0;        // Some blue
                        *(fbp + location + 1) = 0;     // A little green
                        *(fbp + location + 2) = 0;    // A lot of red
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
        idx+=32;
        if(idx>=1920){idx = 0; idxBaris+=34;}
    }
    
    

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
