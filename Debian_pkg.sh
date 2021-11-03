#/bin/tcsh 
set STAGEDIR = `pwd`/debian
mkdir -p ${STAGEDIR}
mkdir -p ${STAGEDIR}/usr
mkdir -p ${STAGEDIR}/usr/local
mkdir -p ${STAGEDIR}/usr/local/bin
mkdir -p ${STAGEDIR}/usr/local/include/HolyC
cp 3d ${STAGEDIR}/usr/local/bin/
set files = ( \
    ASM.HC \
    BITS.HC \
    COMPRESS.HC \
    FILE.HC \
    HASH.HC \
    Opcodes.DD \
    CHAR.HC \
    FIFO.HC \
    FS.HC \
    HCRT.HC \
    MATH.HC \
    QSORT.HC \
    )
rm ${STAGEDIR}/plist
echo "/usr/local/bin/3d" >> ${STAGEDIR}/plist

foreach f ($files)
    cp  HCRT/$f  ${STAGEDIR}/usr/local/include/HolyC/
end

dpkg-deb --build debian 3days.deb
