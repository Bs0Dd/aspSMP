/* Name: aspsmp.c
 * Project: SMP reader device for Electronika MK-90
 * Creation Date: 2009-07-31
 * Copyright: (c) 2009 by Ilya Danilov
 *            (c) 2026 by Bs0Dd [bs0dd.net]
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: aspsmp.c 692 2026-02-11 05:02:47Z cs $
 */

/*
General Description:

This is a host control program for SMP USB-adapter. 

Exit codes are:
     0 - operation OK
     1 - unknown command or command absent
     2 - missing filename
     3 - file operation failed (read or write)
     4 - verifying failed (after writing)
     10 - USB device not found

This program must be linked with libusb on Unix and libusb-win32 on Windows.
See http://libusb.sourceforge.net/ or http://libusb-win32.sourceforge.net/
respectively.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <usb.h>        /* this is libusb */
#include "opendevice.h" /* common code moved to separate module */

#include <requests.h>   /* custom request numbers */
#include <usbconfig.h>  /* device's VID/PID and names */

#define strcasecmp  strcmp

#define CARDSIZE    10240
#define MAXCARDSIZE 65536
#define CHUNK_SIZE  4096
#define INITBYTE    32

unsigned char       debug = 0;

int get_opt_key (int argc, char **argv, char *key) {
  int i;
  for (i=0;i<argc;i++) {
    if (!strcmp(key,argv[i])) return 1; 
  }
  return 0;
}

int get_opt_int (int argc, char **argv, char *key) {
  int i;
  for (i=0;i<argc;i++) {
    if (!strcmp(key,argv[i]) && i!=(argc-1)) {
      return atoi(argv[i+1]);
    }
  }
  return -1;
}

char *get_opt_string(int argc, char **argv, char *key) {
  int i;
  for (i=0;i<argc;i++) {
    if (!strcmp(key,argv[i]) && i!=(argc-1)) return argv[i+1]; 
  }
  return NULL;
}
static void usage()
{
    printf("\nusage:\n  aspsmp [command] [-f filename] [options]");
    printf("\n\nwhere command one of:\n");
    printf("  read  ..... perform cartridge read to filename\n"); 
    printf("  write ..... perform cartridge write and verify from filename\n"); 
    printf("  init  ..... initialize SMP if it was blocked"); 
    printf("\n\noptions may be:\n");
    printf("  -s NNNNN .. override SMP card size (default = %d)\n",CARDSIZE);
    printf("  -c NNNN ... redefine USB chunk size (default = %d)\n",CHUNK_SIZE);
    printf("  -i NNN .... override default init byte value (default = %d)\n",INITBYTE);
    printf("  -d  ....... add debug output\n");
}

int compare_buffers(unsigned char *read_buffer, unsigned char *verify_buffer, unsigned int buf_len, char is_init) {
  unsigned int i;
  int error_flag = 0;
  char all_zeros = 1;
  char all_fulls = 1;
  for (i=0;i<buf_len;i++) {
    if (read_buffer[i] != verify_buffer[i]) {
      if (!error_flag) { 
        error_flag = 1;
        printf("Verification ERROR!\nThere are differences between source after control reading\n");
        printf("==========\n");
      }
      printf("%X: %02x %02x\n",i,read_buffer[i],verify_buffer[i]);
    } 
  }
  if (error_flag) {
        printf("==========\n");
        printf("WARNING: write operation FAILED!\n");
        for (i=0;i<buf_len;i++) { // check if all readed bytes are zeros (probably no SMP)
          if (verify_buffer[i] != 0x0) {
            all_zeros = 0;
            break;
          }
        }
        if (all_zeros) {
          printf("Looks like all readed bytes are zeros.\nCheck the connection of the SMP cartridge.\n");
        }
        else {
          for (i=0;i<buf_len;i++) { // check if all readed bytes are 0xFF (bad connection of SMP is locked)
            if (verify_buffer[i] != 0xFF) {
              all_fulls = 0;
              break;
            }
          }
          if (all_fulls) {
            printf("Looks like all readed bytes are 0xFF.\nCheck the connection of the SMP cartridge");
            if (!is_init) {
              printf(" or try to initialize it");
            }
            printf(".\n");
          }  
        }
  } else 
    printf("Verification SMP contents after writing: OK\n");

  return error_flag;
}

FILE* open_file_write(char *filename) {
  FILE *dst = fopen(filename,"wb");
  if ( dst == NULL ){
    perror("Error opening file for writing"); 
    return NULL;
  }
  return dst;
}

