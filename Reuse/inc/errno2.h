/**
 ********************************************************************************
 * @file    errno.h
 * @author  mkpk
 * @date    25.03.13
 * @brief
 ********************************************************************************
 */

#ifndef __ERRNO_H__
#define __ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/

/* clang-format off */
#define EPERM           1 /* Operation not permitted */                          /* Nieautoryzowana operacja */
#define ENOENT          2 /* No such file or directory */                        /* Brak takiego pliku lub katalogu */
#define ESRCH           3 /* No such process */                                  /* Brak takiego procesu */
#define EINTR           4 /* Interrupted system call */                          /* Przerwane wywołanie systemowe */
#define EIO             5 /* I/O error */                                        /* Błąd wejścia/wyjścia */
#define ENXIO           6 /* No such device or address */                        /* Brak takiego urządzenia lub adresu */
#define E2BIG           7 /* Argument list too long */                           /* Lista argumentów za długa */
#define ENOEXEC         8 /* Exec format error */                                /* Błąd formatu pliku wykonawczego */
#define EBADF           9 /* Bad file number */                                  /* Błędny numer pliku */
#define ECHILD          10 /* No child processes */                              /* Brak procesów potomnych */
#define EAGAIN          11 /* Try again */                                       /* Spróbuj ponownie */
#define ENOMEM          12 /* Out of memory */                                   /* Brak pamięci */
#define EACCES          13 /* Permission denied */                               /* Brak uprawnień */
#define EFAULT          14 /* Bad address */                                     /* Błędny adres */
#define ENOTBLK         15 /* Block device required */                           /* Wymagane urządzenie blokowe */
#define EBUSY           16 /* Device or resource busy */                         /* Urządzenie lub zasób zajęty */
#define EEXIST          17 /* File exists */                                     /* Plik już istnieje */
#define EXDEV           18 /* Cross-device link */                               /* Link między różnymi urządzeniami */
#define ENODEV          19 /* No such device */                                  /* Brak takiego urządzenia */
#define ENOTDIR         20 /* Not a directory */                                 /* To nie jest katalog */
#define EISDIR          21 /* Is a directory */                                  /* To jest katalog */
#define EINVAL          22 /* Invalid argument */                                /* Błędny argument */
#define ENFILE          23 /* File table overflow */                             /* Przepełnienie tabeli plików */
#define EMFILE          24 /* Too many open files */                             /* Zbyt wiele otwartych plików */
#define ENOTTY          25 /* Not a typewriter */                                /* To nie jest terminal */
#define ETXTBSY         26 /* Text file busy */                                  /* Plik tekstowy zajęty */
#define EFBIG           27 /* File too large */                                  /* Plik zbyt duży */
#define ENOSPC          28 /* No space left on device */                         /* Brak miejsca na urządzeniu */
#define ESPIPE          29 /* Illegal seek */                                    /* Niedozwolone poszukiwanie */
#define EROFS           30 /* Read-only file system */                           /* Tylko do odczytu system plików */
#define EMLINK          31 /* Too many links */                                  /* Zbyt wiele linków */
#define EPIPE           32 /* Broken pipe */                                     /* Uszkodzona rura (pipe) */
#define EDOM            33 /* Math argument out of domain of func */             /* Argument matematyczny poza zakresem funkcji */
#define ERANGE          34 /* Math result not representable */                   /* Wynik matematyczny nie może być przedstawiony */
#define EDEADLK         35 /* Resource deadlock would occur */                   /* Wystąpił martwy blok (deadlock) */
#define ENAMETOOLONG    36 /* File name too long */                              /* Zbyt długi nazwisko pliku */
#define ENOLCK          37 /* No record locks available */                       /* Brak dostępnych blokad rekordów */
#define ENOSYS          38 /* Function not implemented */                        /* Funkcja nie jest zaimplementowana */
#define ENOTEMPTY       39 /* Directory not empty */                             /* Katalog nie jest pusty */
#define ELOOP           40 /* Too many symbolic links encountered */             /* Zbyt wiele napotkanych linków symbolicznych */
#define EWOULDBLOCK     EAGAIN /* Operation would block */                       /* Operacja blokująca */
#define ENOMSG          42 /* No message of desired type */                      /* Brak wiadomości odpowiedniego typu */
#define EIDRM           43 /* Identifier removed */                              /* Identyfikator usunięty */
#define ECHRNG          44 /* Channel number out of range */                     /* Numer kanału poza zakresem */
#define EL2NSYNC        45 /* Level 2 not synchronized */                        /* Poziom 2 niezsynchronizowany */
#define EL3HLT          46 /* Level 3 halted */                                  /* Poziom 3 zatrzymany */
#define EL3RST          47 /* Level 3 reset */                                   /* Poziom 3 zresetowany */
#define ELNRNG          48 /* Link number out of range */                        /* Numer linku poza zakresem */
#define EUNATCH         49 /* Protocol driver not attached */                    /* Sterownik protokołu nieprzypisany */
#define ENOCSI          50 /* No CSI structure available */                      /* Brak dostępnej struktury CSI */
#define EL2HLT          51 /* Level 2 halted */                                  /* Poziom 2 zatrzymany */
#define EBADE           52 /* Invalid exchange */                                /* Błędna wymiana */
#define EBADR           53 /* Invalid request descriptor */                      /* Błędny deskryptor żądania */
#define EXFULL          54 /* Exchange full */                                   /* Wymiana pełna */
#define ENOANO          55 /* No anode */                                        /* Brak anody */
#define EBADRQC         56 /* Invalid request code */                            /* Błędny kod żądania */
#define EBADSLT         57 /* Invalid slot */                                    /* Błędny slot */
#define EBFONT          59 /* Bad font file format */                            /* Błędny format pliku czcionki */
#define ENOSTR          60 /* Device not a stream */                             /* Urządzenie nie jest strumieniem */
#define ENODATA         61 /* No data available */                               /* Brak dostępnych danych */
#define ETIME           62 /* Timer expired */                                   /* Upłynął czas timera */
#define ENOSR           63 /* Out of streams resources */                        /* Brak zasobów strumieni */
#define ENONET          64 /* Machine is not on the network */                   /* Maszyna nie jest w sieci */
#define ENOPKG          65 /* Package not installed */                           /* Pakiet nie jest zainstalowany */
#define EREMOTE         66 /* Object is remote */                                /* Obiekt jest zdalny */
#define ENOLINK         67 /* Link has been severed */                           /* Link został przerwany */
#define EADV            68 /* Advertise error */                                 /* Błąd ogłoszenia */
#define ESRMNT          69 /* Srmount error */                                   /* Błąd montowania SR */
#define ECOMM           70 /* Communication error on send */                     /* Błąd komunikacji przy wysyłaniu */
#define EPROTO          71 /* Protocol error */                                  /* Błąd protokołu */
#define EMULTIHOP       72 /* Multihop attempted */                              /* Próba wieloetapowego przeskoku */
#define EDOTDOT         73 /* RFS specific error */                              /* Specyficzny błąd RFS */
#define EBADMSG         74 /* Bad message */                                     /* Błędna wiadomość */
#define EOVERFLOW       75 /* Value too large for defined data type */           /* Wartość zbyt duża dla określonego typu danych */
#define ENOTUNIQ        76 /* Name not unique on network */                      /* Nazwa nieunikalna w sieci */
#define EBADFD          77 /* File descriptor in bad state */                    /* Deskryptor pliku w złym stanie */
#define EREMCHG         78 /* Remote address changed */                          /* Zmieniony zdalny adres */
#define ELIBACC         79 /* Can not access a needed shared library */          /* Nie można uzyskać dostępu do wymaganej biblioteki współdzielonej */
#define ELIBBAD         80 /* Accessing a corrupted shared library */            /* Dostęp do uszkodzonej biblioteki współdzielonej */
#define ELIBSCN         81 /* .lib section in a.out corrupted */                 /* Sekcja .lib w pliku a.out uszkodzona */
#define ELIBMAX         82 /* Attempting to link in too many shared libraries */ /* Próba załadowania zbyt wielu bibliotek współdzielonych */
#define ELIBEXEC        83 /* Cannot exec a shared library directly */           /* Nie można bezpośrednio uruchomić biblioteki współdzielonej */
#define EILSEQ          84 /* Illegal byte sequence */                           /* Nielegalna sekwencja bajtów */
#define ERESTART        85 /* Interrupted system call should be restarted */     /* Wywołanie systemowe przerwane, należy  je ponownie uruchomić */
#define ESTRPIPE        86 /* Streams pipe error */                              /* Błąd strumienia rury */
#define EUSERS          87 /* Too many users */                                  /* Zbyt wielu użytkowników */
#define ENOTSOCK        88 /* Socket operation on non-socket */                  /* Operacja na czymś, co nie jest gniazdem */
#define EDESTADDRREQ    89 /* Destination address required */                    /* Wymagany adres docelowy */
#define EMSGSIZE        90 /* Message too long */                                /* Wiadomość za długa */
#define EPROTOTYPE      91 /* Protocol wrong type for socket */                  /* Błędny typ protokołu dla gniazda */
#define ENOPROTOOPT     92 /* Protocol not available */                          /* Protokół niedostępny */
#define EPROTONOSUPPORT 93 /* Protocol not supported */                          /* Protokół nieobsługiwany */
#define ESOCKTNOSUPPORT 94 /* Socket type not supported */                       /* Typ gniazda nieobsługiwany */
#define EOPNOTSUPP      95 /* Operation not supported on transport endpoint */   /* Operacja niedozwolona na końcówce transportu */
#define EPFNOSUPPORT    96 /* Protocol family not supported */                   /* Rodzina protokołów nieobsługiwana */
#define EAFNOSUPPORT    97 /* Address family not supported by protocol */        /* Rodzina adresów nieobsługiwana przez protokół */
#define EADDRINUSE      98 /* Address already in use */                          /* Adres już w użyciu */
#define EADDRNOTAVAIL   99 /* Cannot assign requested address */                 /* Nie można przypisać żądanego adresu */
#define ENETDOWN        100 /* Network is down */                                /* Sieć jest niedostępna */
#define ENETUNREACH     101 /* Network is unreachable */                         /* Sieć jest niedostępna */
#define ENETRESET       102 /* Network dropped connection because of reset */    /* Sieć zresetowała połączenie */
#define ECONNABORTED    103 /* Software caused connection abort */               /* Połączenie zostało przerwane przez oprogramowanie */
#define ECONNRESET      104 /* Connection reset by peer */                       /* Połączenie zresetowane przez drugą stronę */
#define ENOBUFS         105 /* No buffer space available */                      /* Brak dostępnej przestrzeni bufora */
#define EISCONN         106 /* Transport endpoint is already connected */        /* Końcówka transportu jest już połączona */
#define ENOTCONN        107 /* Transport endpoint is not connected */            /* Końcówka transportu nie jest połączona */
#define ESHUTDOWN       108 /* Cannot send after transport endpoint shutdown */  /* Nie można wysłać po zamknięciu  końcówki transportu */
#define ETOOMANYREFS    109 /* Too many references: cannot splice */             /* Zbyt wiele referencji, nie można połączyć */
#define ETIMEDOUT       110 /* Connection timed out */                           /* Połączenie wygasło */
#define ECONNREFUSED    111 /* Connection refused */                             /* Połączenie odrzucone */
#define EHOSTDOWN       112 /* Host is down */                                   /* Host jest niedostępny */
#define ENETUNREACH     113 /* No route to host */                               /* Brak trasy do hosta */
#define ELAST           113                                                      /* Must be equal largest errno */
/* clang-format on */
/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/

