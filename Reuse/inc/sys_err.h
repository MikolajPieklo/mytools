/**
 ********************************************************************************
 * @file    sys_err.h
 * @author  mkpk
 * @date    25.03.13
 * @brief
 ********************************************************************************
 */

#ifndef __SYS_ERR_H__
#define __SYS_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/

#define ENOMEM    1 /* Memory allocation failure */                   /* Brak dostępnej pamięci */
#define EINVAL    2 /* Invalid operation or state */                  /* Błędna operacja lub stan */
#define EBUSY     3 /* Resource unavailable (e.g., UART, SPI busy) */ /* Zasób zajęty */
#define ETIMEDOUT 4 /* Operation timeout */                           /* Upłynął czas oczekiwania */
#define EINTR     5 /* Operation interrupted by signal */      /* Operacja przerwana sygnałem */
#define ENOSPC    6 /* Insufficient resources for operation */ /* Brak miejsca */
#define EAGAIN    7 /* Resource unavailable due to current state */ /* Spróbuj ponownie */
#define ENOENT    8 /* Requested resource not found */              /* Brak zasobu */
#define EPERM     9 /* Unauthorized access */                       /* Niedozwolona operacja */
#define EFAULT    10 /* Invalid input  */                           /* Błędny argument */
#define EMFILE    11 /* Too many open files */                /* Zbyt wiele otwartych plików */
#define ENOTSUPP  12 /* Operation not supported */            /* Operacja niedozwolona */
#define EOF       13 /* End of file or data stream reached */ /* Koniec pliku */
#define ELOCKED   14 /* Mutex acquisition failed */ /* Nie udało się zablokować semafora */

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

#endif /* __SYS_ERR_H__ */