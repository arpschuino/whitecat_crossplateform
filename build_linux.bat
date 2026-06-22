@echo off
wsl -d Ubuntu-22.04 -u root bash -c "apt-get update -qq && apt-get install -y build-essential libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev libasound2-dev libftdi1-dev libhpdf-dev zlib1g-dev && cd /mnt/c/Nextcloud/ARPSCHUINO_PARTAGE/codes/whitecat_crossplateform && make -f Makefile.linux clean && make -f Makefile.linux -j4"
