
#ifndef DCI0_H
#define DCI0_H

#ifdef __cplusplus
extern "C" {
#endif

#define N_RB_1P4M	6
#define N_RB_3M	    15
#define N_RB_5M		25
#define N_RB_10M	50
#define N_RB_15M	75
#define N_RB_20M	100 

typedef struct {
    unsigned char freqEnabledFlag;
    unsigned char freqHoppingBits;
    unsigned char rbStart;
    unsigned char numOfRb;
    unsigned char mcs;
    unsigned char ndi;
    unsigned char tpc;
    unsigned char cyclicShift2forDMRS;
#ifdef LTE_TDD
    // dl asignment index range: 0 ~ 3
    unsigned char ulIndexOrDAI;
#endif
    unsigned char cqiReq;
} UlDciMsg;

void decodeDci0(unsigned char bw, unsigned int dciData, UlDciMsg* pUlDciMsg);
void encodeDci0(unsigned char bw, UlDciMsg * pUlDciMsg, unsigned int * pDci0Data, unsigned int * pDci0BitLength);
void calcRbByRiv(unsigned char bw, unsigned int riv, unsigned char* pRbStart, unsigned char* pNumOfRb);
unsigned int calcRivByRb(unsigned char bw, unsigned char rbStart, unsigned char numOfRb);

#ifdef __cplusplus
}
#endif

#endif
