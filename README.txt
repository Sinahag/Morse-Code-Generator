Assumptions of what has been done prior to running:

    echo start | sudo tee /sys/class/remoteproc/remoteproc1/state

Need to also config pins P9_17 and P9_18 to i2c
    config-pin P9_17 i2c
    config-pin P9_18 i2c
