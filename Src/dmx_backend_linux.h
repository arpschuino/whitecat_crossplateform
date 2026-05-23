/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2026  Jacques Bouault - arpschuino.fr
       WWWWWWWWWWWWWWW           |
     WWWWWWWWWWWWWWWWWWW         | This file is part of White Cat.
    WWWWWWWWWWWWWWWWWCWWWW       |
   WWWWWWWWWWWWWWWWW tWWWWW      | White Cat is free software: you can redistribute it and/or modify
  WWWW   WWWWWWWWWW  tWWWWWW     | it under the terms of the GNU General Public License as published by
 WWWWWt              tWWWWWWa    | the Free Software Foundation, either version 2 of the License, or
 WWWWWW               WWWWWWW    | (at your option) any later version.
WWWWWWWW              WWWWWWW    |
WWWWWWWW               WWWWWWW   |
WWWWWWW               WWWWWWWW   |
WWWWWWW      CWWW    W WWWWWWW   |
WWWWWWW            aW  WWWWWWW   |
WWWWWWWW           C  WWWWWWWW   |
 WWWWWWWW            CWWWWWWW    |
 WWWWWWWWW          WWWWWWWWW    |
  WWWWWWWWWWC    CWWWWWWWWWW     |
   WWWWWWWWWWWWWWWWWWWWWWWW      |
    WWWWWWWWWWWWWWWWWWWWWW       |
      WWWWWWWWWWWWWWWWWWa        |
        WWWWWWWWWWWWWWW          |
           WWWWWWWWt             |
                                 |
---------------------------------------------------------------------------------------------------------------*/
/**
 * \file dmx_backend_linux.h
 * \brief Backend DMX POSIX — remplace FTD2XX + Win32 serial sur Linux/macOS
 *
 * Interface 2 — Enttec Open DMX USB :
 *   POSIX serial à 250000 baud, 8N2.
 *   Break via ioctl TIOCSBRK/TIOCCBRK.
 *   Thread pthread pour envoi continu ~40 fps.
 *   Baud 250000 via termios2/BOTHER (kernel Linux direct).
 *
 *   Prérequis système :
 *     sudo usermod -aG dialout $USER
 *     udev rule : SUBSYSTEM=="tty", ATTRS{idVendor}=="0403",
 *                 ATTRS{idProduct}=="6001", MODE="0666", GROUP="dialout"
 *
 * Interface 3 — Enttec Pro :
 *   POSIX serial à 57600 baud, 8N1.
 *   Même protocole qu'en Win32 (0x7E + label + len + data + 0xE7).
 *   Détection par scan /dev/ttyUSB0..9.
 *
 * Interface 4 — Sunlite :
 *   Non disponible (DLL Windows propriétaire). Stubs.
 *
 * ArtNet (interface 1) : déjà portable via POSIX sockets — non concerné.
 **/

#pragma once
#ifndef _WIN32

#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

// ============================================================
// termios2 / BOTHER — baud rate 250000 (non standard POSIX)
// Le kernel Linux expose TCGETS2/TCSETS2 via ioctl.
// On définit la structure ici pour éviter le conflit de headers
// entre <termios.h> et <linux/termios.h>.
// ============================================================
#ifndef BOTHER
#define BOTHER 0010000
struct termios2 {
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t     c_line;
    cc_t     c_cc[19];
    speed_t  c_ispeed;
    speed_t  c_ospeed;
};
#define TCGETS2 _IOR('T', 0x2A, struct termios2)
#define TCSETS2 _IOW('T', 0x2B, struct termios2)
#endif

// ============================================================
// Variables globales (équivalent HANDLE Win32 → fd POSIX)
// ============================================================
static int  com_handle_   = -1;  // fd Enttec Pro OUT
static int  com_handle_IN = -1;  // fd Enttec Pro IN
static int  resIn         = 0;

static char DeviceName[256]   = "/dev/ttyUSB0";
static char DeviceNameIN[256] = "/dev/ttyUSB1";

int sunlite_version_number = 0;
int sunlite_serial_number  = 0;

#define SET_DMX_RX_MODE 5
#define SET_DMX_TX_MODE 6