#define EPERM   1  /* Not owner */
#define ENOENT  2  /* No such file or directory */
#define ESRCH   3  /* No such process */
#define EINTR   4  /* Interrupted system call */
#define EIO     5  /* I/O error */
#define ENXIO   6  /* No such device or address */
#define E2BIG   7  /* Arg list too long */
#define ENOEXEC 8  /* Exec format error */
#define EBADF   9  /* Bad file number */
#define ECHILD  10 /* No children */
#define EAGAIN  11 /* No more processes */
#define ENOMEM  12 /* Not enough space */
#define EACCES  13 /* Permission denied */
#define EFAULT  14 /* Bad address */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ENOTBLK 15 /* Block device required */
#endif
#define EBUSY   16 /* Device or resource busy */
#define EEXIST  17 /* File exists */
#define EXDEV   18 /* Cross-device link */
#define ENODEV  19 /* No such device */
#define ENOTDIR 20 /* Not a directory */
#define EISDIR  21 /* Is a directory */
#define EINVAL  22 /* Invalid argument */
#define ENFILE  23 /* Too many open files in system */
#define EMFILE  24 /* File descriptor value too large */
#define ENOTTY  25 /* Not a character device */
#define ETXTBSY 26 /* Text file busy */
#define EFBIG   27 /* File too large */
#define ENOSPC  28 /* No space left on device */
#define ESPIPE  29 /* Illegal seek */
#define EROFS   30 /* Read-only file system */
#define EMLINK  31 /* Too many links */
#define EPIPE   32 /* Broken pipe */
#define EDOM    33 /* Mathematics argument out of domain of function */
#define ERANGE  34 /* Result too large */
#define ENOMSG  35 /* No message of desired type */
#define EIDRM   36 /* Identifier removed */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ECHRNG   37 /* Channel number out of range */
#define EL2NSYNC 38 /* Level 2 not synchronized */
#define EL3HLT   39 /* Level 3 halted */
#define EL3RST   40 /* Level 3 reset */
#define ELNRNG   41 /* Link number out of range */
#define EUNATCH  42 /* Protocol driver not attached */
#define ENOCSI   43 /* No CSI structure available */
#define EL2HLT   44 /* Level 2 halted */
#endif
#define EDEADLK 45 /* Deadlock */
#define ENOLCK  46 /* No lock */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define EBADE     50 /* Invalid exchange */
#define EBADR     51 /* Invalid request descriptor */
#define EXFULL    52 /* Exchange full */
#define ENOANO    53 /* No anode */
#define EBADRQC   54 /* Invalid request code */
#define EBADSLT   55 /* Invalid slot */
#define EDEADLOCK 56 /* File locking deadlock error */
#define EBFONT    57 /* Bad font file fmt */
#endif
#define ENOSTR  60 /* Not a stream */
#define ENODATA 61 /* No data (for no delay io) */
#define ETIME   62 /* Stream ioctl timeout */
#define ENOSR   63 /* No stream resources */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ENONET  64 /* Machine is not on the network */
#define ENOPKG  65 /* Package not installed */
#define EREMOTE 66 /* The object is remote */
#endif
#define ENOLINK 67 /* Virtual circuit is gone */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define EADV   68 /* Advertise error */
#define ESRMNT 69 /* Srmount error */
#define ECOMM  70 /* Communication error on send */
#endif
#define EPROTO    71 /* Protocol error */
#define EMULTIHOP 74 /* Multihop attempted */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ELBIN   75 /* Inode is remote (not really error) */
#define EDOTDOT 76 /* Cross mount point (not really error) */
#endif
#define EBADMSG 77 /* Bad message */
#define EFTYPE  79 /* Inappropriate file type or format */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ENOTUNIQ 80 /* Given log. name not unique */
#define EBADFD   81 /* f.d. invalid for this operation */
#define EREMCHG  82 /* Remote address changed */
#define ELIBACC  83 /* Can't access a needed shared lib */
#define ELIBBAD  84 /* Accessing a corrupted shared lib */
#define ELIBSCN  85 /* .lib section in a.out corrupted */
#define ELIBMAX  86 /* Attempting to link in too many libs */
#define ELIBEXEC 87 /* Attempting to exec a shared library */
#endif
#define ENOSYS 88 /* Function not implemented */
#ifdef __CYGWIN__
#define ENMFILE 89 /* No more files */
#endif
#define ENOTEMPTY    90  /* Directory not empty */
#define ENAMETOOLONG 91  /* File or path name too long */
#define ELOOP        92  /* Too many symbolic links */
#define EOPNOTSUPP   95  /* Operation not supported on socket */
#define EPFNOSUPPORT 96  /* Protocol family not supported */
#define ECONNRESET   104 /* Connection reset by peer */
#define ENOBUFS      105 /* No buffer space available */
#define EAFNOSUPPORT 106 /* Address family not supported by protocol family */
#define EPROTOTYPE   107 /* Protocol wrong type for socket */
#define ENOTSOCK     108 /* Socket operation on non-socket */
#define ENOPROTOOPT  109 /* Protocol not available */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ESHUTDOWN 110 /* Can't send after socket shutdown */
#endif
#define ECONNREFUSED    111 /* Connection refused */
#define EADDRINUSE      112 /* Address already in use */
#define ECONNABORTED    113 /* Software caused connection abort */
#define ENETUNREACH     114 /* Network is unreachable */
#define ENETDOWN        115 /* Network interface is not configured */
#define ETIMEDOUT       116 /* Connection timed out */
#define EHOSTDOWN       117 /* Host is down */
#define EHOSTUNREACH    118 /* Host is unreachable */
#define EINPROGRESS     119 /* Connection already in progress */
#define EALREADY        120 /* Socket already connected */
#define EDESTADDRREQ    121 /* Destination address required */
#define EMSGSIZE        122 /* Message too long */
#define EPROTONOSUPPORT 123 /* Unknown protocol */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ESOCKTNOSUPPORT 124 /* Socket type not supported */
#endif
#define EADDRNOTAVAIL 125 /* Address not available */
#define ENETRESET     126 /* Connection aborted by network */
#define EISCONN       127 /* Socket is already connected */
#define ENOTCONN      128 /* Socket is not connected */
#define ETOOMANYREFS  129
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define EPROCLIM 130
#define EUSERS   131
#endif
#define EDQUOT  132
#define ESTALE  133
#define ENOTSUP 134 /* Not supported */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ENOMEDIUM 135 /* No medium (in tape drive) */
#endif
#if defined(__CYGWIN__) && !defined(__INSIDE_CYGWIN__)
#define ENOSHARE   (_Pragma("GCC warning \"'ENOSHARE' is no longer used by Cygwin\"") 136)
#define ECASECLASH (_Pragma("GCC warning \"'ECASECLASH' is no longer used by Cygwin\"") 137)
#endif
#define EILSEQ          138 /* Illegal byte sequence */
#define EOVERFLOW       139 /* Value too large for defined data type */
#define ECANCELED       140 /* Operation canceled */
#define ENOTRECOVERABLE 141 /* State not recoverable */
#define EOWNERDEAD      142 /* Previous owner died */
#ifdef __LINUX_ERRNO_EXTENSIONS__
#define ESTRPIPE 143 /* Streams pipe error */
#endif
#define EWOULDBLOCK EAGAIN /* Operation would block */

#define __ELASTERROR 2000 /* Users can add values starting here */



#ifdef __cplusplus
}
#endif

#endif