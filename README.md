# Cooling-Fan-Controller
For my project, I have implemented an RTOS to read temperature and to control the speed of fan based
on it. This is meant to serve as a demonstraon of air conditioning systems, which regulate the flow and
temperature of air being pumped into the room to regulate room temperature. The temperature of the
room is also displayed on the LCD in fixed point format.
A typical air condioning system is composed of several tasks that need to executed concurrently for
smooth operation. There needs to be a way of accurately esmang the temperature of the room, a
system to accurately control the speed of the motor and a user-interface for specifying the desired room
temperature. A simplified block diagram of such a system is shown below.
![image](https://github.com/harshitamhaske/Cooling-Fan-Controller/assets/70478573/13e02bc0-1138-4149-8bab-14d6ba691b27)
