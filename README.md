# nqmIOTHub
Component to proxy sensor data to nqmHub

## installation - Raspberry Pi

First, install nodejs on your RPi (see [http://elinux.org/Node.js_on_RPi](http://elinux.org/Node.js_on_RPi)): 

```
wget http://nodejs.org/dist/v0.10.28/node-v0.10.28-linux-arm-pi.tar.gz
tar -xzf node-v0.10.28-linux-arm-pi.tar.gz
node-v0.10.28-linux-arm-pi/bin/node --version
```

Then install the nqminds IOT Hub:

```
git clone https://github.com/nqminds/nqmIOTHub
cd nqmIOTHub
npm install
```

## configuration

You will need a hub Id and a feed Id token for each data stream you intend to publish. You create these in your
NQM Hub (or ask Toby).

Copy the config.local.example.json to config.local.json, and then edit the file.

```
cp config.local.example.json config.local.json
nano config.local.json
```

Change the sync config section to reflect the destination Hub:

 ```
 {
   "syncConfig": {
     "syncType": "nqmHTTPSync",
     "syncServer": "<insert sync server host>",
     "hubId": "<insert hub Id>",
     ...
```

Add/Remove drivers from the drivers section to reflect the sensors you have installed. Remember to add a feed Id 
for each data stream, and make sure the 'port' setting is correct for your device:

```
 "drivers": [
   {
     "id": "c/t/h",
     "type": "COZIR",
     "port": "<insert the port of your device, e.g. /dev/ttyUSB0>",
     "cozirPollInterval": 1,
     "feedId": "<insert feed id>"
   },
   {
     "type": "OEM",
     "port": "<insert the port of your device, e.g. /dev/ttyAMA0>",
     "oemNetwork": 210,
```

## drivers
You need to install a driver for each of your devices. Drivers are available at [https://github.com/nqminds](https://github.com/nqminds). 

For example, if you are using a COZIR CO2 sensor, install the COZIR driver:

```
npm install git://github.com/nqminds/nqmCOZIRDriver
```

## running

Now start the hub using:
```
node index
```