// ============================================================
// Utilitaire interne : ouvrir et configurer un port série
// baud : 250000 (Enttec Open, 8N2) ou 57600 (Enttec Pro, 8N1)
// stop2 : true = 2 stop bits (DMX), false = 1 stop bit
// ============================================================
static int wc_serial_open_dmx(const char* path, int baud, bool stop2)
{
    int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) return -1;

    struct termios tio;
    memset(&tio, 0, sizeof(tio));
    tio.c_cflag  = CS8 | CLOCAL | CREAD;
    if (stop2) tio.c_cflag |= CSTOPB;
    tio.c_cc[VMIN]  = 0;
    tio.c_cc[VTIME] = 5; // timeout lectures : 0.5 s

    if (baud == 250000) {
        // 250000 n'est pas une vitesse standard POSIX.
        // On passe par termios2 + BOTHER (accès direct kernel Linux).
        cfsetispeed(&tio, B38400); // valeur intermédiaire — sera remplacée
        cfsetospeed(&tio, B38400);
        tcsetattr(fd, TCSANOW, &tio);

        struct termios2 tio2;
        if (ioctl(fd, TCGETS2, &tio2) == 0) {
            tio2.c_cflag &= ~CBAUD;
            tio2.c_cflag |= BOTHER;
            tio2.c_ispeed = 250000;
            tio2.c_ospeed = 250000;
            ioctl(fd, TCSETS2, &tio2);
        }
    } else {
        // Vitesse standard (ex: 57600 pour Enttec Pro)
        cfsetispeed(&tio, B57600);
        cfsetospeed(&tio, B57600);
        tcsetattr(fd, TCSANOW, &tio);
    }
    return fd;
}

// ============================================================
// Interface 2 — Enttec Open DMX USB
// Classe Open_USB_DMX : même interface que la version Win32,
// implémentée avec POSIX serial + pthread.
// ============================================================
class Open_USB_DMX {
    int              fd_;
    pthread_t        thread_;
    volatile bool    running_;
    volatile bool    kill_;
    unsigned char    m_DMXData_frame[512];

    static void* thread_func(void* arg) {
        Open_USB_DMX* self = static_cast<Open_USB_DMX*>(arg);
        while (!self->kill_) {
            self->SendDataToHardware();
            usleep(25000); // 25 ms ≈ 40 frames DMX/s
        }
        return NULL;
    }

    void SendDataToHardware() {
        if (fd_ < 0) return;

        // Break DMX : ≥ 88 µs à l'état bas
        ioctl(fd_, TIOCSBRK);
        usleep(110);
        ioctl(fd_, TIOCCBRK);

        // MAB (Mark After Break) : ≥ 8 µs à l'état haut
        usleep(12);

        // Trame DMX : start code (0x00) + 512 canaux
        unsigned char frame[513];
        frame[0] = 0x00;
        memcpy(frame + 1, m_DMXData_frame, 512);
        write(fd_, frame, 513);
    }

public:
    Open_USB_DMX() : fd_(-1), running_(false), kill_(false) {
        memset(m_DMXData_frame, 0, 512);
    }
    ~Open_USB_DMX() { close_dmx_devices(); }

    int open_dmx_devices() {
        // Scan /dev/ttyUSB0..9 — prend le premier accessible à 250k baud
        fd_ = -1;
        for (int i = 0; i < 10 && fd_ < 0; i++) {
            char path[32];
            snprintf(path, sizeof(path), "/dev/ttyUSB%d", i);
            fd_ = wc_serial_open_dmx(path, 250000, true);
        }
        if (fd_ < 0) {
            sprintf(string_display_dmx_params,
                    "Enttec Open: aucun /dev/ttyUSBx disponible");
            return 0;
        }
        tcflush(fd_, TCIOFLUSH);
        kill_    = false;
        running_ = false;
        if (pthread_create(&thread_, NULL, thread_func, this) == 0)
            running_ = true;
        return 1; // succès
    }

    void close_dmx_devices() {
        if (running_) {
            kill_ = true;
            pthread_join(thread_, NULL);
            running_ = false;
        }
        if (fd_ >= 0) { close(fd_); fd_ = -1; }
    }

    void send_dmx_packet(unsigned char* data) {
        memcpy(m_DMXData_frame, data, 512);
    }
};

// ============================================================
// Interface 3 — Enttec Pro (POSIX serial 57600 baud)
// Détection par scan /dev/ttyUSB0..9.
// Protocole identique Win32 : 0x7E + label + len(2) + data + 0xE7
// ============================================================

static int Detect_EnttecProOut() {
    com_handle_ = -1;
    for (int i = 0; i < 10; i++) {
        char path[32];
        snprintf(path, sizeof(path), "/dev/ttyUSB%d", i);
        int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd >= 0) {
            close(fd);
            snprintf(DeviceName, sizeof(DeviceName), "/dev/ttyUSB%d", i);
            index_init_dmx_ok = 1;
            sprintf(string_display_dmx_params,
                    "ENTTEC PRO Out : %s", DeviceName);
            return 0;
        }
    }
    sprintf(string_display_dmx_params, "ENTTEC PRO : aucun /dev/ttyUSBx détecté.");
    return 0;
}

