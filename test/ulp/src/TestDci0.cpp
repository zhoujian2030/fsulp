/*
 * TestDci0.h
 *
 *  Created on: Aug 3, 2018
 *      Author: j.zh
 */

#include "TestDci0.h"
#include <iostream>
#include <stdio.h>
#include "dci0.h"

using namespace std;

// -------------------------------
TEST_F(TestDci0, Decode_RIV) {
    unsigned int riv = 0;
    unsigned char rbStart = 0;
    unsigned char numOfRb = 1;
    unsigned char bw = 100;

    unsigned char decodedRbStart = 0;
    unsigned char decodedNumOfRb = 0;

    for (numOfRb = 1; numOfRb <= bw; numOfRb++) {
        for (rbStart = 0; rbStart < (bw - numOfRb); rbStart++) {
            riv = calcRivByRb(bw, rbStart, numOfRb);
            // printf("rbStart = %d, numOfRb = %d, riv = %d\n", rbStart, numOfRb, riv);

            calcRbByRiv(bw, riv, &decodedRbStart, &decodedNumOfRb);
            // printf("decodedRbStart = %d, decodedNumOfRb = %d\n", decodedRbStart, decodedNumOfRb);

            ASSERT_TRUE(decodedRbStart == rbStart);
            ASSERT_TRUE(decodedNumOfRb == numOfRb);
        }
    }

    rbStart = 16;
    numOfRb = 2;
    bw = 25;
    riv = calcRivByRb(bw, rbStart, numOfRb);
    printf("rbStart = %d, numOfRb = %d, riv = %d\n", rbStart, numOfRb, riv);

    calcRbByRiv(bw, riv, &decodedRbStart, &decodedNumOfRb);
    printf("decodedRbStart = %d, decodedNumOfRb = %d\n", decodedRbStart, decodedNumOfRb);
}

