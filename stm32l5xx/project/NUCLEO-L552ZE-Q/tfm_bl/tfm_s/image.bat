fromelf.exe --bincombined --output=Objects\tfm_s.bin Objects\tfm_s.axf
python ../scripts/imgtool.py sign -l ../tfm_s/layout.txt -k root-rsa-3072.pem -K full --align 1 -v 1.2.3+4 -s 42 -H 0x400 Objects/tfm_s.bin Objects/tfm_s_signed.bin
srec_cat Objects\tfm_s_signed.bin -Binary -offset 0x0C010000 -o Objects/tfm_s.hex -Intel
