# Required Variables

**read_time** → used to store the current time (format HH:MM)

| Field                 | Value                          |
|-----------------------|--------------------------------|
| *Declaration*         | CloudTime read_time            |
| *Type*                | Time                           |
| *Variable Permission* | Read Only                      |
| *Update Policy*       | On change Threshold 0          |

**brightness** → controls the brightness level of the display (0 to 9)

| Field                 | Value                          |
|-----------------------|--------------------------------|
| *Declaration*         | int brightness                 |
| *Type*                | Integer                        |
| *Variable Permission* | Read & Write                   |
| *Update Policy*       | On change Threshold 0          |

**temperature** → stores the current temperature in °C

| Field                 | Value                          |
|-----------------------|--------------------------------|
| *Declaration*         | CloudTemperature temperature   |
| *Type*                | Temperature                    |
| *Variable Permission* | Read Only                      |
| *Update Policy*       | Periodic Every 60 s            |

**humidity** → stores the current humidity in %RH

| Field                 | Value                          |
|-----------------------|--------------------------------|
| *Declaration*         | CloudRelativeHumidity humidity |
| *Type*                | Relative Humidity              |
| *Variable Permission* | Read Only                      |
| *Update Policy*       | Periodic Every 60 s            |

**planif** → defines the schedule when the display should turn off at night

| Field                 | Value                          |
|-----------------------|--------------------------------|
| *Declaration*         | CloudSchedule planif           |
| *Type*                | Schedule                       |
| *Variable Permission* | Read & Write                   |
| *Update Policy*       | On change                      |