#ifdef LTE_FDD
// -------------------------------
TEST_F(TestDci0, Decode_Encode_DCI0_BW_10M_FDD) {
    unsigned int dci0Data = 0x2584A800;
    unsigned char bw = 50;
    UlDciMsg dci0Msg;

    unsigned int encodedDci0Data = 0;
    unsigned int encodedDci0BitLength = 0;

    // 1st dci0 
    memset((void*)&dci0Msg, 0, sizeof(dci0Msg));
    decodeDci0(bw, dci0Data, &dci0Msg);

    printf("---------------------------\n");
    printf("freqEnabledFlag     = %d\n", dci0Msg.freqEnabledFlag);
    printf("freqHoppingBits     = %d\n", dci0Msg.freqHoppingBits);
    printf("rbStart             = %d\n", dci0Msg.rbStart);
    printf("numOfRb             = %d\n", dci0Msg.numOfRb);
    printf("mcs                 = %d\n", dci0Msg.mcs);
    printf("ndi                 = %d\n", dci0Msg.ndi);
    printf("tpc                 = %d\n", dci0Msg.tpc);
    printf("cyclicShift2forDMRS = %d\n", dci0Msg.cyclicShift2forDMRS);
    printf("cqiReq              = %d\n", dci0Msg.cqiReq);

    ASSERT_EQ(0, dci0Msg.freqEnabledFlag);
    ASSERT_EQ(0, dci0Msg.freqHoppingBits);
    ASSERT_EQ(0, dci0Msg.rbStart);
    ASSERT_EQ(25, dci0Msg.numOfRb);
    ASSERT_EQ(18, dci0Msg.mcs);
    ASSERT_EQ(1, dci0Msg.ndi);
    ASSERT_EQ(1, dci0Msg.tpc);
    ASSERT_EQ(0, dci0Msg.cyclicShift2forDMRS);
    ASSERT_EQ(0, dci0Msg.cqiReq);

    // encode dci0
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data);
    ASSERT_TRUE(27==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // 2nd dci0 
    dci0Data = 0x48720800;

    memset((void*)&dci0Msg, 0, sizeof(dci0Msg));
    decodeDci0(bw, dci0Data, &dci0Msg);

    printf("---------------------------\n");
    printf("freqEnabledFlag     = %d\n", dci0Msg.freqEnabledFlag);
    printf("freqHoppingBits     = %d\n", dci0Msg.freqHoppingBits);
    printf("rbStart             = %d\n", dci0Msg.rbStart);
    printf("numOfRb             = %d\n", dci0Msg.numOfRb);
    printf("mcs                 = %d\n", dci0Msg.mcs);
    printf("ndi                 = %d\n", dci0Msg.ndi);
    printf("tpc                 = %d\n", dci0Msg.tpc);
    printf("cyclicShift2forDMRS = %d\n", dci0Msg.cyclicShift2forDMRS);
    printf("cqiReq              = %d\n", dci0Msg.cqiReq);    

    ASSERT_EQ(1, dci0Msg.freqEnabledFlag);
    ASSERT_EQ(0, dci0Msg.freqHoppingBits);
    ASSERT_EQ(20, dci0Msg.rbStart);
    ASSERT_EQ(6, dci0Msg.numOfRb);
    ASSERT_EQ(8, dci0Msg.mcs);
    ASSERT_EQ(0, dci0Msg.ndi);
    ASSERT_EQ(1, dci0Msg.tpc);
    ASSERT_EQ(0, dci0Msg.cyclicShift2forDMRS);
    ASSERT_EQ(0, dci0Msg.cqiReq);

    // encode dci0
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data);
    ASSERT_TRUE(27==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // 3rd dci0 
    dci0Data = 0x07D7E800;

    memset((void*)&dci0Msg, 0, sizeof(dci0Msg));
    decodeDci0(bw, dci0Data, &dci0Msg);

    printf("---------------------------\n");
    printf("freqEnabledFlag     = %d\n", dci0Msg.freqEnabledFlag);
    printf("freqHoppingBits     = %d\n", dci0Msg.freqHoppingBits);
    printf("rbStart             = %d\n", dci0Msg.rbStart);
    printf("numOfRb             = %d\n", dci0Msg.numOfRb);
    printf("mcs                 = %d\n", dci0Msg.mcs);
    printf("ndi                 = %d\n", dci0Msg.ndi);
    printf("tpc                 = %d\n", dci0Msg.tpc);
    printf("cyclicShift2forDMRS = %d\n", dci0Msg.cyclicShift2forDMRS);
    printf("cqiReq              = %d\n", dci0Msg.cqiReq);    

    ASSERT_EQ(0, dci0Msg.freqEnabledFlag);
    ASSERT_EQ(0, dci0Msg.freqHoppingBits);
    ASSERT_EQ(0, dci0Msg.rbStart);
    ASSERT_EQ(6, dci0Msg.numOfRb);
    ASSERT_EQ(31, dci0Msg.mcs);
    ASSERT_EQ(1, dci0Msg.ndi);
    ASSERT_EQ(1, dci0Msg.tpc);
    ASSERT_EQ(0, dci0Msg.cyclicShift2forDMRS);
    ASSERT_EQ(0, dci0Msg.cqiReq);

    // encode dci0
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data);
    ASSERT_TRUE(27==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;
}
#endif

