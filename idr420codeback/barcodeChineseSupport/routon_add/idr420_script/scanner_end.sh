if [ $1 != "F10_key_end" ] && [ $1 == "restartbarcodeservice" ]
then
routon_client 7e8802662e5daf979af0a3a5266bcaa1 /system/bin/am broadcast -a  scanner_service_resume -n  com.geomobile.barcode/.BarcodeReceiver
fi
routon_test_gpio 83 1 1987 1
usleep 50000
routon_test_gpio 87 0 1122 0

