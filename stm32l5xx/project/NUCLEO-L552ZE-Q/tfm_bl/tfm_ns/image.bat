fromelf.exe --bincombined --output=Objects\tfm_ns.bin Objects\tfm_ns.axf
python ../scripts/imgtool.py sign -l ../tfm_ns/layout.txt -k root-rsa-3072_1.pem -K full --align 1 -v 5.6.7+8 -d "(0,1.2.3+0)" -s 54 -H 0x400 Objects/tfm_ns.bin Objects/tfm_ns_signed.bin
srec_cat Objects\tfm_ns_signed.bin -Binary -offset 0x08030000 -o Objects/tfm_ns.hex -Intel
