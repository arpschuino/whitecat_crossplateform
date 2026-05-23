// Serial.h

#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef _WIN32
// CSerial — communication série Win32 (Arduino via COM port)
// Sur Linux/macOS : utiliser POSIX open/read/write sur /dev/ttyACM0

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
// Stub CSerial pour compilation POSIX — Arduino série via /dev/ttyACM0 à implémenter
class CSerial {
public:
    CSerial() {}
    ~CSerial() {}
    int Open(int nPort = 2, int nBaud = 9600) { (void)nPort; (void)nBaud; return 0; }
    int Close()                               { return 0; }
    int ReadData(void*, int)                  { return 0; }
    int SendData(unsigned char*, int)         { return 0; }
    int ReadDataWaiting()                     { return 0; }
    int Flush()                               { return 0; }
    int IsOpened()                            { return 0; }
};
#endif // _WIN32
#endif // __SERIAL_H__
