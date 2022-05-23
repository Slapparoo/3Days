#/bin/tcsh
set STAGEDIR = `pwd`/debian
mkdir -p ${STAGEDIR}
mkdir -p ${STAGEDIR}/usr
mkdir -p ${STAGEDIR}/usr/local
mkdir -p ${STAGEDIR}/usr/local/bin
mkdir -p ${STAGEDIR}/usr/local/include/3Days
cp 3d_loader ${STAGEDIR}/usr/local/bin/3d
strip ${STAGEDIR}/usr/local/bin/3d
chmod +x ${STAGEDIR}/usr/local/bin/3d
set files = ( \
    HCRT.BIN \
    `find T ` \
    )
rm ${STAGEDIR}/plist
echo "/usr/local/bin/3d" >> ${STAGEDIR}/plist

foreach f ($files)
    if -d $f then
        mkdir -p ${STAGEDIR}/usr/local/include/3Days/$f
    else
        cp  $f  ${STAGEDIR}/usr/local/include/3Days/
	echo "/usr/local/include/3Days/$f" >> ${STAGEDIR}/plist
    endif
end

dpkg-deb --build debian 3days.deb
