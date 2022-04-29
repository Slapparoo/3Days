#/bin/tcsh
set STAGEDIR = `pwd`/stagedir
if ! -e $STAGEDIR then
	mkdir stagedir
endif
cp "+MANIFEST" ${STAGEDIR}/
mkdir -p ${STAGEDIR}
mkdir -p ${STAGEDIR}/usr
mkdir -p ${STAGEDIR}/usr/local
mkdir -p ${STAGEDIR}/usr/local/bin
mkdir -p ${STAGEDIR}/usr/local/include/HolyC
cp 3d_loader2 ${STAGEDIR}/usr/local/bin/3d
strip ${STAGEDIR}/usr/local/bin/3d
chmod +x ${STAGEDIR}/usr/local/bin/3d
set files = ( \
    HCRT_TOS.BIN \
    )
rm ${STAGEDIR}/plist
echo "/usr/local/bin/3d" >> ${STAGEDIR}/plist

foreach f ($files)
    cp  HCRT/$f  ${STAGEDIR}/usr/local/include/HolyC/
    echo "/usr/local/include/HolyC/$f" >> ${STAGEDIR}/plist

end
pkg create -m ${STAGEDIR}/ -r ${STAGEDIR}/ -p ${STAGEDIR}/plist -o .
