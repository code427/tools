#git log|grep -m 1 Date|sed -f version.sed |awk '{printf("%02d%02d%02d\n", $6,$3,$4)}'
#export CPATH="/opt/arm-2014.05/arm-none-linux-gnueabi/libc/usr/include/"
#export CRYPTO_INC="/study/code/github/exboot/ex_lib/include/"
#export CRYPTO_LIBS="-L /study/code/github/exboot/ex_lib/ -lcrypto"

make CROSS_COMPILE=/opt/arm-2014.05/bin/arm-none-linux-gnueabi- ARCH=arm -j5 -s 