static int Detect_EnttecProIn() {
    com_handle_IN = -1;
    // Commence à ttyUSB1 pour ne pas prendre le même que OUT
    for (int i = 1; i < 10; i++) {
        char path[32];
        snprintf(path, sizeof(path), "/dev/ttyUSB%d", i);
        int fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd >= 0) {
            close(fd);
            snprintf(DeviceNameIN, sizeof(DeviceNameIN), "/dev/ttyUSB%d", i);
            index_init_EnttecPROIN_ok = 1;
            sprintf(string_display_dmx_params,
                    "ENTTEC PRO In : %s", DeviceNameIN);
            return 0;
        }
    }
    sprintf(string_display_dmx_params, "ENTTEC PRO IN : aucun port détecté.");
    return 0;
}

static int Open_EnttecProOut() {
    com_handle_ = wc_serial_open_dmx(DeviceName, 57600, false); // 8N1
    if (com_handle_ < 0) {
        index_init_dmx_ok = 0;
        sprintf(string_display_dmx_params,
                "Impossible d'ouvrir %s — branché ?", DeviceName);
        return 0;
    }
    tcflush(com_handle_, TCIOFLUSH);
    sprintf(string_display_dmx_params,
            "ENTTEC PRO Out ouvert sur %s", DeviceName);
    return 0;
}

static int Close_EnttecProOut() {
    if (com_handle_ >= 0) { close(com_handle_); com_handle_ = -1; }
    sprintf(string_display_dmx_params, "ENTTEC PRO OUT : Fermé.");
    return 0;
}

static int Enttec_Pro_SendData(int label, unsigned char* data,
                               unsigned int length, void* /*lpOverlapped*/)
{
    if (com_handle_ < 0) return -1;

    unsigned char header[4];
    header[0] = 0x7E;
    header[1] = (unsigned char)label;
    header[2] = length & 0xFF;
    header[3] = (length >> 8) & 0xFF;

    if (write(com_handle_, header, 4) != 4)               return -1;
    if (write(com_handle_, data, length) != (ssize_t)length) return -1;
    unsigned char end = 0xE7;
    if (write(com_handle_, &end, 1) != 1)                 return -1;
    return 0;
}

static int Enttec_Pro_ReceiveData(int label, unsigned char* data,
                                  unsigned int expected_length)
{
    if (com_handle_IN < 0) return 0;

    uint8_t byte = 0;
    int tries = 0;

    // Cherche le marqueur de début 0x7E
    while (byte != 0x7E && tries++ < 1024) {
        if (read(com_handle_IN, &byte, 1) != 1) return 0;
    }
    if (byte != 0x7E) return 0;

    // Label
    if (read(com_handle_IN, &byte, 1) != 1) return 0;
    if (byte != (uint8_t)label) return 0;

    // Longueur (LSB puis MSB)
    uint8_t len_lo, len_hi;
    if (read(com_handle_IN, &len_lo, 1) != 1) return 0;
    if (read(com_handle_IN, &len_hi, 1) != 1) return 0;
    unsigned int length = len_lo | ((unsigned int)len_hi << 8);
    if (length > 513) return 0;

    // Données
    unsigned char buf[513];
    ssize_t got = read(com_handle_IN, buf, length);
    if (got <= 0) return 0;

    // Code de fin 0xE7
    if (read(com_handle_IN, &byte, 1) != 1 || byte != 0xE7) return 0;

    unsigned int n = (expected_length < (unsigned)got)
                     ? expected_length : (unsigned)got;
    memcpy(data, buf, n);
    return (int)n;
}

static int Open_ProIn() {
    com_handle_IN = wc_serial_open_dmx(DeviceNameIN, 57600, false);
    if (com_handle_IN < 0) {
        sprintf(string_display_dmx_params,
                "Impossible d'ouvrir %s — branché ?", DeviceNameIN);
        return 0;
    }
    tcflush(com_handle_IN, TCIOFLUSH);
    index_init_EnttecPROIN_ok = 1;
    sprintf(string_display_dmx_params,
            "Enttec Pro IN %s ouvert", DeviceNameIN);
    return 0;
}

static int Close_ProIn() {
    if (com_handle_IN >= 0) { close(com_handle_IN); com_handle_IN = -1; }
    index_init_EnttecPROIN_ok = 0;
    sprintf(string_display_dmx_params,
            "Enttec Pro IN %s fermé", DeviceNameIN);
    return 0;
}

// ============================================================
// Interface 4 — Sunlite (DLL Windows propriétaire — non portable)
// ============================================================
static int open_sunlite() {
    sprintf(string_display_dmx_params,
            "Sunlite : non disponible sur Linux/macOS");
    return -1;
}
static int close_sunlite()         { return 0; }
static int sunlite_send_data()     { return 0; }
static int Receive_sunlite_dmxIN() { return 0; }

#endif // !_WIN32
