#include "TestSuite.h"

unsigned char gLogLevel = 0;

int main(int argc, char **argv) {  
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
