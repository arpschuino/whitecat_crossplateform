// Serial.h

#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef _WIN32
// CSerial — communication série Win32 (Arduino via COM port)
// Sur Linux/macOS : version POSIX plus bas (fin de fichier)

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

class CSerial
{

public:
	CSerial();
	~CSerial();

	BOOL Open( int nPort = 2, int nBaud = 9600 );
	BOOL Close( void );

	int ReadData( void *, int );
	int SendData( unsigned char *, int );//christoph 21/04/2015 > unsigned char instead of char or data sended in PWM is trunkened a half !
	int ReadDataWaiting( void );
	BOOL Flush(void);

	BOOL IsOpened( void ){ return( m_bOpened ); }

protected:
	BOOL WriteCommByte( unsigned char );

	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	BOOL m_bOpened;

};

#else // !_WIN32
// ============================================================
// CSerial POSIX (Linux / macOS) — Arduino via port série USB
// Le numéro de port (arduino_com0 : 1..17 dans le panneau CFG Arduino,
// sauvegardé dans le show) désigne le N-ième port série USB trouvé :
//   Linux : /dev/ttyACM*, puis /dev/ttyUSB*
//   macOS : /dev/cu.usbmodem*, /dev/cu.usbserial*, /dev/cu.wchusbserial*
// Port ouvert en exclusif (TIOCEXCL) : le scan DMX Enttec ne peut plus
// l'ouvrir ensuite. Linux : l'utilisateur doit être dans le groupe dialout.
// ============================================================
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define WC_SERIAL_MAX_PORTS 17

// Liste les ports série USB candidats (ordre des motifs, puis alphabétique).
// Retourne leur nombre (<= maxn).
static inline int wc_serial_list_ports(char out[][64], int maxn)
{
#ifdef __APPLE__
    static const char* patterns[] = { "/dev/cu.usbmodem*", "/dev/cu.usbserial*", "/dev/cu.wchusbserial*" };
#else
    static const char* patterns[] = { "/dev/ttyACM*", "/dev/ttyUSB*" };
#endif
    int n = 0;
    for (size_t p = 0; p < sizeof(patterns) / sizeof(patterns[0]); p++) {
        glob_t g;
        if (glob(patterns[p], 0, NULL, &g) != 0) continue;
        for (size_t i = 0; i < g.gl_pathc && n < maxn; i++)
            snprintf(out[n++], 64, "%s", g.gl_pathv[i]);
        globfree(&g);
    }
    return n;
}

// Vitesses usuelles Arduino -> constantes termios (0 = non supportée)
static inline speed_t wc_serial_speed(int baud)
{
    switch (baud) {
    case 300:    return B300;
    case 1200:   return B1200;
    case 2400:   return B2400;
    case 4800:   return B4800;
    case 9600:   return B9600;
    case 19200:  return B19200;
    case 38400:  return B38400;
    case 57600:  return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    default:     return 0;
    }
}

class CSerial {
public:
    CSerial() : m_fd(-1) { m_szPort[0] = '\0'; }
    ~CSerial() { Close(); }

    // 8N1 brut, sans contrôle de flux. nPort : 1 = premier port de la liste.
    int Open(int nPort = 2, int nBaud = 9600)
    {
        if (m_fd >= 0) return 1;
        char ports[WC_SERIAL_MAX_PORTS][64];
        int n = wc_serial_list_ports(ports, WC_SERIAL_MAX_PORTS);
        speed_t speed = wc_serial_speed(nBaud);
        if (nPort < 1 || nPort > n || speed == 0) return 0;
        snprintf(m_szPort, sizeof(m_szPort), "%s", ports[nPort - 1]);

        int fd = open(m_szPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0) return 0;
        ioctl(fd, TIOCEXCL);

        struct termios tio;
        if (tcgetattr(fd, &tio) != 0) { close(fd); return 0; }
        cfmakeraw(&tio);
        tio.c_cflag |= CLOCAL | CREAD;
        tio.c_cflag &= ~(CSTOPB | CRTSCTS);
        tio.c_cc[VMIN]  = 0;   // lectures non bloquantes, comme la version Win32
        tio.c_cc[VTIME] = 0;
        cfsetispeed(&tio, speed);
        cfsetospeed(&tio, speed);
        if (tcsetattr(fd, TCSANOW, &tio) != 0) { close(fd); return 0; }
        tcflush(fd, TCIOFLUSH);

        m_fd = fd;
        return 1;
    }

    int Close()
    {
        if (m_fd >= 0) { close(m_fd); m_fd = -1; }
        return 1;
    }

    // Lit ce qui est déjà arrivé, au plus limit octets (jamais bloquant).
    int ReadData(void* buffer, int limit)
    {
        if (m_fd < 0 || limit <= 0) return 0;
        ssize_t r = read(m_fd, buffer, (size_t)limit);
        return (r > 0) ? (int)r : 0;
    }

    // Écrit tout le buffer ; attend au plus 1 s si le tampon de sortie est plein.
    int SendData(unsigned char* buffer, int size)
    {
        if (m_fd < 0) return 0;
        int done = 0;
        while (done < size) {
            ssize_t w = write(m_fd, buffer + done, (size_t)(size - done));
            if (w > 0) { done += (int)w; continue; }
            if (w < 0 && errno != EAGAIN && errno != EINTR) break; // port débranché...
            struct pollfd pfd;
            pfd.fd = m_fd; pfd.events = POLLOUT; pfd.revents = 0;
            if (poll(&pfd, 1, 1000) <= 0) break;
        }
        return done;
    }

    int ReadDataWaiting()
    {
        if (m_fd < 0) return 0;
        int n = 0;
        if (ioctl(m_fd, FIONREAD, &n) != 0) return 0;
        return n;
    }

    int Flush()          { return 1; }   // non appelé hors Win32 (cf. MAIN.cpp)
    int IsOpened()       { return m_fd >= 0; }
    const char* PortName() { return m_szPort; }

private:
    int  m_fd;
    char m_szPort[64];
};
#endif // _WIN32
#endif // __SERIAL_H__