int input_buffer(unsigned char *read_buffer, unsigned int buf_len, char *filename) {
  struct stat s;
  if (stat(filename,&s) == 0){
    if (s.st_mode & S_IFDIR){ // Linux directory opening "bug" workaround
        printf("Error opening file for reading: Is a directory\n"); 
        return 0;
    }
  } else {
    perror("Error opening file for reading"); 
    return 0;
  }
  FILE *src = fopen(filename,"rb");
  if ( src != NULL ){
    fread(read_buffer,buf_len,1,src);
    fclose(src);
  } else {
    perror("Error opening file for reading"); 
    return 0;
  }
  return 1;
}

int main(int argc, char **argv)
{
usb_dev_handle      *handle = NULL;
const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char                vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
unsigned char       read_buffer[MAXCARDSIZE];
unsigned char       verify_buffer[MAXCARDSIZE];
int                 cnt, vid, pid;

char                *filename = NULL;
unsigned int        chunk_size = CHUNK_SIZE;
unsigned int        card_size = CARDSIZE;
unsigned char       init_byte = INITBYTE;

    printf("aspSMP 1.0 - USBasp-based MK90 SMP programmer\n");

    usb_init();
    if(argc < 2){   
        usage();
        exit(1);
    }

    /* parse command line */
    filename = get_opt_string(argc,argv,"-f");
    if (get_opt_key(argc,argv,"-d")) debug = 1;
    if (get_opt_key(argc,argv,"-c")) { 
      int val = get_opt_int(argc,argv,"-c");
      if (val == -1) val = CHUNK_SIZE;
      if ( (val<8) || (val>4096) ) {
        if (debug) {
          printf("Illegal USB chunk size %d. Default value %d wil be used\n",val,CHUNK_SIZE);
        }
        val = CHUNK_SIZE;
      }
      chunk_size = val;
    }
    if (get_opt_key(argc,argv,"-s")) {
      int val = get_opt_int(argc,argv,"-s");
      if (val == -1) val = CARDSIZE;
      if ( (val<1) || (val>MAXCARDSIZE) ) {
        if (debug) {
          printf("Illegal card size %d. Default value %d wil be used\n",val,CARDSIZE);
        }
        val = CARDSIZE;
      }
      card_size = val;
    }
    if (get_opt_key(argc,argv,"-i")) {
      int val = get_opt_int(argc,argv,"-i");
      if (val == -1) val = INITBYTE;
      if ( (val<0) || (val>255) ) {
        if (debug) {
          printf("Illegal init byte %d. Default value %d wil be used\n",val,INITBYTE);
        }
        val = INITBYTE;
      }
      init_byte = val;
    }

    /* compute VID/PID from usbconfig.h so that there is a central source of information */
    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    /* The following function is in opendevice.c: */
    if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL, debug) != 0){
        printf("Could not find USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
        exit(10);
    }
    /* Since we use only control endpoint 0, we don't need to choose a
     * configuration and interface. Reading device descriptor and setting a
     * configuration and interface is done through endpoint 0 after all.
     * However, newer versions of Linux require that we claim an interface
     * even for endpoint 0. Enable the following code if your operating system
     * needs it: */
#if 0
    int retries = 1, usbConfiguration = 1, usbInterface = 0;
    if(usb_set_configuration(handle, usbConfiguration) && showWarnings){
        fprintf(stderr, "Warning: could not set configuration: %s\n", usb_strerror());
    }
    /* now try to claim the interface and detach the kernel HID driver on
     * Linux and other operating systems which support the call. */
    while((len = usb_claim_interface(handle, usbInterface)) != 0 && retries-- > 0){
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
        if(usb_detach_kernel_driver_np(handle, 0) < 0 && showWarnings){
            fprintf(stderr, "Warning: could not detach kernel driver: %s\n", usb_strerror());
        }
#endif
    }
#endif

    if(strcasecmp(argv[1], "read") == 0) {
        unsigned int remaining_bytes = card_size;
        unsigned int buffer_pointer = 0;
        FILE* file;
        if (!filename) {
          printf("Error: file name needed for this operation.\n");
          usage();
          exit(2);
        }
        if ((file = open_file_write(filename)) == NULL) {
          exit(3);
        }
        printf("Reading SMP...\n");
        while (remaining_bytes) {
          cnt = usb_control_msg( handle, 
                                 USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 
                                 CUSTOM_RQ_READ_DATA, buffer_pointer, 0, 
                                 read_buffer+buffer_pointer, 
                                 ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes), 
                                 5000); 
          if (cnt < 0) {
            printf("USB error: %s\n", usb_strerror());
            exit(10);
          } else {
            if (debug) printf("..read chunk from %d (%d)\n", buffer_pointer, ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes) );
            remaining_bytes -= cnt;
            buffer_pointer += cnt;
          }
        }
        if (!fwrite(read_buffer,card_size,1,file)) {
          perror("Error writing file");
          fclose(file);
          exit(3);
        }
        fclose(file);
        printf("Reading finished!\n");
    } else if (strcasecmp(argv[1], "write") == 0) {
        unsigned int remaining_bytes = card_size;
        unsigned int buffer_pointer = 0;
        if (!filename) {
          printf("Error: file name needed for this operation.\n");
          usage();
          exit(2);
        }
        if (!input_buffer(read_buffer,card_size,filename)) {
          exit(3);
        }
        printf("Writing SMP...\n");
        while (remaining_bytes) {
          cnt = usb_control_msg( handle, 
                                 USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, 
                                 CUSTOM_RQ_WRITE_DATA, buffer_pointer, 0, 
                                 read_buffer+buffer_pointer, 
                                 ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes), 
                                 5000); 
          if (cnt < 0) {
            printf("USB error: %s\n", usb_strerror());
            exit(10);
          } else {
            if (debug) printf("..write chunk from %d (%d)\n", buffer_pointer, ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes) );
            remaining_bytes -= cnt;
            buffer_pointer += cnt;
          }
        }
        /* read written data */
        printf("Verifying data...\n");
        remaining_bytes = card_size;
        buffer_pointer = 0;
        while (remaining_bytes) {
          cnt = usb_control_msg( handle, 
                                 USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 
                                 CUSTOM_RQ_READ_DATA, buffer_pointer, 0, 
                                 verify_buffer+buffer_pointer, 
                                 ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes), 
                                 5000); 
          if (cnt < 0) {
            printf("USB error: %s\n", usb_strerror());
            exit(10);
          } else {
            if (debug) printf("..read chunk from %d (%d)\n", buffer_pointer, ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes) );
            remaining_bytes -= cnt;
            buffer_pointer += cnt;
          }
        }
        /* compare two arrays */
        if (compare_buffers(read_buffer,verify_buffer,card_size,0)) {
          exit(4);
        }
    }else if(strcasecmp(argv[1], "init") == 0){
        unsigned int remaining_bytes = MAXCARDSIZE;
        unsigned int buffer_pointer = 0;
        int i;
        /* clear module */
        for (i=0; i < MAXCARDSIZE; i++) {
          read_buffer[i] = init_byte; // fill array with preferred init byte
        }
        printf("Initializing SMP...\n");
        while (remaining_bytes) {
          cnt = usb_control_msg( handle, 
                                 USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, 
                                 CUSTOM_RQ_INIT_MEM, remaining_bytes, 0, read_buffer, 
                                 ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes), 
                                 5000); 
          if (cnt < 0) {
            printf("USB error: %s\n", usb_strerror());
            exit(10);
          } else {
            if (debug) printf("..write init chunk from %d (%d)\n", remaining_bytes, ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes) );
            remaining_bytes -= cnt;
          }
        }
        if (cnt < 0) {
          printf("USB error: %s\n", usb_strerror());
          exit(10);
        }
        /* read written data */
        printf("Verifying data...\n");
        remaining_bytes = card_size;
        while (remaining_bytes) {
          cnt = usb_control_msg( handle, 
                                 USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 
                                 CUSTOM_RQ_READ_DATA, buffer_pointer, 0, 
                                 verify_buffer+buffer_pointer, 
                                 ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes), 
                                 5000); 
          if (cnt < 0) {
            printf("USB error: %s\n", usb_strerror());
            exit(10);
          } else {
            if (debug) printf("..read chunk from %d (%d)\n", buffer_pointer, ((chunk_size <= remaining_bytes) ? chunk_size : remaining_bytes) );
            remaining_bytes -= cnt;
            buffer_pointer += cnt;
          }
        }
        /* compare two arrays */
        if (compare_buffers(read_buffer,verify_buffer,card_size,1)) {
          exit(4);
        }
    }else{
        usage(argv[0]);
        exit(1);
    }
    usb_close(handle);
    return 0;
}
