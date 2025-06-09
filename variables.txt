Add this 5 vaiables in you Arduino clound:

read_time
Declaration: CloudTime read_time
Type: Time
Variable Permission: Read Only
Update Policy: On change Threshold 0

Brightness
Declaration: int brightness
Type: Integer number
Variable Permission: Read & Write
Update Policy: On change Threshold 0

Temperature
Declaration: CloudTemperature temperature
Type: Temperature
Variable Permission: Read Only
Update Policy: Periodic Every 60 s

Humidity
Declaration: CloudRelativeHumidity humidity
Type: Relative humidity
Variable Permission: Read Only
Update Policy: Periodic Every 60 s

Planif
Declaration: CloudSchedule planif
Type: Schedule
Variable Permission: Read & Write
Update Policy: On change
