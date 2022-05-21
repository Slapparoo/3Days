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
mkdir -p ${STAGEDIR}/usr/local/include/3Days
cp 3d_loader ${STAGEDIR}/usr/local/bin/3d
strip ${STAGEDIR}/usr/local/bin/3d
chmod +x ${STAGEDIR}/usr/local/bin/3d
set files = ( \
    HCRT.BIN \
    "T" \
    )
rm ${STAGEDIR}/plist
echo "/usr/local/bin/3d" >> ${STAGEDIR}/plist

foreach f ($files)
    if -d $f then
    	cp -r $f ${STAGEDIR}/usr/local/include/3Days
    	set tmp = ` find T` 
    	foreach t ( $tmp )
    	    if ! -d $t then
    	        echo "/usr/local/include/3Days/$t" >> ${STAGEDIR}/plist
    	    endif
    	end
    else
    	cp  $f  ${STAGEDIR}/usr/local/include/3Days/
    	echo "/usr/local/include/3Days/$f" >> ${STAGEDIR}/plist
    endif
end
pkg create -m ${STAGEDIR}/ -r ${STAGEDIR}/ -p ${STAGEDIR}/plist -o .
