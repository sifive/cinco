Steps for testing Software I2C:

1. Hardware Wire library - Master (UNO) to Hardware Wire library - Slave (UNO).
This is done by wiring pins Master --> Slave: A4(SDA) --> A4(SDA), A5(SCL) --> A5(SCL), GND --> GND.
Then you will need RecieverSlave (Wire Lib)  and also MasterTransmit (Wire Lib) .

2. Software Wire library - Master (UNO) to Hardware Wire library - Slave (UNO).
This is done by the same wiring but this time a setup is required on the Master.
Then you will need RecieverSlave (Wire Lib) and also SW_I2C_Uno_TransmitterMaster.

3. Software Wire library - Master (SiFi) to Hardware Wire library - Slave (UNO).
This is done by wiring pins Master --> Slave: 18(SDA) --> A4(SDA), 19(SCL) --> A5(SCL), GND --> GND.
Then you will need RecieverSlave (Wire Lib) and also SW_I2C_SiFi_TransmitterMaster.