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


#ifdef __cplusplus
}
#endif

#endif