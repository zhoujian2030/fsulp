#include "dci0.h"

#define BIT_MASK_1      0x00000001
#define BIT_MASK_2      0x00000003
#define BIT_MASK_3      0x00000007
#define BIT_MASK_4      0x0000000f
#define BIT_MASK_5      0x0000001f
#define BIT_MASK_6      0x0000003f
#define BIT_MASK_7      0x0000007f
#define BIT_MASK_8      0x000000ff
#define BIT_MASK_9      0x000001ff
#define BIT_MASK_10     0x000003ff
#define BIT_MASK_11     0x000007ff
#define BIT_MASK_12     0x00000fff
#define BIT_MASK_13     0x00001fff

#define GET_BITS_VAL_FROM_INT32(intVal, offset, bitMask) ({(intVal >> (32-offset)) & bitMask;})
#define ADD_BITS_VAL_TO_INT32(intVal, bitVal, offset, bitMask) ({intVal | ((bitVal & bitMask) << (32 - offset));})

// ---------------------------
void calcRbByRiv(unsigned char bw, unsigned int riv, unsigned char* pRbStart, unsigned char* pNumOfRb)
{
    *pRbStart = riv % bw;
    *pNumOfRb = riv / bw + 1;

    unsigned int tmp = bw * bw + bw - 1;

    if ((*pRbStart + *pNumOfRb) >= bw) {
        *pRbStart = (tmp - riv) % bw;
        *pNumOfRb = (tmp - riv) / bw + 1;
    }
}

unsigned int calcRivByRb(unsigned char bw, unsigned char rbStart, unsigned char numOfRb)
{
    if ((numOfRb - 1) <= (bw >> 1)) {
        return ((numOfRb - 1) * bw + rbStart);
    } else {
        return ((bw - numOfRb + 1) * bw + (bw - 1 - rbStart));
    }
}

// ---------------------------
void decodeDci0(unsigned char bw, unsigned int dciData, UlDciMsg* pUlDciMsg)
{
    if (pUlDciMsg == 0) {
        return;
    }

    unsigned char offset = 1; 
    unsigned int riv = 0;

    unsigned char dciFlag = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_1);
    if (dciFlag != 0) {
        return;
    }

    offset += 1;
    pUlDciMsg->freqEnabledFlag = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_1);

    // riv
    switch (bw) {
        case N_RB_20M:
        {
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 13;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_13);
            } else {
                offset += 2;
                pUlDciMsg->freqHoppingBits = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_2);
                offset += 11;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_11);
            }
            break;
        }

        case N_RB_15M:
        {
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 12;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_12);
            } else {
                offset += 2;
                pUlDciMsg->freqHoppingBits = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_2);
                offset += 10;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_10);
            }
            break;
        }

        case N_RB_10M:
        {
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 11;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_11);
            } else {
                offset += 2;
                pUlDciMsg->freqHoppingBits = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_2);
                offset += 9;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_9);
            }
            break;
        }

        case N_RB_5M:
        {
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 9;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_9);
            } else {
                offset += 1;
                pUlDciMsg->freqHoppingBits = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_1);
                offset += 8;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_8);
            }
            break;
        }

        case N_RB_3M:
        {
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 7;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_7);
            } else {
                offset += 1;
                pUlDciMsg->freqHoppingBits = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_1);
                offset += 6;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_6);
            }
            break;
        }

        case N_RB_1P4M:
        {
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 5;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_5);
            } else {
                offset += 1;
                pUlDciMsg->freqHoppingBits = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_1);
                offset += 4;
                riv = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_4);
            }
            break;
        }

        default:
        {
            break;
        }
    }

    calcRbByRiv(bw, riv, &pUlDciMsg->rbStart, &pUlDciMsg->numOfRb);

    offset += 5;
    pUlDciMsg->mcs = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_5);

    offset += 1;
    pUlDciMsg->ndi = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_1);

    offset += 2;
    pUlDciMsg->tpc = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_2);

    offset += 3;
    pUlDciMsg->cyclicShift2forDMRS = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_3);

