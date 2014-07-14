#/bin/sh

SCAPY=http://bb.secdev.org/scapy-com/get/cc06add6dbd9.zip
TMPFILE=`mktemp`
wget $SCAPY -O $TMPFILE
unzip $TMPFILE
rm $TMPFILE
ln -s secdev-scapy-com-cc06add6dbd9/scapy ./scapy
patch -p1 < sixlowpan.patch
