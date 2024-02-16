#!/bin/bash


echo "Starting to make UI"


# if qmake / make was run in the ui folder, it leaves artefacts that need to be removed
rm *.o
rm moc_*
rm ui_*
rm Makefile
# end of removal of artefacts

sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
# qmake "CONFIG+=releaselode" 



cd ..
# rm -fr build-ui-cli-debug
mkdir build-ui-cli-debug
cd build-ui-cli-debug
qmake ../ui/ui.pro -spec linux-g++ CONFIG+=debug
# qmake ../ui.pro
cp DF_UI ../ui
# qmake ../ui.pro -spec linux-g++ CONFIG+=debug CONFIG+=static




# rm -fr build-ui-cli-debug
# mkdir build-ui-cli-debug
# cd build-ui-cli-debug
# qmake ../ui.pro -spec linux-g++ CONFIG+=debug
# # qmake ../ui.pro
# cp DF_UI ..
# # qmake ../ui.pro -spec linux-g++ CONFIG+=debug CONFIG+=static

make --debug

echo "UI make complete"