// -------------------------------
TEST_F(TestDci0, Encode_Decode_DCI0) {
    unsigned char bw = 100;
    UlDciMsg dci0Msg = {
        0,
        0,
        16,
        2,
        10,
        1,
        3,
        0,
#ifdef LTE_TDD
        3,
#endif
        0
    };

    unsigned int encodedDci0Data = 0;
    unsigned int encodedDci0BitLength = 0;
    UlDciMsg decodedDci0Msg = {0};

    // 20M
#ifdef LTE_TDD
    unsigned int dci0Data = 0x00e8ae30;
    unsigned int dci0BitLength = 31;
#else 
    unsigned int dci0Data = 0x00e8ae00;
    unsigned int dci0BitLength = 28;
#endif
    // encode
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data) << "encodedDci0Data = 0x" << hex << encodedDci0Data;;
    ASSERT_TRUE(dci0BitLength==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // decode
    decodeDci0(bw, encodedDci0Data, &decodedDci0Msg);
    ASSERT_TRUE(memcmp((void*)&decodedDci0Msg, (void*)&dci0Msg, sizeof(UlDciMsg)) == 0);

    // 15M
    bw = 75;
#ifdef LTE_TDD
    dci0Data = 0x016d5c60;
    dci0BitLength = 30;
#else 
    dci0Data = 0x016d5c00;
    dci0BitLength = 27;
#endif
    // encode
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data) << "encodedDci0Data = 0x" << hex << encodedDci0Data;;
    ASSERT_TRUE(dci0BitLength==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // decode
    decodeDci0(bw, encodedDci0Data, &decodedDci0Msg);
    ASSERT_TRUE(memcmp((void*)&decodedDci0Msg, (void*)&dci0Msg, sizeof(UlDciMsg)) == 0);

    // 10M
    bw = 50;
#ifdef LTE_TDD
    dci0Data = 0x0212b8c0;
    dci0BitLength = 29;
#else 
    dci0Data = 0x0212b800;
    dci0BitLength = 27;
#endif
    // encode
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data) << "encodedDci0Data = 0x" << hex << encodedDci0Data;;
    ASSERT_TRUE(dci0BitLength==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // decode
    decodeDci0(bw, encodedDci0Data, &decodedDci0Msg);
    ASSERT_TRUE(memcmp((void*)&decodedDci0Msg, (void*)&dci0Msg, sizeof(UlDciMsg)) == 0);

    // 5M
    bw = 25;
#ifdef LTE_TDD
    dci0Data = 0x052ae300;
    dci0BitLength = 27;
#else 
    dci0Data = 0x052ae000;
    dci0BitLength = 25;
#endif
    // encode
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data) << "encodedDci0Data = 0x" << hex << encodedDci0Data;;
    ASSERT_TRUE(dci0BitLength==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // decode
    decodeDci0(bw, encodedDci0Data, &decodedDci0Msg);
    ASSERT_TRUE(memcmp((void*)&decodedDci0Msg, (void*)&dci0Msg, sizeof(UlDciMsg)) == 0);

    // 3M
    bw = 15;
    dci0Msg.rbStart = 3;
#ifdef LTE_TDD
    dci0Data = 0x092b8c00;
    dci0BitLength = 25;
#else 
    dci0Data = 0x092b8000;
    dci0BitLength = 22;
#endif
    // encode
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data) << "encodedDci0Data = 0x" << hex << encodedDci0Data;;
    ASSERT_TRUE(dci0BitLength==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // decode
    decodeDci0(bw, encodedDci0Data, &decodedDci0Msg);
    ASSERT_TRUE(memcmp((void*)&decodedDci0Msg, (void*)&dci0Msg, sizeof(UlDciMsg)) == 0);

    // 1.4M
    bw = 6;
#ifdef LTE_TDD
    dci0Data = 0x12ae3000;
    dci0BitLength = 23;
#else 
    dci0Data = 0x12ae0000;
    dci0BitLength = 21;
#endif
    // encode
    encodeDci0(bw, &dci0Msg, &encodedDci0Data, &encodedDci0BitLength);
    printf("encodedDci0Data = 0x%x, encodedDci0BitLength = %d\n", encodedDci0Data, encodedDci0BitLength);
    ASSERT_EQ(encodedDci0Data, dci0Data) << "encodedDci0Data = 0x" << hex << encodedDci0Data;;
    ASSERT_TRUE(dci0BitLength==encodedDci0BitLength) << "encodedDci0BitLength = " << encodedDci0BitLength;

    // decode
    decodeDci0(bw, encodedDci0Data, &decodedDci0Msg);
    ASSERT_TRUE(memcmp((void*)&decodedDci0Msg, (void*)&dci0Msg, sizeof(UlDciMsg)) == 0);
}