#ifdef LTE_TDD
    offset += 2;
    pUlDciMsg->ulIndexOrDAI = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_2);
#endif

    offset += 1;
    pUlDciMsg->cqiReq = GET_BITS_VAL_FROM_INT32(dciData, offset, BIT_MASK_1);
}

// ---------------------------
void encodeDci0(
    unsigned char bw, 
    UlDciMsg * pUlDciMsg, 
    unsigned int * pDci0Data, 
    unsigned int * pDci0BitLength)
{
    if (pUlDciMsg == 0 || pDci0Data == 0 || pDci0BitLength == 0) {
        return;
    }

    unsigned int encodedData = 0;
    unsigned char offset = 2;
    unsigned int riv = calcRivByRb(bw, pUlDciMsg->rbStart, pUlDciMsg->numOfRb);

    encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->freqEnabledFlag, offset, BIT_MASK_1);
    
    switch (bw) {
        case N_RB_20M:
        {      
#ifdef LTE_TDD   
            *pDci0BitLength = 31;
#else 
            *pDci0BitLength = 28;
#endif
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 13;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_13);
            } else {
                offset += 2;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->freqHoppingBits, offset, BIT_MASK_2);
                offset += 11;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_11);
            }
            break;
        }

        case N_RB_15M:
        {
#ifdef LTE_TDD   
            *pDci0BitLength = 30;
#else 
            *pDci0BitLength = 27;
#endif
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 12;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_12);
            } else {
                offset += 2;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->freqHoppingBits, offset, BIT_MASK_2);
                offset += 10;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_10);
            }
            break;
        }

        case N_RB_10M:
        {
#ifdef LTE_TDD   
            *pDci0BitLength = 29;
#else 
            *pDci0BitLength = 27;
#endif
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 11;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_11);
            } else {
                offset += 2;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->freqHoppingBits, offset, BIT_MASK_2);
                offset += 9;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_9);
            }
            break;
        }

        case N_RB_5M:
        {
#ifdef LTE_TDD   
            *pDci0BitLength = 27;
#else 
            *pDci0BitLength = 25;
#endif
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 9;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_9);
            } else {
                offset += 1;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->freqHoppingBits, offset, BIT_MASK_1);
                offset += 8;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_8);
            }
            break;
        }

        case N_RB_3M:
        {
#ifdef LTE_TDD   
            *pDci0BitLength = 25;
#else 
            *pDci0BitLength = 22;
#endif
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 7;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_7);
            } else {
                offset += 1;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->freqHoppingBits, offset, BIT_MASK_1);
                offset += 6;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_6);
            }
            break;
        }

        case N_RB_1P4M:
        {
#ifdef LTE_TDD   
            *pDci0BitLength = 23;
#else 
            *pDci0BitLength = 21;
#endif
            if (pUlDciMsg->freqEnabledFlag == 0) {
                offset += 5;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_5);
            } else {
                offset += 1;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->freqHoppingBits, offset, BIT_MASK_1);
                offset += 4;
                encodedData = ADD_BITS_VAL_TO_INT32(encodedData, riv, offset, BIT_MASK_4);
            }
            break;
        }

        default:
        {
            break;
        }
    }

    offset += 5;
    encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->mcs, offset, BIT_MASK_5);

    offset += 1;
    encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->ndi, offset, BIT_MASK_1);

    offset += 2;
    encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->tpc, offset, BIT_MASK_2);
    
    offset += 3;
    encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->cyclicShift2forDMRS, offset, BIT_MASK_3);

#ifdef LTE_TDD
    offset += 2;
    encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->ulIndexOrDAI, offset, BIT_MASK_2);
#endif

    offset += 1;
    encodedData = ADD_BITS_VAL_TO_INT32(encodedData, pUlDciMsg->cqiReq, offset, BIT_MASK_1);

    *pDci0Data = encodedData;
}
