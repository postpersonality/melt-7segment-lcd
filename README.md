# melt-7segment-lcd
Melt 7-segment LCD Arduino library

Compatible models are [MT-10T* based on I2C PCF8576](http://www.melt.com.ru/en/shop/indikatory-zhk/segmentnye-zhk-indikatory.html).

## Features
* Extended table of chars `0-9`, `A-Z`, `a-z`, special chars ` -=_~*."`
* Dot is dispayed as modifier to a char
* Replacable/customizable char mapper
* The char map is stored in PROGMEM
* The bundled char mapper is optimized for speed
* Smart transmission planning

The library tries to optimize transmission by sending only the changed parts but only if distance between them is 3 or more. The initializaton of i2c communication has overhead that is close to the cost of sending 2 chars in bulk.
