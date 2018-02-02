if [ $1 != "F10_key_start" ] && [ $1 == "stopbarcodeservice" ]
then 
routon_client 7e8802662e5daf979af0a3a5266bcaa1 /system/bin/am broadcast -a  scanner_service_pause -n  com.geomobile.barcode/.BarcodeReceiver
fi
routon_test_gpio 83 1 1987 1
routon_test_gpio 87 1 1122 1
usleep 200000
routon_test_gpio 83 0 1122 0



