Steps for testing Software I2C:

1. Hardware Wire library - Master (UNO) to Hardware Wire library - Slave (UNO).
This is done by wiring pins Master --> Slave: A4 --> A4, A5 --> A5, GND --> GND.
Then you will need HW_I2C_Uno_RecieverSlave and also HW_I2C_Uno_TransmitterMaster.

2. Software Wire library - Master (UNO) to Hardware Wire library - Slave (UNO).
This is done by the same wiring but this time a setup is required on the Master.
Then you will need HW_I2C_Uno_RecieverSlave and also SW_I2C_Uno_TransmitterMaster.

3. Software Wire library - Master (SiFi) to Hardware Wire library - Slave (UNO).
This is done by wiring pins Master --> Slave: 18(SDA) --> A4, 19(SCL) --> A5, GND --> GND.
Then you will need HW_I2C_Uno_RecieverSlave and also SW_I2C_SiFi_TransmitterMaster.